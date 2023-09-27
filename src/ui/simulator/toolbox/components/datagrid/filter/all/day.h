/*
** Copyright 2007-2023 RTE
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
