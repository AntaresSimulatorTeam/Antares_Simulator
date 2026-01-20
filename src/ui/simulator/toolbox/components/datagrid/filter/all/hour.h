// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_HOUR_H__
#define __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_HOUR_H__

#include "../filter.h"
#include <antares/date/date.h>

namespace Antares::Toolbox::Filter
{
class Hour: public AFilterBase
{
public:
    static const wxChar* Name()
    {
        return wxT("hour");
    }

    static const wxChar* Caption()
    {
        return wxT("Day Hour");
    }

    static Date::Precision Precision()
    {
        return Date::hourly;
    }

public:
    Hour(Input* parent):
        AFilterBase(parent)
    {
        operators.addStdArithmetic();
    }

    virtual ~Hour()
    {
    }

    virtual Date::Precision precision() const
    {
        return Hour::Precision();
    }

    virtual bool checkOnRowsLabels() const
    {
        return true;
    }

    virtual const wxChar* name() const
    {
        return Hour::Name();
    }

    virtual const wxChar* caption() const
    {
        return Hour::Caption();
    }

    virtual bool rowIsValid(int row) const
    {
        // TODO Do not use global study
        auto studyptr = GetCurrentStudy();
        if (!(!studyptr) && row < studyptr->calendar.maxHoursInYear)
        {
            return currentOperator->compute((int)studyptr->calendar.hours[row].dayHour + 1);
        }
        return false;
    }

}; // class Hour

} // namespace Antares::Toolbox::Filter

#endif // __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_HOUR_H__
