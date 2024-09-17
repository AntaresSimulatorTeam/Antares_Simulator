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
#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/expressions/visitors/PortfieldSubstitutionVisitor.h>

namespace Antares::Solver::Visitors
{

PortfieldSubstitutionVisitor::PortfieldSubstitutionVisitor(Registry<Nodes::Node>& registry,
                                                           PortfieldSubstitutionContext& ctx):
    CloneVisitor(registry),
    ctx_(ctx),
    registry_(registry)
{
}

Nodes::Node* PortfieldSubstitutionVisitor::visit(const Nodes::PortFieldNode* node)
{
    // This search has linear complexity
    // To get a search of log complexity, we need to use std::unordered_set::find
    // But std::unordered_set::find_if does not exist
    auto it = std::ranges::find_if(ctx_.portfield, [&node](auto* x) { return *x == *node; });
    if (it != ctx_.portfield.end())
    {
        return *it;
    }

    return CloneVisitor::visit(node);
}

std::string PortfieldSubstitutionVisitor::name() const
{
    return "PortfieldSubstitutionVisitor";
}

} // namespace Antares::Solver::Visitors
