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

#include <antares/solver/expressions/Registry.hxx>
#include <antares/solver/expressions/visitors/Visitor.h>

namespace Antares::Solver::Expressions
{
class CloneVisitor: public Visitor<Node*>
{
public:
    CloneVisitor(Registry<Node>& mem);

    Node* visit(const AddNode& add) override;
    Node* visit(const SubtractionNode& add) override;
    Node* visit(const MultiplicationNode& add) override;
    Node* visit(const DivisionNode& add) override;
    Node* visit(const EqualNode& add) override;
    Node* visit(const LessThanNode& add) override;
    Node* visit(const LessThanOrEqualNode& add) override;
    Node* visit(const GreaterThanNode& add) override;
    Node* visit(const GreaterThanOrEqualNode& add) override;
    Node* visit(const NegationNode& neg) override;
    Node* visit(const VariableNode& param) override;
    Node* visit(const ParameterNode& param) override;
    Node* visit(const LiteralNode& param) override;
    Node* visit(const PortFieldNode& port_field_node) override;
    Node* visit(const ComponentVariableNode& component_node) override;
    Node* visit(const ComponentParameterNode& component_node) override;

private:
    Registry<Node>& registry_;
};
} // namespace Antares::Solver::Expressions
