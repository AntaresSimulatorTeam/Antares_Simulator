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
#pragma once

#include "antares/expressions/visitors/LinearStatus.h"
#include "antares/expressions/visitors/NodeVisitor.h"

namespace Antares::Expressions::Visitors
{
/**
 * @brief Represents a visitor for determining the linearity of nodes (expression).
 */
class LinearityVisitor: public NodeVisitor<LinearStatus>
{
public:
    std::string name() const override;

private:
    LinearStatus visit(const Nodes::SumNode* add) override;
    LinearStatus visit(const Nodes::SubtractionNode* add) override;
    LinearStatus visit(const Nodes::MultiplicationNode* add) override;
    LinearStatus visit(const Nodes::DivisionNode* add) override;
    LinearStatus visit(const Nodes::EqualNode* add) override;
    LinearStatus visit(const Nodes::LessThanOrEqualNode* add) override;
    LinearStatus visit(const Nodes::GreaterThanOrEqualNode* add) override;
    LinearStatus visit(const Nodes::NegationNode* neg) override;
    LinearStatus visit(const Nodes::VariableNode* param) override;
    LinearStatus visit(const Nodes::ParameterNode* param) override;
    LinearStatus visit(const Nodes::LiteralNode* lit) override;
    LinearStatus visit(const Nodes::PortFieldNode* port_field_node) override;
    LinearStatus visit(const Nodes::PortFieldSumNode* port_field_node) override;
    LinearStatus visit(const Nodes::ComponentVariableNode* component_variable_node) override;
    LinearStatus visit(const Nodes::ComponentParameterNode* component_parameter_node) override;
};
} // namespace Antares::Expressions::Visitors
