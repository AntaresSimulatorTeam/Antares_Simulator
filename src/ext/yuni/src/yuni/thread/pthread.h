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


# ifndef YUNI_NO_THREAD_SAFE
#	ifndef YUNI_OS_WINDOWS
#		ifndef _MULTI_THREADED
#			define _MULTI_THREADED
#		endif
#		include <pthread.h>
#	endif
# endif

