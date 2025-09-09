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
#pragma once
#ifndef YUNI_NO_THREAD_SAFE
#ifdef YUNI_OS_WINDOWS
#include "../core/system/windows.hdr.h"
#define YUNI_THREAD_FNC_RETURN DWORD WINAPI
#else
#define YUNI_THREAD_FNC_RETURN void*
#endif
#endif

namespace Yuni::Thread
{
//! Return error status
enum Error
{
    //! No error, the operation succeeded
    errNone = 0,
    //! A timeout occured
    errTimeout,
    //! Impossible to create the new thread
    errThreadCreation,
    //! The onStarting handler returned false.
    errAborted,
    //! The operation failed for an unkown reason
    errUnkown
};

enum
{
    //! The default timeout for stopping a thread
    defaultTimeout = 5000, // 5 seconds
};

} // namespace Yuni::Thread

namespace Yuni::Job
{
// forward declaration
class YUNI_DECL IJob;

} // namespace Yuni::Job

#ifndef YUNI_NO_THREAD_SAFE

namespace Yuni::Private::Thread
{
/*!
** \brief This procedure will be run in a separate thread and will run IThread::onExecute()
*/
extern "C" YUNI_THREAD_FNC_RETURN threadCallbackExecute(void* arg);

} // namespace Yuni::Private::Thread

#endif // YUNI_NO_THREAD_SAFE
