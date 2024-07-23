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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "month.h"
#include <antares/study/study.h>
#include "application/study.h"

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
bool Month::rowIsValid(int row) const
{
    // TODO Do not use global study
    auto studyptr = GetCurrentStudy();
    if (!studyptr)
        return false;
    auto& study = *studyptr;

    switch (pDataGridPrecision)
    {
    case Date::hourly:
    {
        if (row < study.calendar.maxHoursInYear)
        {
            uint calendarMonth = study.calendar.hours[row].month;
            uint realmonth = study.calendar.months[calendarMonth].realmonth;
            return currentOperator->compute((int)realmonth);
        }
        break;
    }
    case Date::daily:
    {
        if (row < study.calendar.maxDaysInYear)
        {
            uint calendarMonth = study.calendar.days[row].month;
            uint realmonth = study.calendar.months[calendarMonth].realmonth;
            return currentOperator->compute((int)realmonth);
        }
        break;
    }
    case Date::monthly:
    {
        if (row < 12 + 1)
        {
            // uint realmonth = study.calendar.months[row].realmonth;
            return currentOperator->compute((int)row);
        }
        break;
    }
    case Date::weekly:
    case Date::annual:
        break;
    case Date::stepAny:
    case Date::stepNone:
        break;
    }
    return false;
}

} // namespace Filter
} // namespace Toolbox
} // namespace Antares
