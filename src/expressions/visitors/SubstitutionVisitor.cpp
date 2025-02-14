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

#include <algorithm>

#include <antares/expressions/nodes/ComponentNode.h>
#include <antares/expressions/visitors/SubstitutionVisitor.h>

namespace Antares::Expressions::Visitors
{
SubstitutionVisitor::SubstitutionVisitor(Registry<Nodes::Node>& registry, SubstitutionContext& ctx):
    CloneVisitor(registry),
    ctx_(ctx),
    registry_(registry)
{
}

Nodes::Node* SubstitutionVisitor::visit(const Nodes::ComponentVariableNode* node)
{
    // This search has linear complexity
    // To get a search of log complexity, we need to use std::unordered_set::find
    // But std::unordered_set::find_if does not exist
    auto it = std::find_if(ctx_.variables.begin(),
                           ctx_.variables.end(),
                           [&node](auto* x) { return *x == *node; });
    if (it != ctx_.variables.end())
    {
        return *it;
    }

    else
    {
        return CloneVisitor::visit(node);
    }
}

std::string SubstitutionVisitor::name() const
{
    return "SubstitutionVisitor";
}
} // namespace Antares::Expressions::Visitors
