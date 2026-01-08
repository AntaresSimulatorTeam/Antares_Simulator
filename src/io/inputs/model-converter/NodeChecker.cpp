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
    visitChildren<SumNode>("sum", sumNode->getOperands(), false);
}

void NodeChecker::visit(const SubtractionNode* subtractionNode)
{
    visitChildren<SubtractionNode>("subtraction", subtractionNode->getOperands(), false);
}

void NodeChecker::visit(const MultiplicationNode* multiplicationNode)
{
    visitChildren<MultiplicationNode>("multiplication", multiplicationNode->getOperands(), false);
}

void NodeChecker::visit(const DivisionNode* divisionNode)
{
    visitChildren<DivisionNode>("division", divisionNode->getOperands(), false);
}

void NodeChecker::visit(const EqualNode* equalNode)
{
    visitComparisonNode<EqualNode>("=", equalNode->getOperands());
}

void NodeChecker::visit(const LessThanOrEqualNode* lessThanOrEqualNode)
{
    visitComparisonNode<LessThanOrEqualNode>("<=", lessThanOrEqualNode->getOperands());
}

void NodeChecker::visit(const GreaterThanOrEqualNode* greaterThanOrEqualNode)
{
    visitComparisonNode<GreaterThanOrEqualNode>(">=", greaterThanOrEqualNode->getOperands());
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
    checkConsistencyWithParents<VariableNode>("variable(" + variableNode->value() + ")");
}

void NodeChecker::visit(const ParameterNode*)
{
    // keep empty
}

void NodeChecker::visit(const PortFieldNode* portFieldNode)
{
    checkConsistencyWithParents<PortFieldNode>("port field (" + portFieldNode->getPortName() + "."
                                               + portFieldNode->getFieldName() + ")");
}

void NodeChecker::visit(const PortFieldSumNode*)
{
    checkConsistencyWithParents<PortFieldSumNode>("sum_connections");
}

void NodeChecker::visit(const TimeShiftNode* timeShiftNode)
{
    visitChildren<TimeShiftNode>("timeShift", timeShiftNode->getOperands(), true);
}

void NodeChecker::visit(const TimeIndexNode* timeIndexNode)
{
    visitChildren<TimeIndexNode>("timeIndex", timeIndexNode->getOperands(), true);
}

void NodeChecker::visit(const TimeSumNode* timeSumNode)
{
    visitChildren<TimeSumNode>("sum", timeSumNode->getOperands(), true);
}

void NodeChecker::visit(const AllTimeSumNode* allTimeSumNode)
{
    visitChildren<AllTimeSumNode>("sum", allTimeSumNode->getOperands(), true);
}

void NodeChecker::visit(const FunctionNode* functionNode)
{
    switch (functionNode->type())
    {
    case FunctionNodeType::reduced_cost:
        visitChildren<FunctionNodeType::reduced_cost>(functionNode->typeToString(),
                                                      functionNode->getOperands(),
                                                      true);
        break;
    case FunctionNodeType::dual:
        visitChildren<FunctionNodeType::dual>(functionNode->typeToString(),
                                              functionNode->getOperands(),
                                              true);
        break;

    case FunctionNodeType::min:
        visitChildren<FunctionNodeType::min>(functionNode->typeToString(),
                                             functionNode->getOperands(),
                                             true);
        break;
    case FunctionNodeType::max:
        visitChildren<FunctionNodeType::max>(functionNode->typeToString(),
                                             functionNode->getOperands(),
                                             true);
        break;
    case FunctionNodeType::pow:
        visitChildren<FunctionNodeType::pow>(functionNode->typeToString(),
                                             functionNode->getOperands(),
                                             true);
        break;
    default:
        break;
    }
}
} // namespace Antares::IO::Inputs::ModelConverter
