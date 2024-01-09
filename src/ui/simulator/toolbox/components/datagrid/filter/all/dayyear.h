/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
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
**
** SPDX-License-Identifier: MPL-2.0
*/
#ifndef __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_DAYYEAR_H__
#define __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_DAYYEAR_H__

#include "../filter.h"
#include <antares/date/date.h>

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
class DayYear : public AFilterBase
{
public:
    static const wxChar* Name()
    {
        return wxT("dayyear");
    }
    static const wxChar* Caption()
    {
        return wxT("Day of the year");
    }
    static Date::Precision Precision()
    {
        return Date::daily;
    }

public:
    DayYear(Input* parent) : AFilterBase(parent)
    {
        operators.addStdArithmetic();
    }

    virtual ~DayYear()
    {
    }

    virtual Date::Precision precision() const
    {
        return DayYear::Precision();
    }

    virtual bool checkOnRowsLabels() const
    {
        return true;
    }

    virtual const wxChar* name() const
    {
        return DayYear::Name();
    }
    virtual const wxChar* caption() const
    {
        return DayYear::Caption();
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
                uint dayyear = calendar.hours[row].dayYear + 1;
                return currentOperator->compute((int)dayyear);
            }
            break;
        }
        case Date::daily:
        {
            if (row < study.calendar.maxDaysInYear)
            {
                return currentOperator->compute((int)row + 1);
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

#endif // __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_DAYYEAR_H__
