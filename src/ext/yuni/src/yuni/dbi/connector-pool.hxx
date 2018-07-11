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
#ifndef __YUNI_DBI_CONNECTOR_HXX__
# define __YUNI_DBI_CONNECTOR_HXX__



namespace Yuni
{
namespace DBI
{

	inline Transaction  ConnectorPool::begin()
	{
		return Transaction(pData);
	}


	inline Error ConnectorPool::open(Adapter::IAdapter* adapter, AnyString host)
	{
		Settings settings;
		settings.host    = host;
		return open(adapter, settings);
	}


	inline Error ConnectorPool::open(Adapter::IAdapter* adapter, AnyString host, AnyString dbname, AnyString user, AnyString password)
	{
		Settings settings;
		settings.host     = host;
		settings.database = dbname;
		settings.username = user;
		settings.password = password;
		return open(adapter, settings);
	}


	inline AutoCommit ConnectorPool::autocommit()
	{
		return AutoCommit(*this);
	}





} // namespace DBI
} // namespace Yuni

#endif // __YUNI_DBI_CONNECTOR_HXX__
