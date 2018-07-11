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
#include "service.h"
#include "../../thread/timer.h"



namespace Yuni
{
namespace Job
{

	inline uint QueueService::waitingJobsCount() const
	{
		return pWaitingRoom.size();
	}


	inline QueueService& QueueService::operator += (IJob* job)
	{
		add(job);
		return *this;
	}


	inline QueueService& QueueService::operator << (IJob* job)
	{
		add(job);
		return *this;
	}


	inline QueueService& QueueService::operator += (const IJob::Ptr& job)
	{
		add(job);
		return *this;
	}


	inline QueueService& QueueService::operator << (const IJob::Ptr& job)
	{
		add(job);
		return *this;
	}


	inline bool QueueService::idle() const
	{
		MutexLocker locker(*this);
		return pWorkerSet.empty();
	}


	inline bool QueueService::started() const
	{
		MutexLocker locker(*this);
		return (pStatus != sStopped);
	}





} // namespace Job
} // namespace Yuni
