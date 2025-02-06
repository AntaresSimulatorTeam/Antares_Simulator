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
#include <antares/expressions/visitors/CompareVisitor.h>

template<class T, class V>
static bool compareBinaryNode(V& visitor,
                              const T* node,
                              const Antares::Expressions::Nodes::Node* other)
{
    if (const T* other_node = dynamic_cast<const T*>(other))
    {
        bool left = visitor.dispatch(node->left(), other_node->left());
        bool right = visitor.dispatch(node->right(), other_node->right());
        return left && right;
    }
    return false;
}

template<class T>
static bool compareGetValue(const T* node, const Antares::Expressions::Nodes::Node* other)
{
    if (const T* other_node = dynamic_cast<const T*>(other))
    {
        return node->value() == other_node->value();
    }
    return false;
}

template<class T>
static bool compareEqualOperator(const T* node, const Antares::Expressions::Nodes::Node* other)
{
    if (const T* other_node = dynamic_cast<const T*>(other))
    {
        return *node == *other_node;
    }
    return false;
}

namespace Antares::Expressions::Visitors
{
bool CompareVisitor::visit(const Nodes::SumNode* node, const Nodes::Node* other)
{
    if (const auto* other_node = dynamic_cast<const Nodes::SumNode*>(other))
    {
        if (node->size() != other_node->size())
        {
            return false;
        }
        for (std::size_t i = 0; i < node->size(); ++i)
        {
            if (!dispatch(node->getOperands()[i], other_node->getOperands()[i]))
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool CompareVisitor::visit(const Nodes::SubtractionNode* node, const Nodes::Node* other)
{
    return compareBinaryNode(*this, node, other);
}

bool CompareVisitor::visit(const Nodes::MultiplicationNode* node, const Nodes::Node* other)
{
    return compareBinaryNode(*this, node, other);
}

bool CompareVisitor::visit(const Nodes::DivisionNode* node, const Nodes::Node* other)
{
    return compareBinaryNode(*this, node, other);
}

bool CompareVisitor::visit(const Nodes::EqualNode* node, const Nodes::Node* other)
{
    return compareBinaryNode(*this, node, other);
}

bool CompareVisitor::visit(const Nodes::LessThanOrEqualNode* node, const Nodes::Node* other)
{
    return compareBinaryNode(*this, node, other);
}

bool CompareVisitor::visit(const Nodes::GreaterThanOrEqualNode* node, const Nodes::Node* other)
{
    return compareBinaryNode(*this, node, other);
}

bool CompareVisitor::visit(const Nodes::NegationNode* node, const Nodes::Node* other)
{
    if (auto* other_node = dynamic_cast<const Nodes::NegationNode*>(other))
    {
        return dispatch(node->child(), other_node->child());
    }
    return false;
}

bool CompareVisitor::visit(const Nodes::ParameterNode* node, const Nodes::Node* other)
{
    return compareGetValue(node, other);
}

bool CompareVisitor::visit(const Nodes::LiteralNode* node, const Nodes::Node* other)
{
    return compareGetValue(node, other);
}

bool CompareVisitor::visit(const Nodes::VariableNode* node, const Nodes::Node* other)
{
    return compareGetValue(node, other);
}

bool CompareVisitor::visit(const Nodes::PortFieldNode* node, const Nodes::Node* other)
{
    return compareEqualOperator(node, other);
}

bool CompareVisitor::visit(const Nodes::PortFieldSumNode* node, const Nodes::Node* other)
{
    return compareEqualOperator(node, other);
}

bool CompareVisitor::visit(const Nodes::ComponentVariableNode* node, const Nodes::Node* other)
{
    return compareEqualOperator(node, other);
}

bool CompareVisitor::visit(const Nodes::ComponentParameterNode* node, const Nodes::Node* other)
{
    return compareEqualOperator(node, other);
}

std::string CompareVisitor::name() const
{
    return "CompareVisitor";
}

} // namespace Antares::Expressions::Visitors
