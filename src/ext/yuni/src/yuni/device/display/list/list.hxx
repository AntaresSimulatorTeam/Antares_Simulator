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
#include "list.h"



namespace Yuni
{
namespace Device
{
namespace Display
{


	inline size_t List::size() const
	{
		return pMonitors.size();
	}


	inline Monitor::Ptr List::primary() const
	{
		return pPrimary;
	}


	inline List::iterator List::begin()
	{
		return pMonitors.begin();
	}


	inline List::iterator List::end()
	{
		return pMonitors.end();
	}

	inline List::const_iterator List::begin() const
	{
		return pMonitors.begin();
	}


	inline List::const_iterator List::end() const
	{
		return pMonitors.end();
	}




} // namespace Display
} // namespace Device
} // namespace Yuni
