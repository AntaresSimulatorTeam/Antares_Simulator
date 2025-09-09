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
#include "../yuni.h"
#include "timer.h"
#include "../job/job.h"
#include "../core/bind.h"

namespace Yuni::Job
{
class QueueService;
}

namespace Yuni::Job
{
class Taskgroup;
}

namespace Yuni
{
/*!
** \brief Convenient wrapper for executing some code every X milliseconds
**
** The code will be executed into another thread.
** From sample:
** \code
** #include <yuni/yuni.h>
** #include <yuni/thread/thread.h>
** #include <yuni/core/system/suspend.h>
** #include <iostream>
** using namespace Yuni;
**
** int main()
** {
**	uint counter = 0;
**	auto timer = every(1000, [&] () -> bool {
**		std::cout << ++counter << std::endl;
**		return true; // continue looping
**	});
**
**	Suspend(5); // wait for 5 seconds
**	return 0;
** }
** \endcode
**
** \param ms An amount of time, in milliseconds
** \param callback The callback to execute
** \param autostart True to automatically start the timer
*/
Thread::Timer::Ptr every(uint ms, const Bind<bool()>& callback, bool autostart = true);

/*!
** \brief Convenient wrapper for executing some code every X milliseconds and
** getting the time elapsed between each tick
**
** The code will be executed into another thread.
** \code
** #include <yuni/yuni.h>
** #include <yuni/thread/thread.h>
** #include <yuni/core/system/suspend.h>
** #include <iostream>
** using namespace Yuni;
**
** int main()
** {
**	auto timer = every(1000, false, [&] (uint64_t elapsed) -> bool {
**		std::cout << elapsed << "ms since the last time" << std::endl;
**		return true; // continue looping
**	});
**
**	Suspend(5); // wait for 5 seconds
**	return 0;
** }
** \endcode
**
** \param ms An amount of time, in milliseconds
** \param precise True to not take into consideration the time spent in the callback (with
*performance penalty)
** \param callback The callback to execute
** \param autostart True to automatically start the timer
*/
Thread::Timer::Ptr every(uint ms,
                         bool precise,
                         const Bind<bool(uint64_t /*elapsed*/)>& callback,
                         bool autostart = true);

} // namespace Yuni
