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
#include <boost/functional/hash.hpp>

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/expressions/visitors/PortfieldSubstitutionVisitor.h>

namespace Antares::Solver::Visitors
{

PortfieldSubstitutionVisitor::PortfieldSubstitutionVisitor(Registry<Nodes::Node>& registry,
                                                           PortfieldSubstitutionContext& ctx):
    CloneVisitor(registry),
    ctx_(ctx)
{
}

Nodes::Node* PortfieldSubstitutionVisitor::visit(const Nodes::PortFieldNode* node)
{
    if (ctx_.portfield.contains(*node))
    {
        return (ctx_.portfield.at(*node));
    }

    return CloneVisitor::visit(node);
}

std::string PortfieldSubstitutionVisitor::name() const
{
    return "PortfieldSubstitutionVisitor";
}

std::size_t KeyHasher::operator()(const Nodes::PortFieldNode& n) const
{
    std::size_t seed = 0;

    boost::hash_combine(seed, boost::hash_value(n.getPortName()));
    boost::hash_combine(seed, boost::hash_value(n.getFieldName()));

    return seed;
}

} // namespace Antares::Solver::Visitors