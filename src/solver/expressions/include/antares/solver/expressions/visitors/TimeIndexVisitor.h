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

#include "antares/solver/expressions/visitors/ExpressionTimeType.h"
#include "antares/solver/expressions/visitors/NodeVisitor.h"

namespace Antares::Solver::Visitors
{

class TimeIndexVisitor: public Nodes::NodeVisitor<ExpressionTimeType>
{
public:
    using Base = Nodes::NodeVisitor<ExpressionTimeType>;

private:
    ExpressionTimeType visit(const Nodes::AddNode& add) override;
    ExpressionTimeType visit(const Nodes::SubtractionNode& add) override;
    ExpressionTimeType visit(const Nodes::MultiplicationNode& add) override;
    ExpressionTimeType visit(const Nodes::DivisionNode& add) override;
    ExpressionTimeType visit(const Nodes::EqualNode& add) override;
    ExpressionTimeType visit(const Nodes::LessThanOrEqualNode& add) override;
    ExpressionTimeType visit(const Nodes::GreaterThanOrEqualNode& add) override;
    ExpressionTimeType visit(const Nodes::NegationNode& neg) override;
    ExpressionTimeType visit(const Nodes::VariableNode& param) override;
    ExpressionTimeType visit(const Nodes::ParameterNode& param) override;
    ExpressionTimeType visit(const Nodes::LiteralNode& lit) override;
    ExpressionTimeType visit(const Nodes::PortFieldNode& port_field_node) override;
    ExpressionTimeType visit(const Nodes::ComponentVariableNode& component_variable_node) override;
    ExpressionTimeType visit(
      const Nodes::ComponentParameterNode& component_parameter_node) override;
};
} // namespace Antares::Solver::Visitors