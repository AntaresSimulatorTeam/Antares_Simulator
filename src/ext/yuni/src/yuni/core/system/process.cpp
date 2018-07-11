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
#include "process.h"
#ifndef YUNI_OS_WINDOWS
#	ifndef YUNI_OS_HAIKU
#		include <sys/errno.h>
#	endif
#	include <unistd.h>
#else
#	include "windows.hdr.h"
#endif




namespace Yuni
{

	uint64 ProcessID()
	{
		#if defined(YUNI_OS_WINDOWS)
		return (uint64) GetCurrentProcessId();
		#else
		return (uint64) getpid();
		#endif
	}



} // namespace Yuni
