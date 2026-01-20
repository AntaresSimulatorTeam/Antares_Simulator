// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_WEEKDAY_H__
#define __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_WEEKDAY_H__

#include "../filter.h"
#include <antares/date/date.h>
#include <antares/study/study.h>
#include "application/study.h"

namespace Antares::Toolbox::Filter
{
class Weekday: public AFilterBase
{
public:
    static const wxChar* Name()
    {
        return wxT("weekday");
    }

    static const wxChar* Caption()
    {
        return wxT("WeekDay");
    }

    static Date::Precision Precision()
    {
        return Date::daily;
    }

public:
    Weekday(Input* parent):
        AFilterBase(parent)
    {
        operators.addStdWeekday();
    }

    virtual ~Weekday()
    {
    }

    virtual Date::Precision precision() const
    {
        return Weekday::Precision();
    }

    virtual bool checkOnRowsLabels() const
    {
        return true;
    }

    virtual const wxChar* name() const
    {
        return Weekday::Name();
    }

    virtual const wxChar* caption() const
    {
        return Weekday::Caption();
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
                uint w = calendar.hours[row].weekday;
                return currentOperator->compute((int)w);
            }
            break;
        }
        case Date::daily:
        {
            if (row < study.calendar.maxDaysInYear)
            {
                uint w = calendar.days[row].weekday;
                return currentOperator->compute((int)w);
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

#endif // __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_WEEKDAY_H__
