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
#include <antares/expressions/visitors/TimeIndexVisitor.h>
#include "antares/expressions/visitors/PrintVisitor.h"

using namespace Antares::ModelerStudy::SystemModel;

namespace Antares::Expressions::Visitors
{

Optimisation::TimeIndex TimeIndexVisitor::processParentNode(const Nodes::ParentNode* node)
{
    const auto& operands = node->getOperands();
    return std::accumulate(std::begin(operands),
                           std::end(operands),
                           Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO,
                           [this](Optimisation::TimeIndex sum, Nodes::Node* operand)
                           { return sum | dispatch(operand); });
}

Optimisation::TimeIndex TimeIndexVisitor::visit(const Nodes::SumNode* node)
{
    return processParentNode(node);
}

Optimisation::TimeIndex TimeIndexVisitor::visit(const Nodes::SubtractionNode* sub)
{
    return dispatch(sub->left()) | dispatch(sub->right());
}

Optimisation::TimeIndex TimeIndexVisitor::visit(const Nodes::MultiplicationNode* mult)
{
    return dispatch(mult->left()) | dispatch(mult->right());
}

Optimisation::TimeIndex TimeIndexVisitor::visit(const Nodes::DivisionNode* div)
{
    return dispatch(div->left()) | dispatch(div->right());
}

Optimisation::TimeIndex TimeIndexVisitor::visit(const Nodes::EqualNode* equ)
{
    return dispatch(equ->left()) | dispatch(equ->right());
}

Optimisation::TimeIndex TimeIndexVisitor::visit(const Nodes::LessThanOrEqualNode* lt)
{
    return dispatch(lt->left()) | dispatch(lt->right());
}

Optimisation::TimeIndex TimeIndexVisitor::visit(const Nodes::GreaterThanOrEqualNode* gt)
{
    return dispatch(gt->left()) | dispatch(gt->right());
}

Optimisation::TimeIndex TimeIndexVisitor::visit(const Nodes::VariableNode* var)
{
    return var->timeIndex();
}

Optimisation::TimeIndex TimeIndexVisitor::visit(const Nodes::ParameterNode* param)
{
    const auto systemParameter = context_.getParameter(param->value());
    if (systemParameter.type == ParameterType::CONSTANT)
    {
        return Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO;
        // TODO: handle more cases, but ParameterType must be exhaustive first
    }
    return param->timeIndex();
}

Optimisation::TimeIndex TimeIndexVisitor::visit([[maybe_unused]] const Nodes::LiteralNode* lit)
{
    return Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO;
}

Optimisation::TimeIndex TimeIndexVisitor::visit(const Nodes::NegationNode* neg)
{
    return dispatch(neg->child());
}

Optimisation::TimeIndex TimeIndexVisitor::visit(const Nodes::PortFieldNode* node)
{
    std::string portId = node->getPortName();
    std::string fieldId = node->getFieldName();

    const auto* nodeToVisit = component_.nodeAtPortField(portId, fieldId);
    return dispatch(nodeToVisit);
}

Optimisation::TimeIndex TimeIndexVisitor::visit(const Nodes::PortFieldSumNode* node)
{
    std::string portId = node->getPortName();
    std::string fieldId = node->getFieldName();

    auto to_return = Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO;
    for (const auto connexion_end: component_.componentConnectionsViaPort(portId))
    {
        auto* component = connexion_end.component();
        auto* port = connexion_end.port();

        TimeIndexVisitor visitor(optimEntityContainer_, *component);
        const Nodes::Node* node = component->nodeAtPortField(port->Id(), fieldId);
        to_return = to_return | visitor.dispatch(node);
    }
    return to_return;
}

Optimisation::TimeIndex TimeIndexVisitor::visit(const Nodes::TimeShiftNode* timeShiftNode)
{
    return dispatch(timeShiftNode->left());
}

Optimisation::TimeIndex TimeIndexVisitor::visit(
  [[maybe_unused]] const Nodes::TimeIndexNode* timeIndexNode)
{
    return Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO;
}

Optimisation::TimeIndex TimeIndexVisitor::visit(const Nodes::TimeSumNode* timeSumNode)
{
    // TODO  case from = to
    return dispatch(timeSumNode->expression());
}

Optimisation::TimeIndex TimeIndexVisitor::visit(
  [[maybe_unused]] const Nodes::AllTimeSumNode* timeSumNode)
{
    return Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO;
}

Optimisation::TimeIndex TimeIndexVisitor::handleReducedCost(const Nodes::FunctionNode* node)
{
    const auto varNode = dynamic_cast<Nodes::VariableNode*>(node->getOperands().at(0));
    return varNode->timeIndex();
}

Optimisation::TimeIndex TimeIndexVisitor::handleDual(const Nodes::FunctionNode* node)
{
    const auto indexNode = dynamic_cast<Nodes::LiteralNode*>(node->getOperands().at(1));
    unsigned int cstrIndex = static_cast<unsigned int>(indexNode->value());
    const auto& [_, timeIndex] = optimEntityContainer_.getConstraintData(component_, cstrIndex);
    return timeIndex;
}

Optimisation::TimeIndex TimeIndexVisitor::handlePow(const Nodes::FunctionNode* node)
{
    if (const auto* exponent = node->getOperands().at(1);
        dispatch(exponent) != Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO)
    {
        PrintVisitor visitor;
        throw std::runtime_error("This exponent must be constant in :" + visitor.dispatch(node));
    }
    return dispatch(node->getOperands().at(0));
}

Optimisation::TimeIndex TimeIndexVisitor::visit(const Nodes::FunctionNode* node)
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
        return Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO;
    }
}

TimeIndexVisitor::TimeIndexVisitor(const Optimisation::OptimEntityContainer& optimEntityContainer,
                                   const ModelerStudy::SystemModel::Component& component):
    optimEntityContainer_(optimEntityContainer),
    component_(component),
    context_(optimEntityContainer.getEvaluationContext(component))
{
}

std::string TimeIndexVisitor::name() const
{
    return "TimeIndexVisitor";
}

} // namespace Antares::Expressions::Visitors
