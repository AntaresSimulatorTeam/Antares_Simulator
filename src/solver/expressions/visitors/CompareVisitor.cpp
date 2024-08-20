/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/expressions/visitors/CompareVisitor.h>

namespace Antares::Solver::Visitors
{
bool CompareVisitor::visit(const Nodes::AddNode& add, const Nodes::Node& other)
{
    if (auto* other_add = dynamic_cast<const Nodes::AddNode*>(&other))
    {
        bool left = dispatch(*add.left(), *other_add->left());
        bool right = dispatch(*add.right(), *other_add->right());
        return left && right;
    }
    return false;
}

bool CompareVisitor::visit(const Nodes::ParameterNode& parameter, const Nodes::Node& other)
{
    if (auto* other_parameter = dynamic_cast<const Nodes::ParameterNode*>(&other))
    {
        return parameter.getValue() == other_parameter->getValue();
    }
    return false;
}

bool CompareVisitor::visit(const Nodes::LiteralNode& literal, const Nodes::Node& other)
{
    if (auto* other_literal = dynamic_cast<const Nodes::LiteralNode*>(&other))
    {
        return literal.getValue() == other_literal->getValue();
    }
    return false;
}
} // namespace Antares::Solver::Visitors
