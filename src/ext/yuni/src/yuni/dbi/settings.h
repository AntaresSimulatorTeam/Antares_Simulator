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
#ifndef __YUNI_DBI_SETTINGS_H__
# define __YUNI_DBI_SETTINGS_H__

# include "../yuni.h"
# include "../core/string.h"


namespace Yuni
{
namespace DBI
{

	class Default
	{
	public:
		enum
		{
			maxReconnectionAttempts = 30,
			delayBetweenReconnection = 1000, // ms
			idleTime = 60 // seconds
		};
	};


	/*!
	** \ingroup DBI
	*/
	class Settings
	{
	public:
		//! Default constructor
		Settings();

		//! Clear all settings
		void clear();


	public:
		//! host (empty means localhost) or filename
		String host;
		//! port (0 means default)
		uint port;
		//! username
		ShortString128 username;
		//! password
		String password;
		//! Database name
		ShortString128 database;
		//! schema to open (only if the database supports it)
		ShortString128 schema;

		//! Maximum number of Attempts of reconnection before declaring the connexion dead (-1 infinite)
		uint maxReconnectionAttempts;
		//! Delay (in milliseconds) to wait before each attempt
		uint delayBetweenReconnectionAttempt;

		//! Minimum time (in seconds) to wait before closing an idle connection
		uint idleTime;

	}; // class Settings





} // namespace DBI
} // namespace Yuni

# include "settings.hxx"

#endif // __YUNI_DBI_SETTINGS_H__
