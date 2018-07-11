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
#include "../../dbi/settings.h"
#include "../../dbi/fwd.h"
#include "../../dbi/adapter.h"
#include "channel.h"
#include "../../core/event/event.h"



namespace Yuni
{
namespace Private
{
namespace DBI
{


	class ConnectorData final
	{
	public:
		//! Default constructor
		ConnectorData(const Yuni::DBI::Settings& settings, Yuni::DBI::Adapter::IAdapter* adapter);
		//! Destructor
		~ConnectorData();

		/*!
		** \brief Open a communication channel to the remote database (per thread)
		*/
		ChannelPtr openChannel();

		/*!
		** \brief Close all old channels
		**
		** \param idletime Idle time (seconds)
		** \param[out] remainingCount The number of channels currently opened (after cleanup)
		** \return The number of channels which have been closed
		*/
		uint closeTooOldChannels(uint idletime, uint& remainingCount);


	public:
		//! Settings used to connect to the database
		Yuni::DBI::Settings settings;
		//! Adapter Entries
		::yn_dbi_adapter adapter;
		//! Mutex
		Mutex mutex;

		//! All channels, ordered by a thread id
		Channel::Table channels;

		// delete the instance
		Yuni::DBI::Adapter::IAdapter* instance;

		//! Event trigered when a SQL error occurs
		Event<void ()> onSQLError;


	private:
		//! Instantiate a new channel
		ChannelPtr createNewChannelWL(uint64 threadid);

	}; // class ConnectorData





} // namespace DBI
} // namespace Private
} // namespace Yuni

#include "connector-data.hxx"
