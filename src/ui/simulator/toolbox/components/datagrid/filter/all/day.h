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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_DAY_H__
#define __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_DAY_H__

#include "../filter.h"
#include <antares/date/date.h>

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
class Day : public AFilterBase
{
public:
    static const wxChar* Name()
    {
        return wxT("day");
    }
    static const wxChar* Caption()
    {
        return wxT("Day of the month");
    }
    static Date::Precision Precision()
    {
        return Date::daily;
    }

public:
    Day(Input* parent) : AFilterBase(parent)
    {
        operators.addStdArithmetic();
    }

    virtual ~Day()
    {
    }

    virtual Date::Precision precision() const
    {
        return Day::Precision();
    }

    virtual bool checkOnRowsLabels() const
    {
        return true;
    }

    virtual const wxChar* name() const
    {
        return Day::Name();
    }
    virtual const wxChar* caption() const
    {
        return Day::Caption();
    }

    virtual bool rowIsValid(int row) const
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
                uint daymonth = calendar.hours[row].dayMonth + 1;
                return currentOperator->compute((int)daymonth);
            }
            break;
        }
        case Date::daily:
        {
            if (row < study.calendar.maxDaysInYear)
            {
                uint daymonth = calendar.days[row].dayMonth + 1;
                return currentOperator->compute((int)daymonth);
            }
            break;
        }
        default:
            break;
        }
        return false;
    }

}; // class HourYear

} // namespace Filter
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_DAY_H__
