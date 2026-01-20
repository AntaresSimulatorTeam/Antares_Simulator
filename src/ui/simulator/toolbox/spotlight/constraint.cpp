// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "constraint.h"
#include <cassert>
#include "../resources.h"

using namespace Yuni;

namespace Antares::Toolbox::Spotlight
{
ItemConstraint::ItemConstraint(Data::BindingConstraint* a):
    constraint(a)
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
    {
        a->buildFormula(tmp);
    }
    else
    {
        tmp << a->comments();
    }
    subcaption(tmp);

    // Tag
    addTag(" Bc ", 137, 157, 199);
}

ItemConstraint::~ItemConstraint()
{
}

} // namespace Antares::Toolbox::Spotlight
