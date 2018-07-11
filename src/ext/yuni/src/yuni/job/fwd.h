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
#include "enum.h"




namespace Yuni
{
namespace Job
{

	// Forward declaration
	class IJob;

} // namespace Job
} // namespace Yuni



namespace Yuni
{
namespace Private
{
namespace QueueService
{

	// Forward declaration
	class WaitingRoom;
	class QueueThread;



	template<class JobT>
	class JobAccessor final
	{
	public:
		template<class ThreadT>
		static void Execute(JobT& job, ThreadT* thread)
		{
			job.execute(thread);
		}

		static void ThreadHasBeenKilled(JobT& job)
		{
			job.pThread = nullptr;
			job.pState  = Yuni::Job::stateWaiting;
		}

		static void AddedInTheWaitingRoom(JobT& job)
		{
			job.pState = Yuni::Job::stateWaiting;
			job.pProgression = 0;
		}

	}; // class JobAccessor





} // namespace QueueService
} // namespace Private
} // namespace Yuni
