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

#include "constraint.h"
#include <cassert>
#include "../resources.h"

using namespace Yuni;

namespace Antares
{
namespace Toolbox
{
namespace Spotlight
{
ItemConstraint::ItemConstraint(Data::BindingConstraint* a) : constraint(a)
{
    assert(a);

    caption(a->name());
    group("Constraint");

    String tmp;
    tmp.clear();
    tmp << '(';
    tmp << Data::BindingConstraint::OperatorToShortCString(a->operatorType());
    tmp << ", ";
    tmp << Data::BindingConstraint::TypeToCString(a->type());
    tmp << ")  ";
    if (!a->comments())
        a->buildFormula(tmp);
    else
        tmp << a->comments();
    subcaption(tmp);

    // Tag
    addTag(" Bc ", 137, 157, 199);
}

ItemConstraint::~ItemConstraint()
{
}

} // namespace Spotlight
} // namespace Toolbox
} // namespace Antares
