/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
**
** SPDX-License-Identifier: MPL 2.0
*/
#ifndef __ANTARES_TOOLBOX_FILTER_OPERATOR_MODULO_H__
#define __ANTARES_TOOLBOX_FILTER_OPERATOR_MODULO_H__

#include "../operator.h"

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
namespace Operator
{
class Modulo : public AOperator
{
public:
    Modulo(AFilterBase* parent) : AOperator(parent, wxT("%"), wxT("modulo"))
    {
        // When the operator is the modulo, we must have another
        // value
        parameters.push_back(Parameter(*this).presetModuloAddon());
    }

    virtual ~Modulo()
    {
    }

    virtual bool compute(const int a) const
    {
        return parameters[0].value.asInt
                 ? (a % parameters[0].value.asInt == parameters[1].value.asInt)
                 : false;
    }

    virtual bool compute(const double a) const
    {
        return (int(floor(a)) % parameters[0].value.asInt == parameters[1].value.asInt);
    }

    virtual bool compute(const wxString&) const
    {
        return false;
    }

}; // class Modulo

} // namespace Operator
} // namespace Filter
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_FILTER_OPERATOR_MODULO_H__
