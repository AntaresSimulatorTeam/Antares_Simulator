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
#include "thread.h"



namespace Yuni
{
namespace Private
{
namespace QueueService
{

	inline QueueThread::QueueThread(Yuni::Job::QueueService& queueservice)
		: pQueueService(queueservice)
	{
	}


	inline QueueThread::~QueueThread()
	{
		// Ensure that the thread is really stopped
		stop();
	}


	inline Yuni::Job::IJob::Ptr QueueThread::currentJob() const
	{
		return pJob;
	}


	inline void WaitingRoom::add(const Yuni::Job::IJob::Ptr& job)
	{
		add(job, Yuni::Job::priorityDefault);
	}





} // namespace QueueService
} // namespace Private
} // namespace Yuni
