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

#include "antares/expressions/visitors/NodeVisitor.h"
#include "antares/expressions/visitors/TimeIndex.h"

namespace Antares::Expressions::Visitors
{
/**
 * @brief Represents a visitor for determining the time and scenario dependency of nodes in a syntax
 * tree.
 */
class TimeIndexVisitor: public NodeVisitor<TimeIndex>
{
public:
    // TODO if Node contains time and scenario dependency,  do we need this ctor?
    /**
     * @brief Constructs a time index visitor with the specified context.
     *
     * @param context The context containing the time index for each node.
     */
    explicit TimeIndexVisitor(std::unordered_map<const Nodes::Node*, TimeIndex> context);
    explicit TimeIndexVisitor() = default;

    std::string name() const override;

private:
    std::unordered_map<const Nodes::Node*, TimeIndex> context_;
    TimeIndex visit(const Nodes::SumNode* add) override;
    TimeIndex visit(const Nodes::SubtractionNode* add) override;
    TimeIndex visit(const Nodes::MultiplicationNode* add) override;
    TimeIndex visit(const Nodes::DivisionNode* add) override;
    TimeIndex visit(const Nodes::EqualNode* add) override;
    TimeIndex visit(const Nodes::LessThanOrEqualNode* add) override;
    TimeIndex visit(const Nodes::GreaterThanOrEqualNode* add) override;
    TimeIndex visit(const Nodes::NegationNode* neg) override;
    TimeIndex visit(const Nodes::VariableNode* param) override;
    TimeIndex visit(const Nodes::ParameterNode* param) override;
    TimeIndex visit(const Nodes::LiteralNode* lit) override;
    TimeIndex visit(const Nodes::PortFieldNode* port_field_node) override;
    TimeIndex visit(const Nodes::PortFieldSumNode* port_field_node) override;
    TimeIndex visit(const Nodes::ComponentVariableNode* component_variable_node) override;
    TimeIndex visit(const Nodes::ComponentParameterNode* component_parameter_node) override;
};
} // namespace Antares::Expressions::Visitors
