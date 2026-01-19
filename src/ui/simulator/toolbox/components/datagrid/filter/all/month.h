// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_MONTH_H__
#define __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_MONTH_H__

#include "../filter.h"

namespace Antares::Toolbox::Filter
{
class Month: public AFilterBase
{
public:
    static const wxChar* Name()
    {
        return wxT("month");
    }

    static const wxChar* Caption()
    {
        return wxT("Month");
    }

    static Date::Precision Precision()
    {
        return Date::monthly;
    }

public:
    Month(Input* parent):
        AFilterBase(parent)
    {
        operators.addStdMonth();
    }

    virtual ~Month()
    {
    }

    virtual Date::Precision precision() const
    {
        return Month::Precision();
    }

    virtual bool checkOnRowsLabels() const
    {
        return true;
    }

    virtual const wxChar* name() const
    {
        return Month::Name();
    }

    virtual const wxChar* caption() const
    {
        return Month::Caption();
    }

    virtual bool rowIsValid(int row) const;

}; // class HourYear

} // namespace Antares::Toolbox::Filter

#endif // __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_MONTH_H__
