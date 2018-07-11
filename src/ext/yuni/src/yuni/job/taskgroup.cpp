/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#include "taskgroup.h"
#include "queue/service.h"




namespace Yuni
{
namespace Job
{

	class YUNI_DECL Taskgroup::ITaskgroupJob : public Job::IJob
	{
	public:
		//! The threading policy
		typedef Job::IJob::ThreadingPolicy ThreadingPolicy;


	public:
		ITaskgroupJob(QueueService& queueservice)
			: taskgroup(nullptr)
			, queueservice(queueservice)
		{}

		virtual ~ITaskgroupJob() {}


	public:
		//! The attached task (if running)
		Taskgroup* taskgroup;
		//! The attached queueservice
		QueueService& queueservice;


	protected:
		void onTerminate(bool success)
		{
			ThreadingPolicy::MutexLocker locker(*this);
			if (taskgroup) // notify that the job has finished
			{
				taskgroup->onJobTerminated(*this, success);
				taskgroup = nullptr;
			}
		}

	}; // class TaskgroupJob




	class YUNI_DECL TaskgroupJobCallback final : public Taskgroup::ITaskgroupJob
	{
	public:
		//! The threading policy
		typedef Job::IJob::ThreadingPolicy ThreadingPolicy;


	public:
		TaskgroupJobCallback(QueueService& queueservice, const Bind<bool (IJob&)>& callback)
			: Taskgroup::ITaskgroupJob(queueservice)
			, callback(callback)
		{}

		virtual ~TaskgroupJobCallback() {}


	public:
		//! The callback to call
		Bind<bool (IJob&)> callback;

	protected:
		virtual void onExecute() override
		{
			bool success = false;
			try { success = callback(*this); } catch (...) {}
			onTerminate(success);
		}

	}; // class TaskgroupJob













	Taskgroup::Taskgroup(QueueService& queueservice, bool cancelOnError, bool autostart)
		: pTaskHasStarted(false)
		, pTaskStatus(stSucceeded) // nothing to do by default, so success
		, pJobsDoneCount(0)
		, pCancelOnError(cancelOnError)
		, pAutostart(autostart)
		, pDefaultQueueservice(queueservice)
	{}


	Taskgroup::~Taskgroup()
	{
		cancel();
	}


	inline void Taskgroup::deleteAllJobsWL()
	{
		uint jobCount = static_cast<uint>(pJobs.size());
		for (uint i = 0; i != jobCount; ++i)
		{
			ITaskgroupJob* job = pJobs[i];
			job->taskgroup = nullptr; // should be useless and already done
			if (job->release())
				delete job;
		}

		JobList clearAndMinimize; // force complete cleanup
		clearAndMinimize.swap(pJobs);
		pJobsDoneCount = 0;
	}


	inline void Taskgroup::onTaskStoppedWL()
	{
		// checking 'pTaskHasStarted' just in case it would have already been stopped
		if (pTaskHasStarted)
		{
			// trigger events
			on.terminated(pTaskStatus);
			// mark as stopped *after* calling the event for being consistent with onJobTerminated
			pTaskHasStarted = false;

			// notify waiters
			pSignalTaskStopped.notify();
		}
	}


	inline void Taskgroup::stopAllJobsWL()
	{
		// remove all waiting jobs
		uint jobCount = static_cast<uint>(pJobs.size());
		if (jobCount != 0)
		{
			bool atLeastOneJobWasAlive = false;

			for (uint i = 0; i != jobCount; ++i)
			{
				ITaskgroupJob& job = *(pJobs[i]);

				ITaskgroupJob::ThreadingPolicy::MutexLocker joblocker(job);
				if (job.taskgroup)
				{
					job.taskgroup = nullptr;
					job.cancel();
					atLeastOneJobWasAlive = true;
				}
			}

			if (atLeastOneJobWasAlive)
				onTaskStoppedWL();

			// resource cleanup
			deleteAllJobsWL();
		}
	}


