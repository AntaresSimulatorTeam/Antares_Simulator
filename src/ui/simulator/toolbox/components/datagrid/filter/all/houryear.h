// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_FILTER_ALL_HOUR_YEAR_H__
#define __ANTARES_TOOLBOX_FILTER_ALL_HOUR_YEAR_H__

#include "../filter.h"

namespace Antares::Toolbox::Filter
{
class HourYear: public AFilterBase
{
public:
    static const wxChar* Name()
    {
        return wxT("houryear");
    }

    static const wxChar* Caption()
    {
        return wxT("Hour Year");
    }

    static Date::Precision Precision()
    {
        return Date::hourly;
    }

public:
    HourYear(Input* parent):
        AFilterBase(parent)
    {
        operators.addStdArithmetic();
    }

    virtual ~HourYear()
    {
    }

    virtual Date::Precision precision() const
    {
        return HourYear::Precision();
    }

    virtual bool checkOnRowsLabels() const
    {
        return true;
    }

    virtual const wxChar* name() const
    {
        return HourYear::Name();
    }

    virtual const wxChar* caption() const
    {
        return HourYear::Caption();
    }

    virtual bool rowIsValid(int row) const
    {
        return currentOperator->compute(row + 1);
    }

}; // class HourYear

} // namespace Antares::Toolbox::Filter

#endif // __ANTARES_TOOLBOX_FILTER_FILTER_H__
