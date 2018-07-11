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
#include "../../yuni.h"
#include "../job.h"
#include "waitingroom.h"
#include "../../core/atomic/bool.h"
#include "../../thread/signal.h"
#include "q-event.h"
#include "../../core/dictionary.h"
#include "../../core/smartptr/intrusive.h"



namespace Yuni
{
namespace Job
{

	/*!
	** \brief Multithreaded Job QueueService
	*/
	class YUNI_DECL QueueService final
		: public IIntrusiveSmartPtr<QueueService, false>
		, public NonCopyable<QueueService>
	{
	public:
		//! Ancestor
		typedef IIntrusiveSmartPtr<QueueService, false>  Ancestor;
		//! The most suitable smart pointer for the class
		typedef Ancestor::SmartPtrType<QueueService>::PtrThreadSafe Ptr;
		//! The threading policy
		typedef Ancestor::ThreadingPolicy ThreadingPolicy;

		enum
		{
			//! A default timeout
			defaultTimeout = Yuni::Thread::defaultTimeout,
		};

		//! Information about a single thread
		class ThreadInfo final
		{
		public:
			//! The most suitable smart pointer for the class
			typedef SmartPtr<ThreadInfo> Ptr;
			//! Vector of ThreadInfo
			typedef std::vector<typename ThreadInfo::Ptr>  Vector;

		public:
			//! Reference to the working thread
			Thread::IThread::Ptr thread;
			//! Reference to the job currently in execution
			Job::IJob::Ptr job;

			//! Flag to know if the thread has a job currently in execution
			bool hasJob;

			//! State of the job (if any)
			Job::State state;
			//! Flag to know if the job is canceling its work
			bool canceling;
			//! Progression (in percent) of the job (if any, between 0 and 100)
			int progression;
			//! Name of the job
			String name;

		}; // class ThreadInfo


	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default Constructor
		*/
		QueueService();
		/*!
		** \brief Constructor, with an autostart mode
		**
		** \param autostart True to automatically start the service
		*/
		explicit QueueService(bool autostart);

		//! Destructor
		~QueueService();
		//@}


		//! \name Service
		//@{
		/*!
		** \brief Start the service and execute the jobs
		*/
		bool start();

		/*!
		** \brief Wait for an event
		**
		** Example: wait for all jobs to be finished
		** \code
		** queueservice.wait(qsIdle);
		** \endcode
		**
		** Example: wait for queue service being stopped
		** \code
		** queueservice.wait(qsStopped);
		** \endcode
		**
		** \param event A queue service event
		*/
		void wait(QServiceEvent event);

		/*!
		** \brief Wait for all jobs are finished
		**
		** \param event A queue service event
		** \param timeout Timeout (in milliseconds)
		** \return True if all jobs are finished, false if the timeout has been reached
		*/
		bool wait(QServiceEvent event, uint timeout);


		/*!
		** \brief Stop the service
		**
		** All unfinished jobs will be kept and re-executed at the next start.
		** It is of their responsibility to properly resume if they have to.
		** All working threads will be destroyed at the very end of this method.
		**
		** \important Since this method waits for the end of all running jobs, it
		**   must not be called from a job or it will result in deadlock
		**   (use 'gracefulStop()' instead)
		**
		** \param timeout Timeout (in milliseconds)
		*/
		void stop(uint timeout = defaultTimeout);


		/*!
		** \brief Ask to Stop the execution of the thread as soon as possible
		**
		** After a call to this method, the method suspend() will return true,
		** which indicates that the job should stop as soon as possible.
		**
		** \note This method can be safely called from a job
		*/
		void gracefulStop();

		/*!
		** \brief Stop then start the service
		**
		** All unfinished jobs will be kept and re-executed at the next start.
		** It is of their responsibility to properly resume if they have to.
		*/
		bool restart(uint timeout = defaultTimeout);

		//! Get if the service is started
		bool started() const;

		//! Get if the scheduler is idle (no worker is in active duty)
		bool idle() const;
		//@}


		//! \name Jobs handling
		//@{
		/*!
		** \brief Add a job into the queue
		**
		** \warning The job may already be into the queue (thus the same job might be
		** executed several times at once). However it must obviously ensure its own
		** thread-safety in this case.
		**
		** \param job The job to add
		*/
		void add(const IJob::Ptr& job);

		/*!
		** \brief Add a job into the queue
		**
		** \warning The job may already be into the queue (thus the same job might be
		** executed several times at once). However it must obviously ensure its own
		** thread-safety in this case.
		**
		** \param job The job to add
		** \param priority Its priority execution
		*/
		void add(const IJob::Ptr& job, Priority priority);

		/*!
		** \brief Retrieve information about the activity of the queue manager
		**
		** \note Event if in the list, a job may already have finished
		**   its work at the end of this method.
		*/
		void activitySnapshot(ThreadInfo::Vector& out);

		/*!
		** \brief Get the number of jobs waiting to be executed
		**
		** This value does not take into account the number of jobs
		** currently running.
		*/
		uint waitingJobsCount() const;

		/*!
		** \brief Remove all jobs waiting for being executed
		**
		** All jobs currently running won't be stopped
		*/
		void clear();
		//@}


		//! \name Threads
		//@{
		//! Get the minimum number of threads
		uint minimumThreadCount() const;
		//! Set the minimum number of threads (must be less than maximumNu)
		bool minimumThreadCount(uint count);

		//! Get the maximum number of threads
		uint maximumThreadCount() const;
		//! Set the maximum number of threads (must be >= 1)
		bool maximumThreadCount(uint count);

		//! Get the minimum and the maximum number of threads
		std::pair<uint,uint> minmaxThreadCount() const;
		//! Set the minimum and the maximum number of threads
		bool minmaxThreadCount(const std::pair<uint, uint>& minmaxcount);

		//! Get the current number of working threads
		uint threadCount() const;
		//@}


		//! \name Operators
		//@{
		//! The operator << (add a job)
		QueueService& operator += (IJob* job);
		//! The operator << (add a job)
		QueueService& operator << (IJob* job);
		//! The operator << (add a job)
		QueueService& operator += (const IJob::Ptr& job);
		//! The operator << (add a job)
		QueueService& operator << (const IJob::Ptr& job);
		//@}


	private:
		//! Register a new thread in active duty
		void registerWorker(void* threadself);
		//! Unregister a thread no longer in active duty
		void unregisterWorker(void* threadself);
		//! Wait for all threads to finish
		bool waitForAllThreads(uint timeout);
		//! Wake up thread - some work here !
		void wakeupWorkers();

	private:
		//! Flag to know if the service is started [must be protected by the internal mutex]
		enum State
		{
			sStopped,
			sRunning,
			sStopping,
		}
		pStatus;

		//! The list of all remaining jobs
		Yuni::Private::QueueService::WaitingRoom pWaitingRoom;

		// Scheduler

		//! The minimum number of threads [must be protected by the internal mutex]
		volatile uint pMinimumThreadCount;
		//! The maximum number of threads [must be protected by the internal mutex]
		volatile uint pMaximumThreadCount;
		//! Array of threads
		volatile void* pThreads;

		//! Signal, for being notified when all threads have stopped to work
		Yuni::Thread::Signal pSignalAllThreadHaveStopped;
		//!
		Yuni::Thread::Signal pSignalShouldStop;

		//! Set of workers in active duty
		// The workers may need to unregister several times and it is not safe
		// to let the worker handle the state (it might be killed or something nasty
		// may happen - race conditions)
		// This set is useless (and should not be used in this current form)
		Yuni::Set<void*>::Unordered  pWorkerSet;

		// Nakama !
		friend class Yuni::Private::QueueService::QueueThread;

	}; // class QueueService






} // namespace Job
} // namespace Yuni

#include "service.hxx"
