// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_DAYYEAR_H__
#define __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_DAYYEAR_H__

#include "../filter.h"
#include <antares/date/date.h>

namespace Antares::Toolbox::Filter
{
class DayYear: public AFilterBase
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
    DayYear(Input* parent):
        AFilterBase(parent)
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

} // namespace Antares::Toolbox::Filter

#endif // __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_DAYYEAR_H__
