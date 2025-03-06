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
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/SearchVisitor.h>

namespace Antares::Expressions::Visitors
{

void SearchVisitor::addToVectorIfNameMatches(const Nodes::Node* node, const std::string& name)
{
    if (node->name() == name)
    {
        results_.emplace_back(node);
    }
}

void SearchVisitor::visit(const Nodes::SumNode* node, const std::string& name)
{
    addToVectorIfNameMatches(node, name);
    for (auto* operand: node->getOperands())
    {
        dispatch(operand, name);
    }
}

void SearchVisitor::visit(const Nodes::SubtractionNode* node, const std::string& name)
{
    addToVectorIfNameMatches(node, name);
    dispatch(node->left(), name);
    dispatch(node->right(), name);
}

void SearchVisitor::visit(const Nodes::MultiplicationNode* node, const std::string& name)
{
    addToVectorIfNameMatches(node, name);
    dispatch(node->left(), name);
    dispatch(node->right(), name);
}

void SearchVisitor::visit(const Nodes::DivisionNode* node, const std::string& name)
{
    addToVectorIfNameMatches(node, name);
    dispatch(node->left(), name);
    dispatch(node->right(), name);
}

void SearchVisitor::visit(const Nodes::EqualNode* node, const std::string& name)
{
    addToVectorIfNameMatches(node, name);
    dispatch(node->left(), name);
    dispatch(node->right(), name);
}

void SearchVisitor::visit(const Nodes::LessThanOrEqualNode* node, const std::string& name)
{
    addToVectorIfNameMatches(node, name);
    dispatch(node->left(), name);
    dispatch(node->right(), name);
}

void SearchVisitor::visit(const Nodes::GreaterThanOrEqualNode* node, const std::string& name)
{
    addToVectorIfNameMatches(node, name);
    dispatch(node->left(), name);
    dispatch(node->right(), name);
}

void SearchVisitor::visit(const Nodes::NegationNode* node, const std::string& name)
{
    addToVectorIfNameMatches(node, name);
    dispatch(node->child(), name);
}

std::string SearchVisitor::name() const
{
    return "SearchVisitor";
}

} // namespace Antares::Expressions::Visitors
