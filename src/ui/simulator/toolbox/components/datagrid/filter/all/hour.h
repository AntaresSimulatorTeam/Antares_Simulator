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
#ifndef __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_HOUR_H__
#define __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_HOUR_H__

#include "../filter.h"
#include <antares/date/date.h>

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
class Hour : public AFilterBase
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
    Hour(Input* parent) : AFilterBase(parent)
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
            return currentOperator->compute((int)studyptr->calendar.hours[row].dayHour + 1);
        return false;
    }

}; // class Hour

} // namespace Filter
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_HOUR_H__
