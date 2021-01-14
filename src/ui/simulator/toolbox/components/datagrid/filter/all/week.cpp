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

#include "week.h"
#include <antares/date.h>
#include <antares/study/study.h>

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
Week::Week(Input* parent) : AFilterBase(parent)
{
    operators.addStdArithmetic();
}

bool Week::rowIsValid(int row) const
{
    // TODO Do not use global study
    auto studyptr = Data::Study::Current::Get();
    if (!studyptr)
        return false;
    auto& study = *studyptr;
    auto& calendar = study.calendar;

    switch (pDataGridPrecision)
    {
    case Date::hourly:
    {
        if (row < study.calendar.maxHoursInYear)
        {
            uint weekindex = calendar.hours[row].week;
            uint userweek = calendar.weeks[weekindex].userweek;
            return currentOperator->compute((int)userweek);
        }
        break;
    }
    case Date::daily:
    {
        if (row < study.calendar.maxDaysInYear)
        {
            uint weekindex = calendar.days[row].week;
            uint userweek = calendar.weeks[weekindex].userweek;
            return currentOperator->compute((int)userweek);
        }
        break;
    }
    case Date::weekly:
    {
        if (row < study.calendar.maxWeeksInYear)
        {
            uint userweek = calendar.weeks[row].userweek;
            return currentOperator->compute((int)userweek);
        }
        break;
    }
    default:
        break;
    }
    return false;
}

} // namespace Filter
} // namespace Toolbox
} // namespace Antares
