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
#include "service.h"
#include "../../core/system/cpu.h"
#include "../../thread/array.h"
#include "../../private/jobs/queue/thread.h"
#ifndef YUNI_OS_WINDOWS
	#include <unistd.h>
#endif


namespace Yuni
{
namespace Job
{

	enum
	{
		maxNumberOfThreads = 512,
	};


	typedef Yuni::Thread::Array<Yuni::Private::QueueService::QueueThread>  ThreadArray;



	static inline uint OptimalCPUCount()
	{
		uint count = System::CPU::Count();
		if (count < 1)
			return 1;
		if (count > 2)
			return count - 1;
		return count;
	}







	QueueService::QueueService()
		: pStatus(sStopped)
		, pThreads(NULL)
	{
		uint count = OptimalCPUCount();
		pMinimumThreadCount = count;
		pMaximumThreadCount = count;
	}


	QueueService::QueueService(bool autostart)
		: pStatus(sStopped)
		, pThreads(NULL)
	{
		uint count = OptimalCPUCount();
		pMinimumThreadCount = count;
		pMaximumThreadCount = count;

		if (autostart)
			start();
	}


	QueueService::~QueueService()
	{
		// making sure that the queueservice is stopped before being destroyed
		stop();
	}


	bool QueueService::maximumThreadCount(uint count)
	{
		if (count > maxNumberOfThreads) // hard-coded value
			return false;
		if (0 == count) // default value
			count = OptimalCPUCount();

		MutexLocker locker(*this);

		// checking for the lower bound
		if (pMinimumThreadCount > count)
			pMinimumThreadCount = count;
		// reseting the upper bound
		pMaximumThreadCount = count;
		return true;
	}


	uint QueueService::maximumThreadCount() const
	{
		MutexLocker locker(*this);
		return pMaximumThreadCount;
	}


	bool QueueService::minimumThreadCount(uint count)
	{
		if (count > maxNumberOfThreads) // hard-coded value
			return false;
		if (0 == count)
			count = OptimalCPUCount();

		MutexLocker locker(*this);

		// checking for the upper bound
		if (pMaximumThreadCount < count)
			pMaximumThreadCount = count;
		// reseting the lower bound
		pMinimumThreadCount = count;
		return true;
	}


	uint QueueService::minimumThreadCount() const
	{
		MutexLocker locker(*this);
		return pMinimumThreadCount;
	}


	std::pair<uint,uint> QueueService::minmaxThreadCount() const
	{
		MutexLocker locker(*this);
		return std::pair<uint, uint>(pMinimumThreadCount, pMaximumThreadCount);
	}


	bool QueueService::minmaxThreadCount(const std::pair<uint, uint>& values)
	{
		uint maxv = values.second;
		if (maxv > maxNumberOfThreads)
			return false;
		uint minv = values.first;
		if (minv > maxv)
			minv = maxv;
		if (maxv == 0)
		{
			maxv = OptimalCPUCount();
			minv = maxv;
		}

		MutexLocker locker(*this);
		pMinimumThreadCount = minv;
		pMaximumThreadCount = maxv;
		return true;
	}


	bool QueueService::start()
	{
		MutexLocker locker(*this);
		if (YUNI_LIKELY(pStatus == sStopped))
		{
			pSignalAllThreadHaveStopped.reset();
			pSignalShouldStop.reset();

			delete (ThreadArray*) pThreads;
			pThreads = new ThreadArray();

			// alias to the thread pool
			ThreadArray& array = *((ThreadArray*) pThreads);

			// recreate the thread pool
			// adding the minimum number of threads
			array.clear();
			for (uint i = 0; i != pMinimumThreadCount; ++i)
				array += new Yuni::Private::QueueService::QueueThread(*this);

			// Start all threads at once
			array.start();

			// Ok now we have started
			pStatus = sRunning;
		}
		return true;
	}


	void QueueService::stop(uint timeout)
	{
		ThreadArray* threads; // the thread pool

		// getting the thread pool
		{
			MutexLocker locker(*this);
			if (pStatus != sRunning)
				return;

			threads  = (ThreadArray*) pThreads;
			pThreads = NULL;
			pStatus  = sStopping;
		}

		// Destroying the thread pool
		if (YUNI_LIKELY(threads))
		{
			// stopping all threads (**before** deleting them)
			threads->stop(timeout);
			delete threads;
		}

		MutexLocker locker(*this);
		// marking the queue service as stopped, just in case
		// (thread workers should already marked us as 'stopped')
		pStatus = sStopped;

		// signalling that the queueservice is stopped. This signal
		// will come after pSignalAllThreadHaveStopped in this case
		pSignalShouldStop.notify();
	}


	void QueueService::registerWorker(void* threadself)
	{
		assert(threadself != nullptr);
		MutexLocker locker(*this);
		if (pWorkerSet.count(threadself) == 0)
			pWorkerSet.insert(threadself);
	}


