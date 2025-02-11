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

namespace Antares::Expressions::Visitors
{
/**
 * @brief Represents a visitor for printing nodes in a syntax tree as strings.
 */
class PrintVisitor: public NodeVisitor<std::string>
{
public:
    std::string name() const override;

private:
    std::string visit(const Nodes::SumNode* node) override;
    std::string visit(const Nodes::SubtractionNode* node) override;
    std::string visit(const Nodes::MultiplicationNode* node) override;
    std::string visit(const Nodes::DivisionNode* node) override;
    std::string visit(const Nodes::EqualNode* node) override;
    std::string visit(const Nodes::LessThanOrEqualNode* node) override;
    std::string visit(const Nodes::GreaterThanOrEqualNode* node) override;
    std::string visit(const Nodes::NegationNode* node) override;
    std::string visit(const Nodes::VariableNode* node) override;
    std::string visit(const Nodes::ParameterNode* node) override;
    std::string visit(const Nodes::LiteralNode* node) override;
    std::string visit(const Nodes::PortFieldNode* node) override;
    std::string visit(const Nodes::PortFieldSumNode* node) override;
    std::string visit(const Nodes::ComponentVariableNode* node) override;
    std::string visit(const Nodes::ComponentParameterNode* node) override;
};
} // namespace Antares::Expressions::Visitors
