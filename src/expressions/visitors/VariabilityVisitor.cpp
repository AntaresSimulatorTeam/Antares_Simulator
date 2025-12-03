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

#include <numeric>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/VariabilityVisitor.h>
#include "antares/expressions/visitors/PrintVisitor.h"

using namespace Antares::ModelerStudy::SystemModel;

namespace Antares::Expressions::Visitors
{

Optimisation::VariabilityType VariabilityVisitor::processParentNode(const Nodes::ParentNode* node)
{
    const auto& operands = node->getOperands();
    return std::accumulate(std::begin(operands),
                           std::end(operands),
                           Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO,
                           [this](Optimisation::VariabilityType sum, Nodes::Node* operand)
                           { return sum | dispatch(operand); });
}

Optimisation::VariabilityType VariabilityVisitor::visit(const Nodes::SumNode* node)
{
    return processParentNode(node);
}

Optimisation::VariabilityType VariabilityVisitor::visit(const Nodes::SubtractionNode* sub)
{
    return dispatch(sub->left()) | dispatch(sub->right());
}

Optimisation::VariabilityType VariabilityVisitor::visit(const Nodes::MultiplicationNode* mult)
{
    return dispatch(mult->left()) | dispatch(mult->right());
}

Optimisation::VariabilityType VariabilityVisitor::visit(const Nodes::DivisionNode* div)
{
    return dispatch(div->left()) | dispatch(div->right());
}

Optimisation::VariabilityType VariabilityVisitor::visit(const Nodes::EqualNode* equ)
{
    return dispatch(equ->left()) | dispatch(equ->right());
}

Optimisation::VariabilityType VariabilityVisitor::visit(const Nodes::LessThanOrEqualNode* lt)
{
    return dispatch(lt->left()) | dispatch(lt->right());
}

Optimisation::VariabilityType VariabilityVisitor::visit(const Nodes::GreaterThanOrEqualNode* gt)
{
    return dispatch(gt->left()) | dispatch(gt->right());
}

Optimisation::VariabilityType VariabilityVisitor::visit(const Nodes::VariableNode* var)
{
    return var->variability();
}

Optimisation::VariabilityType VariabilityVisitor::visit(const Nodes::ParameterNode* param)
{
    const auto systemParameter = context_.getParameter(param->value());
    if (systemParameter.type == ParameterType::CONSTANT)
    {
        return Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO;
        // TODO: handle more cases, but ParameterType must be exhaustive first
    }
    return param->variability();
}

Optimisation::VariabilityType VariabilityVisitor::visit(
  [[maybe_unused]] const Nodes::LiteralNode* lit)
{
    return Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO;
}

Optimisation::VariabilityType VariabilityVisitor::visit(const Nodes::NegationNode* neg)
{
    return dispatch(neg->child());
}

Optimisation::VariabilityType VariabilityVisitor::visit(const Nodes::PortFieldNode* node)
{
    std::string portId = node->getPortName();
    std::string fieldId = node->getFieldName();

    const auto* nodeToVisit = component_.nodeAtPortField(portId, fieldId);
    return dispatch(nodeToVisit);
}

Optimisation::VariabilityType VariabilityVisitor::visit(const Nodes::PortFieldSumNode* node)
{
    std::string portId = node->getPortName();
    std::string fieldId = node->getFieldName();

    auto to_return = Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO;
    for (const auto connexion_end: component_.componentConnectionsViaPort(portId))
    {
        auto* component = connexion_end.component();
        auto* port = connexion_end.port();

        VariabilityVisitor visitor(optimEntityContainer_, *component);
        const Nodes::Node* node = component->nodeAtPortField(port->Id(), fieldId);
        to_return = to_return | visitor.dispatch(node);
    }
    return to_return;
}

Optimisation::VariabilityType VariabilityVisitor::visit(const Nodes::TimeShiftNode* timeShiftNode)
{
    return dispatch(timeShiftNode->left());
}

Optimisation::VariabilityType VariabilityVisitor::visit(
  [[maybe_unused]] const Nodes::TimeIndexNode* timeIndexNode)
{
    return Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO;
}

Optimisation::VariabilityType VariabilityVisitor::visit(const Nodes::TimeSumNode* timeSumNode)
{
    // TODO  case from = to
    return dispatch(timeSumNode->expression());
}

Optimisation::VariabilityType VariabilityVisitor::visit(
  [[maybe_unused]] const Nodes::AllTimeSumNode* timeSumNode)
{
    return Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO;
}

Optimisation::VariabilityType VariabilityVisitor::handleReducedCost(const Nodes::FunctionNode* node)
{
    const auto varNode = dynamic_cast<Nodes::VariableNode*>(node->getOperands().at(0));
    return varNode->variability();
}

Optimisation::VariabilityType VariabilityVisitor::handleDual(const Nodes::FunctionNode* node)
{
    const auto indexNode = dynamic_cast<Nodes::LiteralNode*>(node->getOperands().at(1));
    unsigned int cstrIndex = static_cast<unsigned int>(indexNode->value());
    const auto& [_, timeIndex] = optimEntityContainer_.getConstraintData(component_, cstrIndex);
    return timeIndex;
}

Optimisation::VariabilityType VariabilityVisitor::handlePow(const Nodes::FunctionNode* node)
{
    if (const auto* exponent = node->getOperands().at(1);
        dispatch(exponent) != Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO)
    {
        PrintVisitor visitor;
        throw std::runtime_error("This exponent must be constant in :" + visitor.dispatch(node));
    }
    return dispatch(node->getOperands().at(0));
}

Optimisation::VariabilityType VariabilityVisitor::visit(const Nodes::FunctionNode* node)
{
    switch (node->type())
    {
    case Nodes::FunctionNodeType::reduced_cost:
        return handleReducedCost(node);
    case Nodes::FunctionNodeType::dual:
        return handleDual(node);
    case Nodes::FunctionNodeType::max:
    case Nodes::FunctionNodeType::min:
        return processParentNode(node);
    case Nodes::FunctionNodeType::pow:
        return handlePow(node);
    default:
        // TODO
        return Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO;
    }
}

VariabilityVisitor::VariabilityVisitor(
  const Optimisation::OptimEntityContainer& optimEntityContainer,
  const ModelerStudy::SystemModel::Component& component):
    optimEntityContainer_(optimEntityContainer),
    component_(component),
    context_(optimEntityContainer.getEvaluationContext(component))
{
}

std::string VariabilityVisitor::name() const
{
    return "TimeIndexVisitor";
}

} // namespace Antares::Expressions::Visitors