	inline void Taskgroup::onJobTerminated(Taskgroup::ITaskgroupJob&, bool success)
	{
		ThreadingPolicy::MutexLocker locker(*this);
		if (not pTaskHasStarted) // should never happen
			return;

		// we may need to stop at the first encoutered error
		bool forceCancel = (pCancelOnError and (not success and pTaskStatus == stSucceeded));
		// update the status of the task
		if (not success and (pTaskStatus == stSucceeded)) // no update if == stCanceled for example
			pTaskStatus = stFailed;

		if (++pJobsDoneCount == (uint) pJobs.size() or forceCancel)
		{
			onTaskStoppedWL();
			deleteAllJobsWL();
		}
	}


	inline void Taskgroup::startWL()
	{
		pTaskHasStarted = true;
		pTaskStatus = stSucceeded; // success by default
		on.started();

		if (not pJobs.empty())
		{
			pSignalTaskStopped.reset();
			pJobsDoneCount = 0;

			for (uint i = 0; i != (uint) pJobs.size(); ++i)
			{
				ITaskgroupJob* job = pJobs[i];
				job->taskgroup = this;
				job->queueservice.add(job);
			}
		}
		else
		{
			pTaskHasStarted = false;
			on.terminated(stSucceeded);
		}
	}


	void Taskgroup::start()
	{
		ThreadingPolicy::MutexLocker locker(*this);
		if (not pTaskHasStarted)
			startWL();
	}


	void Taskgroup::cancel()
	{
		ThreadingPolicy::MutexLocker locker(*this);
		pTaskStatus = stCanceled;
		if (pTaskHasStarted)
			stopAllJobsWL();
	}


	void Taskgroup::markAsSucceeded()
	{
		ThreadingPolicy::MutexLocker locker(*this);
		pTaskStatus = stSucceeded;
		if (pTaskHasStarted)
			stopAllJobsWL();
	}


	void Taskgroup::markAsFailed()
	{
		ThreadingPolicy::MutexLocker locker(*this);
		pTaskStatus = stFailed;
		if (pTaskHasStarted)
			stopAllJobsWL();
	}



	void Taskgroup::add(QueueService& queueservice, const Bind<bool (IJob&)>& callback)
	{
		ITaskgroupJob* job = new TaskgroupJobCallback(queueservice, callback);
		job->addRef();

		// register the new job
		{
			ThreadingPolicy::MutexLocker locker(*this);
			// resize the job list
			pJobs.push_back(job);

			// get if the job should be automatically added to the queue
			if (not pTaskHasStarted)
			{
				// if the task is not running, starting it as if the user did it itself
				if (pAutostart)
					startWL();
				return;
			}

			job->taskgroup = this;
		}

		queueservice.add(job);
	}


	Taskgroup::Status Taskgroup::wait()
	{
		// checking if not already stopped
		{
			ThreadingPolicy::MutexLocker locker(*this);
			if (not pTaskHasStarted)
				return pTaskStatus;
		}

		// wait
		pSignalTaskStopped.wait();

		// getting the return value
		ThreadingPolicy::MutexLocker locker(*this);
		assert(pTaskHasStarted == false and "the task should have already been stopped");
		return pTaskStatus;
	}


	Taskgroup::Status Taskgroup::wait(uint timeout)
	{
		// checking if not already stopped
		{
			ThreadingPolicy::MutexLocker locker(*this);
			if (not pTaskHasStarted)
				return pTaskStatus;
		}

		if (pSignalTaskStopped.wait(timeout)) // received notification
		{
			// getting the return value
			ThreadingPolicy::MutexLocker locker(*this);
			assert(pTaskHasStarted == false and "the task should have already been stopped");
			return pTaskStatus;
		}

		// the timeout has been reached
		return stRunning;
	}


	Taskgroup::Status Taskgroup::status(uint* jobCount, uint* doneCount) const
	{
		ThreadingPolicy::MutexLocker locker(*this);
		if (jobCount != nullptr)
			*jobCount = (uint) pJobs.size();
		if (doneCount != nullptr)
			*doneCount = pJobsDoneCount;

		return (not pTaskHasStarted) ? pTaskStatus : stRunning;
	}





} // namespace Job
} // namespace Yuni
