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
#include "../yuni.h"
# ifdef YUNI_OS_LINUX
#	ifndef _GNU_SOURCE
#	define _GNU_SOURCE /* ou _BSD_SOURCE or _SVID_SOURCE */
#	endif
#	include <unistd.h>
#	include <sys/types.h>
#	include <sys/syscall.h>
# endif
#include "id.h"
#include "../core/system/windows.hdr.h"
# ifndef YUNI_NO_THREAD_SAFE
#	include "pthread.h"
#	ifdef YUNI_OS_WINDOWS
#		include "../core/system/windows.hdr.h"
# 	endif
# endif
# ifdef YUNI_HAS_PTHREAD_GETTHREADID_NP
# include <pthread.h>
# endif


namespace Yuni
{
namespace Thread
{

	# ifndef YUNI_NO_THREAD_SAFE
	uint64 ID()
	{
		# ifdef YUNI_HAS_PTHREAD_GETTHREADID_NP
		return (uint64) pthread_getthreadid_np();
		# else

		#	ifdef YUNI_OS_MAC
		return (uint64) pthread_self();
		#	else
		#		ifndef YUNI_OS_WINDOWS
		#			ifdef YUNI_OS_LINUX
		return (uint64) syscall(SYS_gettid);
		#			else
		// man : The pthread_self() function returns the thread ID of the calling thread
		return (uint64) pthread_self();
		#			endif
		#		else
		return (uint64) GetCurrentThreadId();
		#		endif
		#	endif

		# endif
		return 0;
	}

	# else

	uint64 ID()
	{
		return 0;
	}

	# endif



} // namespace Thread
} // namespace Yuni

