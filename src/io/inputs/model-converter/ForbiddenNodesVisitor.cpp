/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#include "include/antares/io/inputs/model-converter/ForbiddenNodesVisitor.h"

#include <antares/expressions/nodes/ExpressionsNodes.h>
using namespace Antares::Expressions::Nodes;

namespace Antares::IO::Inputs::ModelConverter
{

std::string ErrorMessage(const std::string expr, const std::string node, const std::string parent)
{
    if (!parent.empty())
    {
        std::string format_str = "'{}' is not allowed to contain '{}' in expression '{}'";
        return fmt::format(fmt::runtime(format_str), parent, node, expr);
    }
    return fmt::format("'{}' is not allowed in expression '{}'", node, expr);
}

ForbiddenNodeFound::ForbiddenNodeFound(const std::string expr,
                                       const std::string node,
                                       const std::string parent):
    invalid_argument(ErrorMessage(expr, node, parent))
{
}

ForbiddenNodesVisitor::ForbiddenNodesVisitor(const ForbiddenNodes& forbiddenNodes,
                                             const std::string& expression):
    forbiddenNodes_(forbiddenNodes),
    expression_(expression)
{
}

std::string ForbiddenNodesVisitor::name() const
{
    return "ForbiddenNodesVisitor";
}

void ForbiddenNodesVisitor::visit(const SumNode* sumNode)
{
    visitChildren(sumNode, typeIndexOf<SumNode>());
}

void ForbiddenNodesVisitor::visit(const SubtractionNode* subtractionNode)
{
    visitChildren(subtractionNode, typeIndexOf<SubtractionNode>());
}

void ForbiddenNodesVisitor::visit(const MultiplicationNode* multiplicationNode)
{
    visitChildren(multiplicationNode, typeIndexOf<MultiplicationNode>());
}

void ForbiddenNodesVisitor::visit(const DivisionNode* divisionNode)
{
    visitChildren(divisionNode, typeIndexOf<DivisionNode>());
}

void ForbiddenNodesVisitor::visit(const EqualNode* equalNode)
{
    std::type_index nodeTypeId = typeIndexOf<EqualNode>();
    checkIsForbidden(nodeTypeId, equalNode);
    visitChildren(equalNode, nodeTypeId);
}

void ForbiddenNodesVisitor::visit(const LessThanOrEqualNode* lessThanOrEqualNode)
{
    std::type_index nodeTypeId = typeIndexOf<LessThanOrEqualNode>();
    checkIsForbidden(nodeTypeId, lessThanOrEqualNode);
    visitChildren(lessThanOrEqualNode, nodeTypeId);
}

void ForbiddenNodesVisitor::visit(const GreaterThanOrEqualNode* greaterThanOrEqualNode)
{
    std::type_index nodeTypeId = typeIndexOf<GreaterThanOrEqualNode>();
    checkIsForbidden(nodeTypeId, greaterThanOrEqualNode);
    visitChildren(greaterThanOrEqualNode, nodeTypeId);
}

void ForbiddenNodesVisitor::visit(const NegationNode* negationNode)
{
    dispatch(negationNode->child());
}

void ForbiddenNodesVisitor::visit(const LiteralNode*)
{
    // keep empty
}

void ForbiddenNodesVisitor::visit(const VariableNode* variableNode)
{
    checkIsForbidden(typeIndexOf<VariableNode>(), variableNode);
}

void ForbiddenNodesVisitor::visit(const ParameterNode*)
{
    // keep empty
}

void ForbiddenNodesVisitor::visit(const PortFieldNode* portFieldNode)
{
    checkIsForbidden(typeIndexOf<PortFieldNode>(), portFieldNode);
}

void ForbiddenNodesVisitor::visit(const PortFieldSumNode* portFieldSumNode)
{
    checkIsForbidden(typeIndexOf<PortFieldSumNode>(), portFieldSumNode);
}

void ForbiddenNodesVisitor::visit(const TimeShiftNode* timeShiftNode)
{
    std::type_index nodeTypeId = typeIndexOf<TimeShiftNode>();
    checkIsForbidden(nodeTypeId, timeShiftNode);
    visitChildren(timeShiftNode, nodeTypeId);
}

void ForbiddenNodesVisitor::visit(const TimeIndexNode* timeIndexNode)
{
    std::type_index nodeTypeId = typeIndexOf<TimeIndexNode>();
    checkIsForbidden(nodeTypeId, timeIndexNode);
    visitChildren(timeIndexNode, nodeTypeId);
}

void ForbiddenNodesVisitor::visit(const TimeSumNode* timeSumNode)
{
    std::type_index nodeTypeId = typeIndexOf<TimeSumNode>();
    checkIsForbidden(nodeTypeId, timeSumNode);
    visitChildren(timeSumNode, nodeTypeId);
}

void ForbiddenNodesVisitor::visit(const AllTimeSumNode* allTimeSumNode)
{
    std::type_index nodeTypeId = typeIndexOf<AllTimeSumNode>();
    checkIsForbidden(nodeTypeId, allTimeSumNode);
    visitChildren(allTimeSumNode, nodeTypeId);
}

void ForbiddenNodesVisitor::visit(const FunctionNode* functionNode)
{
    std::type_index nodeTypeId(typeid(int)); // Must be default constructed here

    switch (functionNode->type())
    {
    case FunctionNodeType::reduced_cost:
        nodeTypeId = typeIndexOf<FunctionNodeType::reduced_cost>();
        break;
    case FunctionNodeType::dual:
        nodeTypeId = typeIndexOf<FunctionNodeType::dual>();
        break;
    case FunctionNodeType::min:
        nodeTypeId = typeIndexOf<FunctionNodeType::min>();
        break;
    case FunctionNodeType::max:
        nodeTypeId = typeIndexOf<FunctionNodeType::max>();
        break;
    case FunctionNodeType::pow:
        nodeTypeId = typeIndexOf<FunctionNodeType::pow>();
        break;
    case FunctionNodeType::floor:
        nodeTypeId = typeIndexOf<FunctionNodeType::floor>();
        break;
    default:
        break;
    }

    checkIsForbidden(nodeTypeId, functionNode);
    visitChildren(functionNode, nodeTypeId);
}

void ForbiddenNodesVisitor::checkIsForbidden(const std::type_index& nodeTypeId,
                                             const Node* node) const
{
    checkIGloballyForbidden(nodeTypeId, node);
    checkIsForbiddenByParent(nodeTypeId, node);
}

void ForbiddenNodesVisitor::checkIGloballyForbidden(const std::type_index& nodeTypeId,
                                                    const Node* node) const
{
    if (forbiddenNodes_.isGloballyForbidden(nodeTypeId))
    {
        throw ForbiddenNodeFound(expression_, node->name());
    }
}

void ForbiddenNodesVisitor::checkIsForbiddenByParent(const std::type_index& nodeTypeId,
                                                     const Node* node) const
{
    for (const auto& [parentNodeName, parentTypeIndex]: std::ranges::reverse_view(parentsStack_))
    {
        if (forbiddenNodes_.isForbiddenByParent(parentTypeIndex, nodeTypeId))
        {
            throw ForbiddenNodeFound(expression_, node->name(), parentNodeName);
        }
    }
}

void ForbiddenNodesVisitor::visitChildren(const Expressions::Nodes::ParentNode* node,
                                          const std::type_index& nodeTypeId)
{
    parentsStack_.emplace_back(node->name(), nodeTypeId);
    for (const auto* childNode: node->getOperands())
    {
        dispatch(childNode);
    }
    parentsStack_.pop_back();
}

} // namespace Antares::IO::Inputs::ModelConverter
