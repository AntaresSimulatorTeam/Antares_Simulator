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
#include "suspend.h"
#ifdef YUNI_OS_WINDOWS
# include "windows.hdr.h"
#else
# ifdef YUNI_HAS_UNISTD_H
#	include <unistd.h>
# endif
#endif



namespace Yuni
{

	void Suspend(uint seconds)
	{
		# if defined(YUNI_OS_WINDOWS)
		Sleep(1000 * seconds);
		# else
		#	ifdef YUNI_HAS_UNISTD_H
		::sleep(seconds);
		#	else
		#	warning function sleep() missing
		#	endif
		# endif
	}


	void SuspendMilliSeconds(uint milliseconds)
	{
		# if defined(YUNI_OS_WINDOWS)
		Sleep(milliseconds);
		# else
		#	ifdef YUNI_HAS_UNISTD_H
		::usleep(1000 * milliseconds);
		#	else
		#	warning function usleep() missing
		#	endif
		# endif
	}



} // namespace Yuni

