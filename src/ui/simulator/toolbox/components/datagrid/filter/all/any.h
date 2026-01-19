// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_FILTER_ALL_ANY_H__
#define __ANTARES_TOOLBOX_FILTER_ALL_ANY_H__

#include "../filter.h"

namespace Antares::Toolbox::Filter
{
class Any: public AFilterBase
{
public:
    static const wxChar* Name()
    {
        return wxT("any");
    }

    static const wxChar* Caption()
    {
        return wxT("Any");
    }

    static Date::Precision Precision()
    {
        return Date::stepAny;
    }

public:
    Any(Input* parent):
        AFilterBase(parent)
    {
    }

    virtual ~Any()
    {
    }

    virtual Date::Precision precision() const
    {
        return Any::Precision();
    }

    virtual const wxChar* name() const
    {
        return Any::Name();
    }

    virtual const wxChar* caption() const
    {
        return Any::Caption();
    }

}; // class Any

} // namespace Antares::Toolbox::Filter

#endif // __ANTARES_TOOLBOX_FILTER_FILTER_H__
