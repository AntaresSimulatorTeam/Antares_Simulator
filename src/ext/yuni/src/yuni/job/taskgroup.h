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
#pragma once
#include "../yuni.h"
#include "../core/string.h"
#include "../core/event/event.h"
#include "../core/noncopyable.h"
#include "../core/smartptr/intrusive.h"
#include "../thread/signal.h"




namespace Yuni
{
namespace Job
{

	//! Forward declaration
	class IJob;
	class QueueService;



	/*!
	** \brief Group of jobs
	**
	** \code
	** Job::QueueService queueservice;
	** Job::Taskgroup taskgroup(queueservice);
	**
	** taskgroup += [&](IJob&) -> bool {
	**		std::cout << "job 1 complete !" << std::endl;
	**		return true;
	** };
	** taskgroup += [&](IJob&) -> bool {
	**		std::cout << "job 2 complete !" << std::endl;
	**		return true;
	** };
	** taskgroup += [&](IJob&) -> bool {
	**		std::cout << "job 3 complete !" << std::endl;
	**		return true;
	** };
	**
	** taskgroup.start();
	** taskgroup.wait();
	** \endcode
	** \internal This class must keep the fewer dependencies as possible
	*/
	class YUNI_DECL Taskgroup final
		: public IIntrusiveSmartPtr<Taskgroup, false>
		, public NonCopyable<Taskgroup>
	{
	public:
		//! Ancestor
		typedef IIntrusiveSmartPtr<Taskgroup, false>  Ancestor;
		//! The most suitable smart pointer for the class
		typedef Ancestor::SmartPtrType<Taskgroup>::PtrThreadSafe Ptr;
		//! The threading policy
		typedef Ancestor::ThreadingPolicy ThreadingPolicy;
		//! Job class for the task
		class YUNI_DECL ITaskgroupJob;

		enum Status
		{
			//! The task is currently running
			stRunning,
			//! The task has succeeded
			stSucceeded,
			//! The task has failed
			stFailed,
			//! The task has been canceled
			stCanceled,
		};


	public:
		//! \name Constructors & Destructor
		//@{
		/*!
		** \brief Default constructor
		** \param queueservice The default queueservice to use when not specified
		*/
		explicit Taskgroup(QueueService& queueservice, bool cancelOnError = true, bool autostart = false);

		# ifdef YUNI_HAS_CPP_MOVE
		//! Move constructor
		Taskgroup(Taskgroup&&) = delete;
		# endif

		/*!
		** \brief Destructor (will stop all underlying jobs)
		**
		** The task will be canceled if running. Use `wait()` to avoid this behavior
		*/
		~Taskgroup();
		//@}


		//! \name Task
		//@{
		/*!
		** \brief Run the task
		**
		** This method has no effect if the taskgroup was already running and will
		** stop immediatly if no job/callback has been added first.
		*/
		void start();

		/*!
		** \brief Cancel the task
		**
		** All underlying jobs will be stopped as soon as possible
		*/
		void cancel();

		/*!
		** \brief Wait for the task being complete
		** \return The status of the task
		*/
		Status wait();

		/*!
		** \brief Wait for the task being complete (with timeout)
		**
		** \param timeout A timeout, in milliseconds
		** \return The status of the task. stRunning if the timeout has been reached
		*/
		Status wait(uint timeout);

		/*!
		** \brief Get the current status of the task and Fetch various information in the same time
		**
		** \param[out] jobCount The total number of jobs [optional]
		** \param[out] doneCount The total number of jobs which have terminated [optional]
		** \return status The current status of the task
		*/
		Status status(uint* jobCount = nullptr, uint* doneCount = nullptr) const;


		/*!
		** \brief Make the task succceed
		**
		** All underlying jobs will be stopped as soon as possible
		*/
		void markAsSucceeded();

		/*!
		** \brief Make the task fail
		**
		** All underlying jobs will be stopped as soon as possible
		*/
		void markAsFailed();
		//@}


		//! \name Jobs
		//@{
		/*!
		** \brief Add a new job in the task
		**
		** The job will be added to the default queueservice immediatly if the taskgroup is running
		** \param callback A functor / lambda
		*/
		void add(const Bind<bool (IJob&)>& callback);

		/*!
		** \brief Add a new job in the task
		**
		** The job will be added to the queueservice immediatly if the taskgroup is running
		** \param queueservice Any queueservice can be used.
		** \param callback A functor / lambda
		*/
		void add(QueueService& queueservice, const Bind<bool (IJob&)>& callback);
		//@}


		//! \name Operators
		//@{
		//! Add a new job
		Taskgroup& operator += (const Bind<bool (IJob&)>& callback);

		# ifdef YUNI_HAS_CPP_MOVE
		//! Move constructor
		Taskgroup& operator = (Taskgroup&&) = delete;
		# endif
		//@}



	public:
		//! All events
		struct
		{
			//! Event: the task has started (can be called from any thread)
			Event<void ()> started;
			//! Event: the task has terminated its work (can be called from any thread)
			Event<void (Status success)> terminated;
		}
		on;



	private:
		//! Array of sub-jobs
		typedef std::vector<ITaskgroupJob*> JobList;

	private:
		inline void startWL();
		inline void stopAllJobsWL();
		inline void deleteAllJobsWL();
		inline void onJobTerminated(ITaskgroupJob&, bool success);
		inline void onTaskStoppedWL();

	private:
		//! The task has been told to start (thus 'is running' and jobs are waiting to run)
		bool pTaskHasStarted;
		//! Current state of the task
		// (should not be directly used if the task is still running)
		Status pTaskStatus;
		//! Signal for notifyng that the task has finished (canceled or all jobs are terminated)
		Thread::Signal pSignalTaskStopped;

		//! All jobs registered for the task (can be added while running)
		JobList pJobs;
		//! The total number of jobs which have finish their work
		uint pJobsDoneCount;
		//! True to automatically cancel when the first error is encountered
		const bool pCancelOnError;
		//! True to automatically run the task (if not already running) when adding a job
		const bool pAutostart;

		//! The default queueservice
		QueueService& pDefaultQueueservice;

	}; // class Taskgroup







} // namespace Job
} // namespace Yuni

#include "taskgroup.hxx"
