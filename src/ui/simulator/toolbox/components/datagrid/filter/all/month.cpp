// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "month.h"
#include <antares/study/study.h>
#include "application/study.h"

namespace Antares::Toolbox::Filter
{
bool Month::rowIsValid(int row) const
{
    // TODO Do not use global study
    auto studyptr = GetCurrentStudy();
    if (!studyptr)
    {
        return false;
    }
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

} // namespace Antares::Toolbox::Filter
