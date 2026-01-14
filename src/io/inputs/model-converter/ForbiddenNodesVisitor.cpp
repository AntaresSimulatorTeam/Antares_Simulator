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
    checkIsForbidden(equalNode, nodeTypeId);
    visitChildren(equalNode, nodeTypeId);
}

void ForbiddenNodesVisitor::visit(const LessThanOrEqualNode* lessThanOrEqualNode)
{
    std::type_index nodeTypeId = typeIndexOf<LessThanOrEqualNode>();
    checkIsForbidden(lessThanOrEqualNode, nodeTypeId);
    visitChildren(lessThanOrEqualNode, nodeTypeId);
}

void ForbiddenNodesVisitor::visit(const GreaterThanOrEqualNode* greaterThanOrEqualNode)
{
    std::type_index nodeTypeId = typeIndexOf<GreaterThanOrEqualNode>();
    checkIsForbidden(greaterThanOrEqualNode, nodeTypeId);
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
    checkIsForbidden(variableNode, typeIndexOf<VariableNode>());
}

void ForbiddenNodesVisitor::visit(const ParameterNode*)
{
    // keep empty
}

void ForbiddenNodesVisitor::visit(const PortFieldNode* portFieldNode)
{
    checkIsForbidden(portFieldNode, typeIndexOf<PortFieldNode>());
}

void ForbiddenNodesVisitor::visit(const PortFieldSumNode* portFieldSumNode)
{
    checkIsForbidden(portFieldSumNode, typeIndexOf<PortFieldSumNode>());
}

void ForbiddenNodesVisitor::visit(const TimeShiftNode* timeShiftNode)
{
    std::type_index nodeTypeId = typeIndexOf<TimeShiftNode>();
    checkIsForbidden(timeShiftNode, nodeTypeId);
    visitChildren(timeShiftNode, nodeTypeId);
}

void ForbiddenNodesVisitor::visit(const TimeIndexNode* timeIndexNode)
{
    std::type_index nodeTypeId = typeIndexOf<TimeIndexNode>();
    checkIsForbidden(timeIndexNode, nodeTypeId);
    visitChildren(timeIndexNode, nodeTypeId);
}

void ForbiddenNodesVisitor::visit(const TimeSumNode* timeSumNode)
{
    std::type_index nodeTypeId = typeIndexOf<TimeSumNode>();
    checkIsForbidden(timeSumNode, nodeTypeId);
    visitChildren(timeSumNode, nodeTypeId);
}

void ForbiddenNodesVisitor::visit(const AllTimeSumNode* allTimeSumNode)
{
    std::type_index nodeTypeId = typeIndexOf<AllTimeSumNode>();
    checkIsForbidden(allTimeSumNode, nodeTypeId);
    visitChildren(allTimeSumNode, nodeTypeId);
}

std::type_index functionNodeTypeIndex(const FunctionNode* functionNode)
{
    switch (functionNode->type())
    {
    case FunctionNodeType::reduced_cost:
        return typeIndexOf<FunctionNodeType::reduced_cost>();
    case FunctionNodeType::dual:
        return typeIndexOf<FunctionNodeType::dual>();
    case FunctionNodeType::min:
        return typeIndexOf<FunctionNodeType::min>();
    case FunctionNodeType::max:
        return typeIndexOf<FunctionNodeType::max>();
    case FunctionNodeType::pow:
        return typeIndexOf<FunctionNodeType::pow>();
    case FunctionNodeType::floor:
        return typeIndexOf<FunctionNodeType::floor>();
    case FunctionNodeType::ceil:
        return typeIndexOf<FunctionNodeType::ceil>();
    default:
        std::string err_msg = "ForbiddenNodesVisitor > ";
        err_msg += "function '" + functionNode->name() + "' is unknown.";
        throw std::invalid_argument(err_msg);
    }
}

void ForbiddenNodesVisitor::visit(const FunctionNode* functionNode)
{
    std::type_index nodeTypeId(functionNodeTypeIndex(functionNode));
    checkIsForbidden(functionNode, nodeTypeId);
    visitChildren(functionNode, nodeTypeId);
}

void ForbiddenNodesVisitor::checkIsForbidden(const Node* node,
                                             const std::type_index& nodeTypeId) const
{
    checkIsGloballyForbidden(nodeTypeId, node);
    checkIsForbiddenByParent(nodeTypeId, node);
}

void ForbiddenNodesVisitor::checkIsGloballyForbidden(const std::type_index& nodeTypeId,
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
    std::ranges::for_each(node->getOperands(), [this](auto* childNode) { dispatch(childNode); });
    parentsStack_.pop_back();
}

} // namespace Antares::IO::Inputs::ModelConverter
