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
#include "include/antares/io/inputs/model-converter/NodeChecker.h"

#include <antares/expressions/nodes/ExpressionsNodes.h>
using namespace Antares::Expressions::Nodes;

namespace Antares::IO::Inputs::ModelConverter
{

std::string ErrorMessage(const std::string expr, const std::string node, const std::string parent)
{
    if (!parent.empty())
    {
        std::string format_str = "'{}' is not allowed to contain '{}' in this context '{}'";
        return fmt::format(fmt::runtime(format_str), parent, node, expr);
    }
    return fmt::format("'{}' is not allowed in this context '{}'", node, expr);
}

ForbiddenNodeFound::ForbiddenNodeFound(const std::string expr,
                                       const std::string node,
                                       const std::string parent):
    invalid_argument(ErrorMessage(expr, node, parent))
{
}

NodeChecker::NodeChecker(const ForbiddenNodes& forbiddenNodes, const std::string& expression):
    forbiddenNodes_(forbiddenNodes),
    expression_(expression)
{
}

std::string NodeChecker::name() const
{
    return "NodeChecker";
}

void NodeChecker::visit(const SumNode* sumNode)
{
    visitChildren("sum", sumNode, typeIndexOf<SumNode>());
}

void NodeChecker::visit(const SubtractionNode* subtractionNode)
{
    visitChildren("subtraction", subtractionNode, typeIndexOf<SubtractionNode>());
}

void NodeChecker::visit(const MultiplicationNode* multiplicationNode)
{
    visitChildren("multiplication", multiplicationNode, typeIndexOf<MultiplicationNode>());
}

void NodeChecker::visit(const DivisionNode* divisionNode)
{
    visitChildren("division", divisionNode, typeIndexOf<DivisionNode>());
}

void NodeChecker::visit(const EqualNode* equalNode)
{
    std::string nodeName = "expression with =";
    std::type_index nodeTypeId = typeIndexOf<EqualNode>();
    checkIsForbidden(nodeTypeId, nodeName);
    visitChildren(nodeName, equalNode, nodeTypeId);
}

void NodeChecker::visit(const LessThanOrEqualNode* lessThanOrEqualNode)
{
    std::string nodeName = "expression with <=";
    std::type_index nodeTypeId = typeIndexOf<LessThanOrEqualNode>();
    checkIsForbidden(nodeTypeId, nodeName);
    visitChildren(nodeName, lessThanOrEqualNode, nodeTypeId);
}

void NodeChecker::visit(const GreaterThanOrEqualNode* greaterThanOrEqualNode)
{
    std::string nodeName = "expression with >=";
    std::type_index nodeTypeId = typeIndexOf<GreaterThanOrEqualNode>();
    checkIsForbidden(nodeTypeId, nodeName);
    visitChildren(nodeName, greaterThanOrEqualNode, nodeTypeId);
}

void NodeChecker::visit(const NegationNode* negationNode)
{
    dispatch(negationNode->child());
}

void NodeChecker::visit(const LiteralNode*)
{
    // keep empty
}

void NodeChecker::visit(const VariableNode* variableNode)
{
    std::string nodeName = "variable(" + variableNode->value() + ")";
    checkIsForbidden(typeIndexOf<VariableNode>(), nodeName);
}

void NodeChecker::visit(const ParameterNode*)
{
    // keep empty
}

void NodeChecker::visit(const PortFieldNode* portFieldNode)
{
    std::string nodeName = "port field (" + portFieldNode->getPortName() + "."
                           + portFieldNode->getFieldName() + ")";
    checkIsForbidden(typeIndexOf<PortFieldNode>(), nodeName);
}

void NodeChecker::visit(const PortFieldSumNode*)
{
    checkIsForbidden(typeIndexOf<PortFieldSumNode>(), "sum_connections");
}

void NodeChecker::visit(const TimeShiftNode* timeShiftNode)
{
    std::string nodeName = "timeShift";
    std::type_index nodeTypeId = typeIndexOf<TimeShiftNode>();
    checkIsForbidden(nodeTypeId, nodeName);
    visitChildren(nodeName, timeShiftNode, nodeTypeId);
}

void NodeChecker::visit(const TimeIndexNode* timeIndexNode)
{
    std::string nodeName = "timeIndex";
    std::type_index nodeTypeId = typeIndexOf<TimeIndexNode>();
    checkIsForbidden(nodeTypeId, nodeName);
    visitChildren(nodeName, timeIndexNode, nodeTypeId);
}

void NodeChecker::visit(const TimeSumNode* timeSumNode)
{
    std::string nodeName = "sum";
    std::type_index nodeTypeId = typeIndexOf<TimeSumNode>();
    checkIsForbidden(nodeTypeId, nodeName);
    visitChildren(nodeName, timeSumNode, nodeTypeId);
}

void NodeChecker::visit(const AllTimeSumNode* allTimeSumNode)
{
    std::string nodeName = "sum";
    std::type_index nodeTypeId = typeIndexOf<AllTimeSumNode>();
    checkIsForbidden(nodeTypeId, nodeName);
    visitChildren(nodeName, allTimeSumNode, nodeTypeId);
}

void NodeChecker::visit(const FunctionNode* functionNode)
{
    std::type_index nodeTypeId(typeid(int) /* has to be default constructed here */);

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

    const std::string nodeName = functionNode->typeToString();

    checkIsForbidden(nodeTypeId, nodeName);
    visitChildren(nodeName, functionNode, nodeTypeId);
}

void NodeChecker::checkIsForbidden(const std::type_index& nodeTypeId,
                                   const std::string& nodeName) const
{
    if (forbiddenNodes_.isGloballyForbidden(nodeTypeId))
    {
        throw ForbiddenNodeFound(expression_, nodeName);
    }

    for (const auto& [parentNodeName, parentTypeIndex]: std::ranges::reverse_view(parentsStack_))
    {
        if (forbiddenNodes_.isForbiddenByParent(parentTypeIndex, nodeTypeId))
        {
            throw ForbiddenNodeFound(expression_, nodeName, parentNodeName);
        }
    }
}

void NodeChecker::visitChildren(const std::string& nodeName,
                                const Expressions::Nodes::ParentNode* node,
                                const std::type_index& nodeTypeId)
{
    parentsStack_.emplace_back(nodeName, nodeTypeId);
    for (const auto* child: node->getOperands())
    {
        dispatch(child);
    }
    parentsStack_.pop_back();
}

} // namespace Antares::IO::Inputs::ModelConverter
