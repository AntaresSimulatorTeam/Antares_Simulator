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
#ifndef __YUNI_DBI_SETTINGS_HXX__
# define __YUNI_DBI_SETTINGS_HXX__


namespace Yuni
{
namespace DBI
{

	inline Settings::Settings() :
		port(),
		maxReconnectionAttempts(Default::maxReconnectionAttempts),
		delayBetweenReconnectionAttempt(Default::delayBetweenReconnection),
		idleTime(Default::idleTime)
	{}


	inline void Settings::clear()
	{
		host.clear();
		username.clear();
		password.clear();
		database.clear();
		schema.clear();
		port = 0;

		maxReconnectionAttempts = Default::maxReconnectionAttempts;
		delayBetweenReconnectionAttempt = Default::delayBetweenReconnection;
		idleTime = Default::idleTime;
	}





} // namespace DBI
} // namespace Yuni

#endif // __YUNI_DBI_SETTINGS_H__
