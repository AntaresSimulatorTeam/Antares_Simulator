// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_DAY_H__
#define __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_DAY_H__

#include "../filter.h"
#include <antares/date/date.h>

namespace Antares::Toolbox::Filter
{
class Day: public AFilterBase
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
    Day(Input* parent):
        AFilterBase(parent)
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

} // namespace Antares::Toolbox::Filter

#endif // __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_DAY_H__
