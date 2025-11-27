/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include "antares/expressions/visitors/LinearityVisitor.h"

#include <numeric>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/InvalidNode.h>
#include <antares/expressions/visitors/LinearStatus.h>
#include "antares/expressions/visitors/EvalVisitor.h"
#include "antares/expressions/visitors/PrintVisitor.h"
#include "antares/expressions/visitors/VariabilityVisitor.h"
#include "antares/expressions/visitors/VariadicNodeFunctionVisit.h"

namespace Antares::Expressions::Visitors
{

LinearStatus LinearityVisitor::visit(const Nodes::SumNode* node)
{
    const auto& operands = node->getOperands();
    return std::accumulate(std::begin(operands),
                           std::end(operands),
                           LinearStatus::CONSTANT,
                           [this](LinearStatus sum, Nodes::Node* operand)
                           { return sum + dispatch(operand); });
}

LinearStatus LinearityVisitor::visit(const Nodes::SubtractionNode* node)
{
    return dispatch(node->left()) - dispatch(node->right());
}

LinearStatus LinearityVisitor::visit(const Nodes::MultiplicationNode* node)
{
    return dispatch(node->left()) * dispatch(node->right());
}

LinearStatus LinearityVisitor::visit(const Nodes::DivisionNode* node)
{
    return dispatch(node->left()) / dispatch(node->right());
}

LinearStatus LinearityVisitor::visit(const Nodes::EqualNode* node)
{
    return dispatch(node->left()) + dispatch(node->right());
}

LinearStatus LinearityVisitor::visit(const Nodes::LessThanOrEqualNode* node)
{
    return dispatch(node->left()) + dispatch(node->right());
}

LinearStatus LinearityVisitor::visit(const Nodes::GreaterThanOrEqualNode* node)
{
    return dispatch(node->left()) + dispatch(node->right());
}

LinearStatus LinearityVisitor::visit([[maybe_unused]] const Nodes::VariableNode*)
{
    return LinearStatus::LINEAR;
}

LinearStatus LinearityVisitor::visit([[maybe_unused]] const Nodes::ParameterNode*)
{
    return LinearStatus::CONSTANT;
}

LinearStatus LinearityVisitor::visit([[maybe_unused]] const Nodes::LiteralNode*)
{
    return LinearStatus::CONSTANT;
}

LinearStatus LinearityVisitor::visit(const Nodes::NegationNode* node)
{
    return -dispatch(node->child());
}

LinearStatus LinearityVisitor::visit(const Nodes::PortFieldNode*)
{
    return LinearStatus::CONSTANT;
}

LinearStatus LinearityVisitor::visit(const Nodes::PortFieldSumNode*)
{
    return LinearStatus::CONSTANT;
}

LinearStatus LinearityVisitor::visit(const Nodes::TimeShiftNode* timeShiftNode)
{
    return dispatch(timeShiftNode->left());
}

LinearStatus LinearityVisitor::visit(const Nodes::TimeIndexNode*)
{
    return LinearStatus::CONSTANT;
}

LinearStatus LinearityVisitor::visit(const Nodes::TimeSumNode* timeSumNode)
{
    return dispatch(timeSumNode->expression());
}

LinearStatus LinearityVisitor::visit([[maybe_unused]] const Nodes::AllTimeSumNode* timeSumNode)
{
    return LinearStatus::CONSTANT;
}

LinearStatus LinearityVisitor::handleReducedCost([[maybe_unused]] const Nodes::FunctionNode*)
{
    throw NodeTypeShouldBeInExtraOutput("reduced_cost");
}

LinearStatus LinearityVisitor::handleDual([[maybe_unused]] const Nodes::FunctionNode*)
{
    throw NodeTypeShouldBeInExtraOutput("dual");
}

LinearStatus LinearityVisitor::handlePow(const Nodes::FunctionNode* node)
{
    const auto& operands = node->getOperands();
    if (operands.size() != 2)
    {
        throw std::invalid_argument("Invalid operand count for pow: ");
    }

    LinearStatus base = dispatch(operands[0]);

    // Check if exponent is constant in time and scenario
    VariabilityVisitor variability_visitor(optimEntityContainer_, component_);
    auto exponentTimeIndex = variability_visitor.dispatch(operands[1]);
    if (exponentTimeIndex != Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO)
    {
        PrintVisitor visitor;
        throw std::invalid_argument("exponent must be constant in time and scenario: "
                                    + visitor.dispatch(node));
    }

    // If base is constant, the result is constant
    if (base == LinearStatus::CONSTANT)
    {
        return LinearStatus::CONSTANT;
    }
    EvalVisitor evalVisitor(optimEntityContainer_, fillContext_, component_);
    auto exponentValue = evalVisitor.dispatch(operands[1]).valueAsDouble();
    if (std::abs(exponentValue) < DEFAULT_THRESHOLD)
    {
        return LinearStatus::CONSTANT;
    }
    // If base is linear and exponent is 1 the result is linear
    if (base == LinearStatus::LINEAR && std::abs(exponentValue - 1) < DEFAULT_THRESHOLD)
    {
        return LinearStatus::LINEAR;
    }

    return LinearStatus::NON_LINEAR;
}

LinearStatus LinearityVisitor::visit(const Nodes::FunctionNode* node)
{
    switch (node->type())
    {
    case Nodes::FunctionNodeType::reduced_cost:
        return handleReducedCost(node);
    case Nodes::FunctionNodeType::dual:
        return handleDual(node);
    case Nodes::FunctionNodeType::max:
    case Nodes::FunctionNodeType::min:
        return applyOperation(variadicFunction(*this, node),
                              [](const auto& elements)
                              { return *std::max_element(elements.begin(), elements.end()); });

    case Nodes::FunctionNodeType::pow:
        return handlePow(node);
    default:
        // TODO exception ?
        return LinearStatus::CONSTANT;
    }
}

std::string LinearityVisitor::name() const
{
    return "LinearityVisitor";
}

LinearityVisitor::LinearityVisitor(const Optimisation::OptimEntityContainer& optimEntityContainer,
                                   const Optimisation::LinearProblemApi::FillContext& fillContext,
                                   const ModelerStudy::SystemModel::Component& component):
    optimEntityContainer_(optimEntityContainer),
    fillContext_(fillContext),
    component_(component)
{
}
} // namespace Antares::Expressions::Visitors
