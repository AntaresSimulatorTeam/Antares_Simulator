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
#include "signal.h"



namespace Yuni
{
namespace Thread
{

	inline bool Signal::valid() const
	{
		#ifndef YUNI_NO_THREAD_SAFE
		# ifdef YUNI_OS_WINDOWS
		return (pHandle != NULL);
		# else
		return true;
		# endif
		# else
		return true;
		# endif
	}


	inline bool Signal::operator ! () const
	{
		return not valid();
	}


	inline Signal& Signal::operator = (const Signal&)
	{
		return *this;
	}





} // namespace Thread
} // namespace Yuni

