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

#include "antares/solver/expressions/nodes/TwoNodesVisitor.h"
#include "antares/solver/expressions/visitors/NodesComparisonResult.h"

namespace Antares::Solver::Visitors
{
class EqualityVisitor: public Nodes::TwoNodesVisitor<NodeComparisonResult>
{
public:
    using Base = Nodes::TwoNodesVisitor<NodeComparisonResult>;

private:
    NodeComparisonResult visit(const Nodes::AddNode& add1, const Nodes::AddNode& add2) override;
    NodeComparisonResult visit(const Nodes::SubtractionNode& add1,
                               const Nodes::SubtractionNode& add2) override;
    NodeComparisonResult visit(const Nodes::MultiplicationNode& add1,
                               const Nodes::MultiplicationNode& add2) override;
    NodeComparisonResult visit(const Nodes::DivisionNode& add1,
                               const Nodes::DivisionNode& add2) override;
    NodeComparisonResult visit(const Nodes::EqualNode& add1, const Nodes::EqualNode& add2) override;
    NodeComparisonResult visit(const Nodes::LessThanOrEqualNode& add1,
                               const Nodes::LessThanOrEqualNode& add2) override;
    NodeComparisonResult visit(const Nodes::GreaterThanOrEqualNode& add1,
                               const Nodes::GreaterThanOrEqualNode& add2) override;
    NodeComparisonResult visit(const Nodes::NegationNode& neg1,
                               const Nodes::NegationNode& neg2) override;
    NodeComparisonResult visit(const Nodes::VariableNode& param1,
                               const Nodes::VariableNode& param2) override;
    NodeComparisonResult visit(const Nodes::ParameterNode& param1,
                               const Nodes::ParameterNode& param2) override;
    NodeComparisonResult visit(const Nodes::LiteralNode& lit1,
                               const Nodes::LiteralNode& lit2) override;
    NodeComparisonResult visit(const Nodes::PortFieldNode& port_field_node1,
                               const Nodes::PortFieldNode& port_field_node2) override;
    NodeComparisonResult visit(
      const Nodes::ComponentVariableNode& component_variable_node1,
      const Nodes::ComponentVariableNode& component_variable_node2) override;
    NodeComparisonResult visit(
      const Nodes::ComponentParameterNode& component_parameter_node1,
      const Nodes::ComponentParameterNode& component_parameter_node2) override;
};
} // namespace Antares::Solver::Visitors
