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
#include "../../thread/id.h"


namespace Yuni
{
namespace Private
{
namespace DBI
{

	inline ConnectorData::ConnectorData(const Yuni::DBI::Settings& settings, Yuni::DBI::Adapter::IAdapter* instance) :
		settings(settings),
		instance(instance)
	{
		assert(instance != NULL);

		// reset adapter
		(void)::memset(&adapter, '\0', sizeof(adapter));
		instance->retrieveEntries(adapter);
		assert(adapter.dbh == NULL and "the variable dbh must not be set by adapter");
	}


	inline ChannelPtr ConnectorData::openChannel()
	{
		// current thread id
		auto threadid = Thread::ID();

		// locker
		Yuni::MutexLocker locker(mutex);

		// checking if a channel does not already exists
		Channel::Table::iterator i = channels.find(threadid);
		return (i != channels.end())
			? i->second
			: createNewChannelWL(threadid);
	}





} // namespace DBI
} // namespace Private
} // namespace Yuni
