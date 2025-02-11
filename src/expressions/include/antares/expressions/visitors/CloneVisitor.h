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
 * @brief Represents a visitor for cloning nodes in a syntax tree.
 */
class CloneVisitor: public NodeVisitor<Nodes::Node*>
{
public:
    /**
     * @brief Constructs a clone visitor with the specified registry for creating new nodes.
     *
     * @param registry The registry used for creating new nodes.
     */
    explicit CloneVisitor(Registry<Nodes::Node>& registry);
    std::string name() const override;

    Nodes::Node* visit(const Nodes::SumNode* node) override;
    Nodes::Node* visit(const Nodes::SubtractionNode* node) override;
    Nodes::Node* visit(const Nodes::MultiplicationNode* node) override;
    Nodes::Node* visit(const Nodes::DivisionNode* node) override;
    Nodes::Node* visit(const Nodes::EqualNode* node) override;
    Nodes::Node* visit(const Nodes::LessThanOrEqualNode* node) override;
    Nodes::Node* visit(const Nodes::GreaterThanOrEqualNode* node) override;
    Nodes::Node* visit(const Nodes::NegationNode* node) override;
    Nodes::Node* visit(const Nodes::VariableNode* node) override;
    Nodes::Node* visit(const Nodes::ParameterNode* node) override;
    Nodes::Node* visit(const Nodes::LiteralNode* node) override;
    Nodes::Node* visit(const Nodes::PortFieldNode* node) override;
    Nodes::Node* visit(const Nodes::PortFieldSumNode* node) override;
    Nodes::Node* visit(const Nodes::ComponentVariableNode* node) override;
    Nodes::Node* visit(const Nodes::ComponentParameterNode* node) override;

private:
    Registry<Nodes::Node>& registry_;
};
} // namespace Antares::Expressions::Visitors
