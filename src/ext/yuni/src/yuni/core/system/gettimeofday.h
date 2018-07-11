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
#include "windows.hdr.h"



#define YUNI_HAS_GETTIMEOFDAY

#ifndef YUNI_OS_MSVC
# include <sys/time.h>
#else // YUNI_OS_MSVC

#	ifdef YUNI_OS_WINDOWS
#		include <time.h>
#		if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#			define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#		else
#			define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#		endif
#	endif

#	ifdef YUNI_HAS_GETTIMEOFDAY
#		undef YUNI_HAS_GETTIMEOFDAY
#	endif

#endif // YUNI_OS_MSVC



namespace Yuni
{

	# ifndef YUNI_HAS_GETTIMEOFDAY

	struct timezone
	{
		int  tz_minuteswest; // minutes W of Greenwich
		int  tz_dsttime;	 // type of dst correction
	};

	struct timeval
	{
		sint64 tv_sec;
		sint64 tv_usec;
	};

	YUNI_DECL int gettimeofday(struct timeval *tv, struct timezone *tz);

	# define YUNI_SYSTEM_GETTIMEOFDAY  ::Yuni::gettimeofday

	# else

	typedef struct timezone timezone;
	typedef struct timeval timeval;
	#	ifndef YUNI_SYSTEM_GETTIMEOFDAY
	#		define YUNI_SYSTEM_GETTIMEOFDAY  ::gettimeofday
	#	endif

	# endif

} // namespace Yuni

