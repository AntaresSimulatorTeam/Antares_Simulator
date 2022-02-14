/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <yuni/yuni.h>
#include "timer.h"
#include "aggregator.h"
#include "../logs.h"
#include <math.h>
#include <yuni/core/system/gettimeofday.h>

using namespace Yuni;

static inline sint64 MilliSecTimer()
{
    Yuni::timeval tv;
    YUNI_SYSTEM_GETTIMEOFDAY(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

namespace TimeElapsed
{
void Timer::reset()
{
    pStartTime = MilliSecTimer();
}

Timer::Timer(const AnyString& userText,
             const AnyString& logText,
             bool verbose,
             Aggregator* aggregator) :
userText(userText), logText(logText), verbose(verbose), pAggregator(aggregator)
{
    reset();
}

Timer::~Timer()
{
    const sint64 delta_ms = MilliSecTimer() - pStartTime;
    if (verbose)
    {
        Antares::logs.info() << " Elapsed time: " << userText << ": " << delta_ms << "ms";
    }
    if (pAggregator)
    {
        pAggregator->append(logText, delta_ms);
    }
}
} // namespace TimeElapsed
