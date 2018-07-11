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
#include "../../yuni.h"

/* This header must remain compatible with C compilers */


#ifdef YUNI_OS_WINDOWS
#	ifdef YUNI_OS_MSVC
#		pragma warning(push)
#		pragma warning(disable : 4995)
#		pragma warning(push, 0)
#	endif
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN 1
#	endif
#	ifndef _WIN32_WINNT /* currently mingw does not define, mingw64 does */
#		define _WIN32_WINNT 0x0500 /* At least windows 2000 */
#	endif
#	include <winsock2.h>
#	include <windows.h>
#	include <stdio.h>
#	include <stdlib.h>
#	include <time.h>
#	define __PRETTY_FUNCTION__ __FUNCDNAME__
#	ifdef YUNI_OS_MSVC
#		pragma warning(pop)
#	endif
#endif


/* On some compiler, the macro min() and max() are defined... (Visual Studio for example...) */
# ifdef min
#   undef min
# endif
# ifdef max
#   undef max
# endif





#ifdef YUNI_OS_WINDOWS
namespace Yuni
{
namespace Windows
{

	inline yint64 FILETIMEToTimestamp(const FILETIME& filetime)
	{
		LARGE_INTEGER date, adjust;
		date.HighPart = filetime.dwHighDateTime;
		date.LowPart = filetime.dwLowDateTime;

		// 100-nanoseconds = milliseconds * 10000
		adjust.QuadPart = 11644473600000 * 10000;

		// removes the diff between 1970 and 1601
		date.QuadPart -= adjust.QuadPart;

		// converts back from 100-nanoseconds to seconds
		return date.QuadPart / 10000000;
	}





} // namespace Windows
} // namespace Yuni
#endif
