/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

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
#ifdef YUNI_OS_LINUX
#ifndef _GNU_SOURCE
#define _GNU_SOURCE /* ou _BSD_SOURCE or _SVID_SOURCE */
#endif
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#endif
#include "id.h"
#include "../core/system/windows.hdr.h"
#ifndef YUNI_NO_THREAD_SAFE
#include "pthread.h"
#ifdef YUNI_OS_WINDOWS
#include "../core/system/windows.hdr.h"
#endif
#endif
#ifdef YUNI_HAS_PTHREAD_GETTHREADID_NP
#include <pthread.h>
#endif

namespace Yuni::Thread
{
#ifndef YUNI_NO_THREAD_SAFE
uint64_t ID()
{
#ifdef YUNI_HAS_PTHREAD_GETTHREADID_NP
    return (uint64_t)pthread_getthreadid_np();
#else

#ifdef YUNI_OS_MAC
    return (uint64_t)pthread_self();
#else
#ifndef YUNI_OS_WINDOWS
#ifdef YUNI_OS_LINUX
    return (uint64_t)syscall(SYS_gettid);
#else
    // man : The pthread_self() function returns the thread ID of the calling thread
    return (uint64_t)pthread_self();
#endif
#else
    return (uint64_t)GetCurrentThreadId();
#endif
#endif

#endif
    return 0;
}

#else

uint64_t ID()
{
    return 0;
}

#endif

} // namespace Yuni::Thread
