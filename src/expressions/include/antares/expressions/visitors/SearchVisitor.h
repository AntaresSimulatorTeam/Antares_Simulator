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
 * @brief Represents a visitor for searching node in a syntax tree.
 */
class SearchVisitor: public NodeVisitor<void, const std::string&>
{
public:
    SearchVisitor() = default;
    std::string name() const override;

    void visit(const Nodes::SumNode*, const std::string&) override;
    void visit(const Nodes::SubtractionNode*, const std::string&) override;
    void visit(const Nodes::MultiplicationNode*, const std::string&) override;
    void visit(const Nodes::DivisionNode*, const std::string&) override;
    void visit(const Nodes::EqualNode*, const std::string&) override;
    void visit(const Nodes::LessThanOrEqualNode*, const std::string&) override;
    void visit(const Nodes::GreaterThanOrEqualNode*, const std::string&) override;
    void visit(const Nodes::NegationNode*, const std::string&) override;
    void visit(const Nodes::VariableNode*, const std::string&) override;
    void visit(const Nodes::ParameterNode*, const std::string&) override;
    void visit(const Nodes::LiteralNode*, const std::string&) override;
    void visit(const Nodes::PortFieldNode*, const std::string&) override;
    void visit(const Nodes::PortFieldSumNode*, const std::string&) override;
    void visit(const Nodes::ComponentVariableNode*, const std::string&) override;
    void visit(const Nodes::ComponentParameterNode*, const std::string&) override;

void addToVectorIfNameMatches(const Nodes::Node* node, const std::string& name);
private:
    std::vector<const Nodes::Node*> results_;
};
} // namespace Antares::Expressions::Visitors
