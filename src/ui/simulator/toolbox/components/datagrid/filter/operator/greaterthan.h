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
#ifndef __ANTARES_TOOLBOX_FILTER_OPERATOR_GREATER_THAN_H__
#define __ANTARES_TOOLBOX_FILTER_OPERATOR_GREATER_THAN_H__

#include "../operator.h"

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
namespace Operator
{
class GreaterThan : public AOperator
{
public:
    GreaterThan(AFilterBase* parent) : AOperator(parent, wxT(">"), wxT(">"))
    {
    }
    virtual ~GreaterThan()
    {
    }

    virtual bool compute(const int a) const
    {
        return a > parameters[0].value.asInt;
    }

    virtual bool compute(const double a) const
    {
        return a > parameters[0].value.asDouble;
    }
    virtual bool compute(const wxString& a) const
    {
        return a > parameters[0].value.asString;
    }

}; // class GreaterThan

} // namespace Operator
} // namespace Filter
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_FILTER_OPERATOR_GREATER_THAN_H__
