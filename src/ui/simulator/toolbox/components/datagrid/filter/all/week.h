// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_WEEK_H__
#define __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_WEEK_H__

#include "../filter.h"

namespace Antares::Toolbox::Filter
{
class Week: public AFilterBase
{
public:
    static const wxChar* Name()
    {
        return wxT("week");
    }

    static const wxChar* Caption()
    {
        return wxT("Week");
    }

    static Date::Precision Precision()
    {
        return Date::weekly;
    }

public:
    Week(Input* parent);

    virtual ~Week()
    {
    }

    virtual Date::Precision precision() const
    {
        return Week::Precision();
    }

    virtual bool checkOnRowsLabels() const
    {
        return true;
    }

    virtual const wxChar* name() const
    {
        return Week::Name();
    }

    virtual const wxChar* caption() const
    {
        return Week::Caption();
    }

    virtual bool rowIsValid(int row) const;

}; // class Week

} // namespace Antares::Toolbox::Filter

#endif // __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_WEEK_H__
