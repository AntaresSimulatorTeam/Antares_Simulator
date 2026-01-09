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
    visitChildren<SumNode>("sum", sumNode->getOperands());
}

void NodeChecker::visit(const SubtractionNode* subtractionNode)
{
    visitChildren<SubtractionNode>("subtraction", subtractionNode->getOperands());
}

void NodeChecker::visit(const MultiplicationNode* multiplicationNode)
{
    visitChildren<MultiplicationNode>("multiplication", multiplicationNode->getOperands());
}

void NodeChecker::visit(const DivisionNode* divisionNode)
{
    visitChildren<DivisionNode>("division", divisionNode->getOperands());
}

void NodeChecker::visit(const EqualNode* equalNode)
{
    std::string nodeName = "expression with =";
    checkIsForbidden(typeIndexOf<EqualNode>(), nodeName);
    visitChildren<EqualNode>(nodeName, equalNode->getOperands());
}

void NodeChecker::visit(const LessThanOrEqualNode* lessThanOrEqualNode)
{
    std::string nodeName = "expression with <=";
    checkIsForbidden(typeIndexOf<LessThanOrEqualNode>(), nodeName);
    visitChildren<LessThanOrEqualNode>(nodeName, lessThanOrEqualNode->getOperands());
}

void NodeChecker::visit(const GreaterThanOrEqualNode* greaterThanOrEqualNode)
{
    std::string nodeName = "expression with >=";
    checkIsForbidden(typeIndexOf<GreaterThanOrEqualNode>(), nodeName);
    visitChildren<GreaterThanOrEqualNode>(nodeName, greaterThanOrEqualNode->getOperands());
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
    checkIsForbidden(typeIndexOf<PortFieldNode>(),nodeName);
}

void NodeChecker::visit(const PortFieldSumNode*)
{
    checkIsForbidden(typeIndexOf<PortFieldSumNode>(), "sum_connections");
}

void NodeChecker::visit(const TimeShiftNode* timeShiftNode)
{
    std::string nodeName = "timeShift";
    checkIsForbidden(typeIndexOf<TimeShiftNode>(), nodeName);
    visitChildren<TimeShiftNode>(nodeName, timeShiftNode->getOperands());
}

void NodeChecker::visit(const TimeIndexNode* timeIndexNode)
{
    std::string nodeName = "timeIndex";
    checkIsForbidden(typeIndexOf<TimeIndexNode>(), nodeName);
    visitChildren<TimeIndexNode>(nodeName, timeIndexNode->getOperands());
}

void NodeChecker::visit(const TimeSumNode* timeSumNode)
{
    std::string nodeName = "sum";
    std::type_index typeId = typeIndexOf<TimeSumNode>();
    checkIsForbidden(typeId, nodeName);
    visitChildren<TimeSumNode>(nodeName, timeSumNode->getOperands());
}

void NodeChecker::visit(const AllTimeSumNode* allTimeSumNode)
{
    std::string nodeName = "sum";
    std::type_index typeId = typeIndexOf<AllTimeSumNode>();
    checkIsForbidden(typeId, nodeName);
    visitChildren<AllTimeSumNode>(nodeName, allTimeSumNode->getOperands());
}

void NodeChecker::visit(const FunctionNode* functionNode)
{
    const std::string nodeName = functionNode->typeToString();
    const auto& operands = functionNode->getOperands();
    const FunctionNodeType type = functionNode->type();
    std::type_index typeId(typeid(int) /* has to be default constructed here */);

    switch (type)
    {
    case FunctionNodeType::reduced_cost:
        typeId = typeIndexOf<FunctionNodeType::reduced_cost>();
        checkIsForbidden(typeId, nodeName);
        visitChildren<FunctionNodeType::reduced_cost>(nodeName, operands);
        break;
    case FunctionNodeType::dual:
        typeId = typeIndexOf<FunctionNodeType::dual>();
        checkIsForbidden(typeId, nodeName);
        visitChildren<FunctionNodeType::dual>(nodeName, operands);
        break;
    case FunctionNodeType::min:
        typeId = typeIndexOf<FunctionNodeType::min>();
        checkIsForbidden(typeId, nodeName);
        visitChildren<FunctionNodeType::min>(nodeName, operands);
        break;
    case FunctionNodeType::max:
        typeId = typeIndexOf<FunctionNodeType::max>();
        checkIsForbidden(typeId, nodeName);
        visitChildren<FunctionNodeType::max>(nodeName, operands);
        break;
    case FunctionNodeType::pow:
        typeId = typeIndexOf<FunctionNodeType::pow>();
        checkIsForbidden(typeId, nodeName);
        visitChildren<FunctionNodeType::pow>(nodeName, operands);
        break;
    case FunctionNodeType::floor:
        typeId = typeIndexOf<FunctionNodeType::floor>();
        checkIsForbidden(typeId, nodeName);
        visitChildren<FunctionNodeType::floor>(nodeName, operands);
        break;
    default:
        break;
    }
}

void NodeChecker::checkIsForbidden(const std::type_index& typeId, const std::string& nodeName) const
{
    if (forbiddenNodes_.isGloballyForbidden(typeId))
    {
        throw ForbiddenNodeFound(expression_, nodeName);
    }

    for (const auto& [parentNodeName, parentTypeIndex]: std::ranges::reverse_view(parentsStack_))
    {
        if (forbiddenNodes_.isForbiddenByParent(parentTypeIndex, typeId))
        {
            throw ForbiddenNodeFound(expression_, nodeName, parentNodeName);
        }
    }
}

} // namespace Antares::IO::Inputs::ModelConverter
