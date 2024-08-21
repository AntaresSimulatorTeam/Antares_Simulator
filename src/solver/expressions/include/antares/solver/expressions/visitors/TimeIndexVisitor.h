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

#include "antares/solver/expressions/visitors/NodeVisitor.h"
#include "antares/solver/expressions/visitors/TimeStructure.h"

namespace Antares::Solver::Visitors
{

class TimeIndexVisitor: public Nodes::NodeVisitor<TIME_STRUCTURE_TYPE>
{
public:
    using Base = Nodes::NodeVisitor<TIME_STRUCTURE_TYPE>;
    TimeIndexVisitor(std::unordered_map<const Nodes::Node*, TIME_STRUCTURE_TYPE> context);
    TimeIndexVisitor() = default;

private:
    std::unordered_map<const Nodes::Node*, TIME_STRUCTURE_TYPE> context_;
    TIME_STRUCTURE_TYPE visit(const Nodes::AddNode& add) override;
    TIME_STRUCTURE_TYPE visit(const Nodes::SubtractionNode& add) override;
    TIME_STRUCTURE_TYPE visit(const Nodes::MultiplicationNode& add) override;
    TIME_STRUCTURE_TYPE visit(const Nodes::DivisionNode& add) override;
    TIME_STRUCTURE_TYPE visit(const Nodes::EqualNode& add) override;
    TIME_STRUCTURE_TYPE visit(const Nodes::LessThanOrEqualNode& add) override;
    TIME_STRUCTURE_TYPE visit(const Nodes::GreaterThanOrEqualNode& add) override;
    TIME_STRUCTURE_TYPE visit(const Nodes::NegationNode& neg) override;
    TIME_STRUCTURE_TYPE visit(const Nodes::VariableNode& param) override;
    TIME_STRUCTURE_TYPE visit(const Nodes::ParameterNode& param) override;
    TIME_STRUCTURE_TYPE visit(const Nodes::LiteralNode& lit) override;
    TIME_STRUCTURE_TYPE visit(const Nodes::PortFieldNode& port_field_node) override;
    TIME_STRUCTURE_TYPE visit(const Nodes::ComponentVariableNode& component_variable_node) override;
    TIME_STRUCTURE_TYPE visit(
      const Nodes::ComponentParameterNode& component_parameter_node) override;
};
} // namespace Antares::Solver::Visitors
