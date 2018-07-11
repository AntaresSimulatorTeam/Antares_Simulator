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
#include "fwd.h"
#include "../thread/thread.h"
#include "../core/string.h"
#include "../core/atomic/bool.h"
#include "../core/smartptr/intrusive.h"




namespace Yuni
{

/*!
** \brief Jobs (Multithreading programming)
** \ingroup Jobs
*/
namespace Job
{

	/*!
	** \brief Job Interface (time consuming operations)
	**
	** This class is designed to implement a single task executed by
	** Job::QueueService.
	**
	** This clas can be used for wrapping a downloader, an image
	** processing algorithm, a movie encoder, etc.
	**
	** Indeed, doing time-consuming operations like downloads and database
	** transactions in the main thread can cause your user interface to seem as
	** though it has stopped responding while they are running. To avoid this
	** problem, you want to execute your long-running task in an asynchonous
	** manner (a background task).
	**
	** See async() for a more convenient way for simple tasks
	**
	** \code
	**
	** class MyJob final : public Job::IJob
	** {
	** public:
	**   virtual ~MyJob() { }
	** private:
	**   virtual void onExecute() override
	**   {
	**		[... time consuming job ...]
	**   }
	** };
	**
	** [... add a MyJob instance to a Job::QueueService ...]
	**
	** \endcode
	**
	** \see QueueService
	** \ingroup Jobs
	*/
	class YUNI_DECL IJob : public IIntrusiveSmartPtr<IJob, false>
	{
	public:
		//! Ancestor
		typedef IIntrusiveSmartPtr<IJob, false>  Ancestor;
		//! The most suitable smart pointer for the class
		typedef Ancestor::SmartPtrType<IJob>::PtrThreadSafe Ptr;
		//! The threading policy
		typedef Ancestor::ThreadingPolicy ThreadingPolicy;
		//! List of jobs
		typedef std::list<Ptr> List;

	public:
		//! \name Contructor & Destructor
		//@{
		//! Default constructor
		IJob();
		//! Destructor
		virtual ~IJob();
		//@}


		//! \name Informations about the job itself
		//@{
		//! Get the progression in percent (value between 0 and 100)
		int progression() const;

		/*!
		** \brief Get if the job is finished
		**
		** This is a convenient (and faster) replacement for the following code :
		** \code
		** (state() == stateIdle and progression() == 100)
		** \endcode
		*/
		bool finished() const;

		//! Get the caption of the job
		virtual String caption() const;
		//@}


		//! \name States
		//@{
		//! Get the current state of the job
		enum Job::State state() const;

		//! Get if the job is idle
		bool idle() const;
		//! Get if the job is waiting for being executed
		bool waiting() const;
		//! Get if the job is running
		bool running() const;
		//! Get if the job is canceling its work (and it is currently running)
		bool canceling() const;
		//@}


		//! \name Execution flow
		//@{
		/*!
		** \brief Ask to the job to cancel its work as soon as possible
		**
		** This method has no effect if the job is not currently running.
		*/
		void cancel();

		/*!
		** \brief Execute the job
		**
		** In standard uses, 't' must not be NULL.
		** \param t The thread which will execute this job (can be null)
		*/
		void execute(Thread::IThread* t);
		//@}


		template<class T> void fillInformation(T& info);

	protected:
		/*!
		** \brief Implement this method to define your own time-consuming task
		*/
		virtual void onExecute() = 0;

		/*!
		** \brief Set the progression in percent (0..100)
		*/
		void progression(const int p);

		/*!
		** \brief Suspend the execution of the job during X miliseconds
		**
		** This method is nearly identical to IThread::suspend() in its behavior.
		** But this method should not have to be called in most of the jobs,
		** except when polling events or equivalent.
		** If you only want to know if the job should abort, prefer shouldAbort()
		** instead.
		**
		** \attention This method must only be called inside the execution of the job
		**
		** \param delay The delay in miliseconds. 0 will only return if the thread should exit
		** \return True indicates that the job should stop immediately
		** \see IThread::suspend()
		*/
		bool suspend(uint delay = 0) const;

		/*!
		** \brief Get if the job should abort as soon as possible
		**
		** This is a convenient replacement of `suspend(0)`, and a bit faster.
		**
		** \attention This method must only be called inside the execution of the job
		** \return True indicates that the thread should stop immediately
		*/
		bool shouldAbort() const;


	private:
		//! State of the job
		Atomic::Int<32> pState;
		//! Progression
		Atomic::Int<32> pProgression;
		//! Flag to cancel the work
		Atomic::Bool pCanceling;
		//! The attached thread to this job, if any
		ThreadingPolicy::Volatile<Thread::IThread*>::Type pThread;

		// our friends !
		template<class JobT> friend class Yuni::Private::QueueService::JobAccessor;

	}; // class IJob






} // namespace Job
} // namespace Yuni

# include "job.hxx"

