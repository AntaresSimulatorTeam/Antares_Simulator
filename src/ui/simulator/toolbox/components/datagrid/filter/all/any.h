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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_TOOLBOX_FILTER_ALL_ANY_H__
#define __ANTARES_TOOLBOX_FILTER_ALL_ANY_H__

#include "../filter.h"

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
class Any : public AFilterBase
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
    Any(Input* parent) : AFilterBase(parent)
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

} // namespace Filter
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_FILTER_FILTER_H__
