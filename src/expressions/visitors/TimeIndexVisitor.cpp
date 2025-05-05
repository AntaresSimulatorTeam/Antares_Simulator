/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
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

using namespace Antares::ModelerStudy::SystemModel;

namespace Antares::Expressions::Visitors
{

TimeIndex TimeIndexVisitor::visit(const Nodes::SumNode* node)
{
    auto operands = node->getOperands();
    return std::accumulate(std::begin(operands),
                           std::end(operands),
                           TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO,
                           [this](TimeIndex sum, Nodes::Node* operand)
                           { return sum | dispatch(operand); });
}

TimeIndex TimeIndexVisitor::visit(const Nodes::SubtractionNode* sub)
{
    return dispatch(sub->left()) | dispatch(sub->right());
}

TimeIndex TimeIndexVisitor::visit(const Nodes::MultiplicationNode* mult)
{
    return dispatch(mult->left()) | dispatch(mult->right());
}

TimeIndex TimeIndexVisitor::visit(const Nodes::DivisionNode* div)
{
    return dispatch(div->left()) | dispatch(div->right());
}

TimeIndex TimeIndexVisitor::visit(const Nodes::EqualNode* equ)
{
    return dispatch(equ->left()) | dispatch(equ->right());
}

TimeIndex TimeIndexVisitor::visit(const Nodes::LessThanOrEqualNode* lt)
{
    return dispatch(lt->left()) | dispatch(lt->right());
}

TimeIndex TimeIndexVisitor::visit(const Nodes::GreaterThanOrEqualNode* gt)
{
    return dispatch(gt->left()) | dispatch(gt->right());
}

TimeIndex TimeIndexVisitor::visit(const Nodes::VariableNode* var)
{
    return var->timeIndex();
}

TimeIndex TimeIndexVisitor::visit(const Nodes::ParameterNode* param)
{
    return param->timeIndex();
}

TimeIndex TimeIndexVisitor::visit([[maybe_unused]] const Nodes::LiteralNode* lit)
{
    return TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO;
}

TimeIndex TimeIndexVisitor::visit(const Nodes::NegationNode* neg)
{
    return dispatch(neg->child());
}

TimeIndex TimeIndexVisitor::visit(const Nodes::PortFieldNode* port_field_node)
{
    throw std::invalid_argument("PortFieldNode not handled by visitor TimeIndexVisitor");
}

TimeIndex TimeIndexVisitor::visit(const Nodes::PortFieldSumNode* node)
{
    std::string portId = node->getPortName();
    std::string fieldId = node->getFieldName();

    TimeIndex to_return = TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO;
    for (const auto connexion_end: component_.componentConnectionsViaPort(portId))
    {
        auto* component = connexion_end.component();
        auto* port = connexion_end.port();

        TimeIndexVisitor visitor(*component);
        const Nodes::Node* node = component->nodeAtPortField(port->Id(), fieldId);
        to_return = to_return | visitor.dispatch(node);
    }
    return to_return;
}

TimeIndex TimeIndexVisitor::visit(const Nodes::ComponentVariableNode* component_variable_node)
{
    throw std::invalid_argument("ComponentVariableNode not handled by visitor TimeIndexVisitor");
}

TimeIndex TimeIndexVisitor::visit(const Nodes::ComponentParameterNode* component_parameter_node)
{
    throw std::invalid_argument("ComponentParameterNode not handled by visitor TimeIndexVisitor");
}

TimeIndex TimeIndexVisitor::visit(const Nodes::TimeShiftNode* timeShiftNode)
{
    return dispatch(timeShiftNode->left());
}

TimeIndex TimeIndexVisitor::visit([[maybe_unused]] const Nodes::TimeIndexNode* timeIndexNode)
{
    return TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO;
}

TimeIndex TimeIndexVisitor::visit(const Nodes::TimeSumNode* timeSumNode)
{
    // TODO  case from = to
    return dispatch(timeSumNode->expression());
}

TimeIndex TimeIndexVisitor::visit([[maybe_unused]] const Nodes::AllTimeSumNode* timeSumNode)
{
    return TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO;
}

TimeIndexVisitor::TimeIndexVisitor(const Component& component):
    component_(component)
{
}

std::string TimeIndexVisitor::name() const
{
    return "TimeIndexVisitor";
}

} // namespace Antares::Expressions::Visitors