	void QueueService::unregisterWorker(void* threadself)
	{
		assert(threadself != nullptr);
		MutexLocker locker(*this);

		Yuni::Set<void*>::Unordered::iterator it = pWorkerSet.find(threadself);
		if (pWorkerSet.end() != it)
		{
			pWorkerSet.erase(it);
			if (pWorkerSet.empty())
			{
				if (pStatus == sStopping)
					pStatus = sStopped;
				pSignalAllThreadHaveStopped.notify();
			}
		}
	}


	bool QueueService::restart(uint timeout)
	{
		stop(timeout);
		return start();
	}


	void QueueService::gracefulStop()
	{
		MutexLocker locker(*this);
		if (pThreads and pStatus == sRunning)
		{
			// about to stop
			pStatus = sStopping;
			// ask to stop to all threads
			((ThreadArray*) pThreads)->gracefulStop();
			// notifying that the queueservice is stopped (or will stop soon)
			pSignalShouldStop.notify();
		}
	}


	inline bool QueueService::waitForAllThreads(uint timeout)
	{
		// waiting for all threads to stop
		do
		{
			if (0 == timeout)
			{
				pSignalAllThreadHaveStopped.wait();
			}
			else
			{
				if (not pSignalAllThreadHaveStopped.wait(timeout))
					return false;
			}

			MutexLocker locker(*this);
			// if the queue is running, we may have to reset our internal state
			if (pStatus == sRunning)
			{
				if (not pWorkerSet.empty() or not pWaitingRoom.empty())
				{
					pSignalAllThreadHaveStopped.reset();
					continue;
				}
			}
			break;
		}
		while (true);
		return true;
	}


	void QueueService::wait(QServiceEvent event)
	{
		assert((event == qseIdle or event == qseStop) and "invalid event");

		// checking if not started
		{
			MutexLocker locker(*this);
			if (pStatus == sStopped)
				return;
		}

		switch (event)
		{
			case qseStop:
			{
				// waiting for being terminated
				pSignalShouldStop.wait();

				// break : do not break - waiting for all threads being stopped
			}
			case qseIdle:
			{
				waitForAllThreads(0);
				break;
			}
			default:
			{
				assert(false and "invalid value for event");
			}
		}
	}


	bool QueueService::wait(QServiceEvent event, uint timeout)
	{
		assert((event == qseIdle or event == qseStop) and "invalid event");
		assert(timeout > 0 and "invalid timeout");

		// checking if not started
		{
			MutexLocker locker(*this);
			if (pStatus == sStopped)
				return true;
		}

		switch (event)
		{
			case qseStop:
			{
				// waiting for being terminated
				if (not pSignalShouldStop.wait(timeout))
					return false;

				waitForAllThreads(0);
				break;
			}
			case qseIdle:
			{
				if (not waitForAllThreads(timeout))
					return false;
				break;
			}
			default:
			{
				assert(false and "invalid value for event");
			}
		}
		return true;
	}


	inline void QueueService::wakeupWorkers()
	{
		MutexLocker locker(*this);
		if (pWorkerSet.size() < pMaximumThreadCount and pThreads)
			((ThreadArray*) pThreads)->wakeUp();
	}


	void QueueService::add(const IJob::Ptr& job)
	{
		assert(!(!job) and "invalid job");
		pWaitingRoom.add(job);
		wakeupWorkers();
	}


	void QueueService::add(const IJob::Ptr& job, Priority priority)
	{
		assert(!(!job) and "invalid job");
		pWaitingRoom.add(job, priority);
		wakeupWorkers();
	}


	void QueueService::clear()
	{
		pWaitingRoom.clear();
	}


	uint QueueService::threadCount() const
	{
		MutexLocker locker(*this);
		return pThreads ? ((const ThreadArray*) pThreads)->size() : 0;
	}




	namespace // anonymous
	{

		class QueueActivityPredicate final
		{
		public:
			//!
			typedef QueueService::ThreadInfo ThreadInfoType;
			typedef ThreadInfoType::Vector VectorType;

		public:
			QueueActivityPredicate(VectorType* out) :
				pList(out)
			{
				pList->clear();
			}

			template<class ThreadPtrT>
			bool operator () (const ThreadPtrT& thread) const
			{
				ThreadInfoType* info = new ThreadInfoType();
				info->thread = thread;
				if (!(!(info->thread)))
				{
					info->job = thread->currentJob();
					if (!(!(info->job)))
					{
						// We have a job which is currently working !
						info->hasJob = true;
						info->job->fillInformation(*info);
						pList->push_back(info);
						return true;
					}
				}
				info->hasJob = false;
				info->state = Yuni::Job::stateIdle;
				info->canceling = false;
				info->progression = 0;
				pList->push_back(info);
				return true;
			}

		private:
			mutable VectorType* pList;
		};

	} // anonymous namespace


	void QueueService::activitySnapshot(QueueService::ThreadInfo::Vector& out)
	{
		QueueActivityPredicate predicate(&out);
		MutexLocker locker(*this);
		if (pThreads)
			((ThreadArray*) pThreads)->foreachThread(predicate);
	}







} // namespace Job
} // namespace Yuni

