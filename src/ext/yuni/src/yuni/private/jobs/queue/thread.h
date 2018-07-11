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
#include "../../../thread/thread.h"
#include "../../../job/queue/service.h"
#include "../../../thread/signal.h"



namespace Yuni
{
namespace Private
{
namespace QueueService
{


	/*!
	** \brief A single thread for a queue service
	*/
	class YUNI_DECL QueueThread final : public Yuni::Thread::IThread
	{
	public:
		//! The most suitable smart pointer for the class
		typedef Yuni::Thread::IThread::Ptr::Promote<QueueThread>::Ptr Ptr;

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default Constructor
		*/
		explicit QueueThread(Yuni::Job::QueueService& queueservice);
		//! Destructor
		virtual ~QueueThread();
		//@}

		/*!
		** \brief Get the Job currently running
		*/
		Yuni::Job::IJob::Ptr currentJob() const;


	protected:
		//! Implementation of the `onExecute` method to run the jobs from the waiting room
		virtual bool onExecute() override;
		//! Implementation of the `onKill` method when the thread is killed without mercy
		virtual void onKill() override;
		//! Implementation of the `onStop` method when the thread is killed without mercy
		virtual void onStop() override;
		//! Implementation of the `onPause` method when the thread is killed without mercy
		virtual void onPause() override;

	private:
		//! Notify the queueservice that we have stopped to work
		void notifyEndOfWork();

	private:
		//! The scheduler
		Yuni::Job::QueueService& pQueueService;
		//! The current job
		Yuni::Job::IJob::Ptr pJob;

	}; // class QueueThread






} // namespace QueueService
} // namespace Private
} // namespace Yuni

#include "thread.hxx"
