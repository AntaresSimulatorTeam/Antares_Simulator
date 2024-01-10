/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "week.h"
#include <antares/date/date.h>
#include <antares/study/study.h>
#include "application/study.h"

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
    auto studyptr = GetCurrentStudy();
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
