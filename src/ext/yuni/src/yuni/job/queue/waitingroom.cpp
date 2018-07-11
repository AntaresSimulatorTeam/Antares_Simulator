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
#include "../../yuni.h"
#include "waitingroom.h"


namespace Yuni
{
namespace Private
{
namespace QueueService
{


	WaitingRoom::~WaitingRoom()
	{
		// locking all mutex to prevent some race conditions
		// (with clear() for example)
		for (uint i = 0; i != (uint) priorityCount; ++i)
			pMutexes[i].lock();
		for (uint i = 0; i != (uint) priorityCount; ++i)
			pMutexes[i].unlock();
	}


	void WaitingRoom::clear()
	{
		// we should lock all lists before anything
		for (uint i = 0; i != (uint) priorityCount; ++i)
			pMutexes[i].lock();

		// reset the total number of job _before_ unlocking
		pJobCount = 0; // may notify listeners that there is nothing to do

		// clear
		for (uint i = 0; i != (uint) priorityCount; ++i)
			pJobs[i].clear();

		// unlock all
		for (uint i = 0; i != (uint) priorityCount; ++i)
				pMutexes[i].unlock();
	}


	void WaitingRoom::add(const Yuni::Job::IJob::Ptr& job, Yuni::Job::Priority priority)
	{
		// Locking the priority queue
		// We should avoid ThreadingPolicy::MutexLocker since it may not be
		// the good threading policy for these mutexes
		Yuni::MutexLocker locker(pMutexes[priority]);

		// Resetting some internal variables of the job
		Yuni::Private::QueueService::JobAccessor<Yuni::Job::IJob>::AddedInTheWaitingRoom(*job);
		// Adding it into the good priority queue
		pJobs[priority].push_back(job);

		// Resetting our internal state
		++pJobCount;
	}


	bool WaitingRoom::pop(Yuni::Job::IJob::Ptr& out, const Yuni::Job::Priority priority)
	{
		// We should avoid ThreadingPolicy::MutexLocker since it may not be
		// the good threading policy for these mutexes
		Yuni::MutexLocker locker(pMutexes[priority]);

		if (not pJobs[priority].empty())
		{
			// It remains at least one job to run !
			out = pJobs[priority].front();
			// Removing it from the list of waiting jobs
			pJobs[priority].pop_front();

			--pJobCount;
			return true;
		}
		// It does not remain any job for this priority. Aborting.
		return false;
	}


	bool WaitingRoom::pop(Yuni::Job::IJob::Ptr& out)
	{
		return (pop(out, Yuni::Job::priorityHigh))
			or (pop(out, Yuni::Job::priorityDefault))
			or (pop(out, Yuni::Job::priorityLow));
	}





} // namespace QueueService
} // namespace Private
} // namespace Yuni

