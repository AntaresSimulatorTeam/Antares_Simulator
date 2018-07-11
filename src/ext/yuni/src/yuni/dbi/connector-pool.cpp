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
#include "connector-pool.h"
#include "../private/dbi/connector-data.h"



namespace Yuni
{
namespace DBI
{

	ConnectorPool::ConnectorPool(const ConnectorPool& rhs) :
		pData(rhs.pData)
	{}


	ConnectorPool& ConnectorPool::operator = (const ConnectorPool& rhs)
	{
		pData = rhs.pData;
		return *this;
	}


	ConnectorPool::ConnectorPool()
	{
		// does nothing
	}


	ConnectorPool::~ConnectorPool()
	{
		close();
	}


	Error ConnectorPool::open(Adapter::IAdapter* adapter, const Settings& settings)
	{
		// close all existing connectionss if possible
		close();

		// the adapter must be set
		if (not adapter)
			return errInvalidAdapter;

		// creating the new connector data
		auto* data = new Yuni::Private::DBI::ConnectorData(settings, adapter);

		// attempt to open a new channel to the remote database
		// from the current thread in order to check the connection settings
		auto channel = data->openChannel();

		// should never happen, but we never know
		assert(!(!channel) and "invalid channel");

		// checking the channel status
		MutexLocker locker(channel->mutex);
		if (errNone == channel->status)
		{
			// the channel has been successfully opened

			// retrieving all events
			// we may have a minor inconsistency here in some heavily multithreaded
			// applications with the value of onSQLError, but it should not matter
			// (event is thread-safe of course)
			data->onSQLError = onSQLError;

			// installing the new connector data
			pData = data;
			return errNone;
		}
		else
		{
			// the channel could not be opened, aborting
			delete data;
			return channel->status;
		}
	}


	void ConnectorPool::close()
	{
		// acquiring / swapping the data, to destroy it in the same time
		Yuni::Private::DBI::ConnectorDataPtr data;
		data.swap(pData);

		if (!(!data))
		{
			// try to close as many connections as possible.
			// with a null idle time, the only remaining connections will be
			// still in use, probably by another thread
			uint dummy;
			data->closeTooOldChannels(/*idletime:*/ 0, dummy);
		}
	}


	bool ConnectorPool::retrieveSettings(Settings& out) const
	{
		// acquiring the pointer to avoid race conditions
		Yuni::Private::DBI::ConnectorDataPtr data = pData;
		if (!(!data))
		{
			out = data->settings;
			return true;
		}
		else
		{
			out.clear();
			return false;
		}
	}


	void ConnectorPool::closeIdleConnections(uint* remainingCount, uint* closedCount)
	{
		// acquiring the pointer to avoid race conditions
		Yuni::Private::DBI::ConnectorDataPtr data = pData;

		if (!(!data))
		{
			// idle time, from the settings
			uint idletime = data->settings.idleTime;

			// stats
			uint statsRemain = 0;
			uint statsClosed;

			// closing idle connections
			statsClosed = data->closeTooOldChannels(idletime, statsRemain);

			// reporting
			if (remainingCount)
				*remainingCount = statsRemain;
			if (closedCount)
				*closedCount = statsClosed;
		}
		else
		{
			// no connector data, no connections
			if (remainingCount)
				*remainingCount = 0;
			if (closedCount)
				*closedCount = 0;
		}
	}


	void ConnectorPool::closeIdleConnections(uint idleTime, uint* remainingCount, uint* closedCount)
	{
		// acquiring the pointer to avoid race conditions
		Yuni::Private::DBI::ConnectorDataPtr data = pData;

		if (!(!data))
		{
			// stats
			uint statsRemain = 0;
			uint statsClosed;

			// closing idle connections
			statsClosed = data->closeTooOldChannels(idleTime, statsRemain);

			// reporting
			if (remainingCount)
				*remainingCount = statsRemain;
			if (closedCount)
				*closedCount = statsClosed;
		}
		else
		{
			// no connector data, no connections
			if (remainingCount)
				*remainingCount = 0;
			if (closedCount)
				*closedCount = 0;
		}
	}





} // namespace DBI
} // namespace Yuni

