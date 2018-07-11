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
#include "channel.h"


namespace Yuni
{
namespace Private
{
namespace DBI
{


	Channel::Channel(const Yuni::DBI::Settings& settings, const ::yn_dbi_adapter& adapter) :
		mutex(/*recursive:*/ true),
		adapter(adapter),
		nestedTransactionCount(0),
		settings(settings),
		lastUsed(Yuni::DateTime::Now())
	{
		open();
	}


	Channel::~Channel()
	{
		MutexLocker locker(mutex);

		if (nestedTransactionCount > 0)
		{
			std::cerr << "closing database channel but " << nestedTransactionCount
				<< " transaction(s) remain" << std::endl;
			assert(false and "closing database channel but some transactions remain");
		}

		if (adapter.dbh)
			adapter.close(adapter.dbh);
	}





} // namespace DBI
} // namespace Private
} // namespace Yuni

