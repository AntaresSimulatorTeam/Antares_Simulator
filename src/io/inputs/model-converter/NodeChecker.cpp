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
NodeChecker::NodeChecker(const ForbiddenNodes& forbid, const std::string& expression):
    forbid_(forbid),
    expression_(expression)
{
}

std::string NodeChecker::name() const
{
    return "NodeChecker";
}

void NodeChecker::visit(const SumNode* sumNode)
{
    checkChildren<SumNode>("sum", sumNode->getOperands(), false);
}

void NodeChecker::visit(const SubtractionNode* subtractionNode)
{
    checkChildren<SubtractionNode>("subtraction", subtractionNode->getOperands(), false);
}

void NodeChecker::visit(const MultiplicationNode* multiplicationNode)
{
    checkChildren<MultiplicationNode>("multiplication", multiplicationNode->getOperands(), false);
}

void NodeChecker::visit(const DivisionNode* divisionNode)
{
    checkChildren<DivisionNode>("division", divisionNode->getOperands(), false);
}

void NodeChecker::visit(const EqualNode* equalNode)
{
    handleComparisonNode<EqualNode>("=", equalNode->getOperands());
}

void NodeChecker::visit(const LessThanOrEqualNode* lessThanOrEqualNode)
{
    handleComparisonNode<LessThanOrEqualNode>("<=", lessThanOrEqualNode->getOperands());
}

void NodeChecker::visit(const GreaterThanOrEqualNode* greaterThanOrEqualNode)
{
    handleComparisonNode<GreaterThanOrEqualNode>(">=", greaterThanOrEqualNode->getOperands());
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
    checkChildren<TimeShiftNode>("timeShift", timeShiftNode->getOperands(), true);
}

void NodeChecker::visit(const TimeIndexNode* timeIndexNode)
{
    checkChildren<TimeIndexNode>("timeIndex", timeIndexNode->getOperands(), true);
}

void NodeChecker::visit(const TimeSumNode* timeSumNode)
{
    checkChildren<TimeSumNode>("sum", timeSumNode->getOperands(), true);
}

void NodeChecker::visit(const AllTimeSumNode* allTimeSumNode)
{
    checkChildren<AllTimeSumNode>("sum", allTimeSumNode->getOperands(), true);
}

void NodeChecker::visit(const FunctionNode* functionNode)
{
    switch (functionNode->type())
    {
    case FunctionNodeType::reduced_cost:
        checkChildren<FunctionNodeType::reduced_cost>(functionNode->typeToString(),
                                                      functionNode->getOperands(),
                                                      true);
        break;
    case FunctionNodeType::dual:
        checkChildren<FunctionNodeType::dual>(functionNode->typeToString(),
                                              functionNode->getOperands(),
                                              true);
        break;

    case FunctionNodeType::min:

        checkChildren<FunctionNodeType::min>(functionNode->typeToString(),
                                             functionNode->getOperands(),
                                             true);
        break;
    case FunctionNodeType::max:

        checkChildren<FunctionNodeType::max>(functionNode->typeToString(),
                                             functionNode->getOperands(),
                                             true);
        break;
    case FunctionNodeType::pow:

        checkChildren<FunctionNodeType::pow>(functionNode->typeToString(),
                                             functionNode->getOperands(),
                                             true);
        break;
    default:
        break;
    }
}
} // namespace Antares::IO::Inputs::ModelConverter
