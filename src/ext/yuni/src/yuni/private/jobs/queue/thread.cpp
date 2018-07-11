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
#include "thread.h"



namespace Yuni
{
namespace Private
{
namespace QueueService
{


	inline void QueueThread::notifyEndOfWork()
	{
		// Notify the scheduler that this thread goes to sleep
		pQueueService.unregisterWorker(this);
	}


	bool QueueThread::onExecute()
	{
		// Notify the scheduler that this thread has begun its work
		pQueueService.registerWorker(this);

		// Asking for the next job
		while (pQueueService.pWaitingRoom.pop(pJob))
		{
			// Execute the job, via a wrapper for symbol visibility issues
			Yuni::Private::QueueService::JobAccessor<Yuni::Job::IJob>::Execute(*pJob, this);

			// We must release our pointer to the job here to avoid its destruction
			// in `pQueueService.nextJob()` (when `pJob` is re-assigned).
			// This method uses a lock and the destruction of the job may take some time.
			// Obviously, there is absolutely no guarantee that the job will be destroyed
			// at this point but we don't really care
			pJob = nullptr;

			// Cancellation point
			if (YUNI_UNLIKELY(shouldAbort())) // We have to stop as soon as possible, no need for hibernation
				return false;

		} // loop for retrieving jobs to execute

		// Returning true, for hibernation
		return true;
	}


	void QueueThread::onStop()
	{
		notifyEndOfWork(); // we are done here !
	}


	void QueueThread::onPause()
	{
		notifyEndOfWork(); // we are done here !
	}


	void QueueThread::onKill()
	{
		if (!(!(pJob)))
		{
			// Notify the job that it has been killed
			// (via the wrapper for symbol visibility issues)
			Yuni::Private::QueueService::JobAccessor<Yuni::Job::IJob>::ThreadHasBeenKilled(*pJob);
			// Release the pointer, if possible of course
			pJob = nullptr;
		}

		notifyEndOfWork(); // we are done here !
	}






} // namespace QueueService
} // namespace Private
} // namespace Yuni

