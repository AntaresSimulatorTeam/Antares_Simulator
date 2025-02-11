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

#include <antares/expressions/Registry.hxx>
#include "antares/expressions/visitors/NodeVisitor.h"

namespace Antares::Expressions::Visitors
{
/**
 * @brief Represents a visitor for comparing nodes in a syntax tree.
 */
class CompareVisitor: public NodeVisitor<bool, const Nodes::Node*>
{
public:
    CompareVisitor() = default;
    std::string name() const override;

    bool visit(const Nodes::SumNode* add, const Nodes::Node* other) override;
    bool visit(const Nodes::SubtractionNode* add, const Nodes::Node* other) override;
    bool visit(const Nodes::MultiplicationNode* add, const Nodes::Node* other) override;
    bool visit(const Nodes::DivisionNode* add, const Nodes::Node* other) override;
    bool visit(const Nodes::EqualNode* add, const Nodes::Node* other) override;
    bool visit(const Nodes::LessThanOrEqualNode* add, const Nodes::Node* other) override;
    bool visit(const Nodes::GreaterThanOrEqualNode* add, const Nodes::Node* other) override;
    bool visit(const Nodes::NegationNode* neg, const Nodes::Node* other) override;
    bool visit(const Nodes::VariableNode* param, const Nodes::Node* other) override;
    bool visit(const Nodes::ParameterNode* param, const Nodes::Node* other) override;
    bool visit(const Nodes::LiteralNode* param, const Nodes::Node* other) override;
    bool visit(const Nodes::PortFieldNode* port_field_node, const Nodes::Node* other) override;
    bool visit(const Nodes::PortFieldSumNode* port_field_node, const Nodes::Node* other) override;
    bool visit(const Nodes::ComponentVariableNode* component_node,
               const Nodes::Node* other) override;
    bool visit(const Nodes::ComponentParameterNode* component_node,
               const Nodes::Node* other) override;
};
} // namespace Antares::Expressions::Visitors
