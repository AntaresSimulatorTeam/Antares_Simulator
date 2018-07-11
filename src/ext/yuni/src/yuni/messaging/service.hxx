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



namespace Yuni
{
namespace Messaging
{

	inline Service::Transports::Transports()
		: pService()
	{}


	inline Service::HeavyTasks::HeavyTasks()
		: pService()
	{}


	inline Net::Error Service::Transports::add(const Net::Port& port, Transport::ITransport::Ptr transport)
	{
		return add("*", port, transport);
	}


	inline bool Service::HeavyTasks::enabled() const
	{
		return (0 != pEnabled);
	}


	inline void Service::HeavyTasks::enabled(bool on)
	{
		pEnabled = (on ? 1 : 0);
	}


	inline Service::HeavyTasks& Service::HeavyTasks::operator += (Job::IJob* job)
	{
		queue += job;
		return *this;
	}


	inline Service::HeavyTasks& Service::HeavyTasks::operator += (Job::IJob::Ptr& job)
	{
		queue += job;
		return *this;
	}





} // namespace Messaging
} // namespace Yuni
