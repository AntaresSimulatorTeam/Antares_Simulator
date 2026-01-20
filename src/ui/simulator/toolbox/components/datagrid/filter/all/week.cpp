// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "week.h"
#include <antares/date/date.h>
#include <antares/study/study.h>
#include "application/study.h"

namespace Antares::Toolbox::Filter
{
Week::Week(Input* parent):
    AFilterBase(parent)
{
    operators.addStdArithmetic();
}

bool Week::rowIsValid(int row) const
{
    // TODO Do not use global study
    auto studyptr = GetCurrentStudy();
    if (!studyptr)
    {
        return false;
    }
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

} // namespace Antares::Toolbox::Filter
