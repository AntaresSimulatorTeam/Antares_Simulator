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

#include <antares/solver/expressions/visitors/Visitor.h>

namespace Antares::Solver::Expressions
{
class PrintVisitor: public Visitor<std::string>
{
public:
    using Base = Visitor<std::string>;

private:
    std::string visit(const AddNode& add) override;
    std::string visit(const SubtractionNode& add) override;
    std::string visit(const MultiplicationNode& add) override;
    std::string visit(const DivisionNode& add) override;
    std::string visit(const EqualNode& add) override;
    std::string visit(const LessThanNode& add) override;
    std::string visit(const LessThanOrEqualNode& add) override;
    std::string visit(const GreaterThanNode& add) override;
    std::string visit(const GreaterThanOrEqualNode& add) override;

    std::string visit(const NegationNode& neg) override;

    std::string visit(const ParameterNode& param) override;
    std::string visit(const LiteralNode& lit) override;
    std::string visit(const PortFieldNode& port_field_node) override;
    std::string visit(const ComponentVariableNode& component_variable_node);
    std::string visit(const ComponentParameterNode& component_parameter_node);
};
} // namespace Antares::Solver::Expressions
