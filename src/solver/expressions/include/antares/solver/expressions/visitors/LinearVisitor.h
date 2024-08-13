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

#include "antares/solver/expressions/nodes/NodeVisitor.h"

namespace Antares::Solver::Visitors
{

class LinearOperations
{
public:
    enum LinearStatus
    {
        CONSTANT,
        LINEAR,
        NON_LINEAR
    };

    constexpr LinearOperations() = default;
    constexpr LinearOperations(const LinearStatus& status);
    constexpr LinearOperations(const LinearOperations& other) = default;
    constexpr LinearOperations operator*(const LinearOperations& other);
    constexpr LinearOperations operator/(const LinearOperations& other);
    constexpr LinearOperations operator+(const LinearOperations& other);
    constexpr LinearOperations operator-(const LinearOperations& other);
    // Conversions
    constexpr explicit operator bool() const = delete;

    constexpr operator LinearStatus() const
    {
        return status_;
    }

    // Comparisons
    constexpr bool operator==(LinearOperations a) const
    {
        return status_ == a.status_;
    }

    constexpr bool operator==(LinearStatus status) const
    {
        return status_ == status;
    }

    constexpr bool operator!=(LinearOperations a) const
    {
        return status_ != a.status_;
    }

private:
    LinearStatus status_;
};

class LinearVisitor: public Nodes::NodeVisitor<LinearOperations>
{
public:
    using Base = Nodes::NodeVisitor<LinearOperations>;

private:
    LinearOperations visit(const Nodes::AddNode& add) override;
    LinearOperations visit(const Nodes::SubtractionNode& add) override;
    LinearOperations visit(const Nodes::MultiplicationNode& add) override;
    LinearOperations visit(const Nodes::DivisionNode& add) override;
    LinearOperations visit(const Nodes::EqualNode& add) override;
    LinearOperations visit(const Nodes::LessThanOrEqualNode& add) override;
    LinearOperations visit(const Nodes::GreaterThanOrEqualNode& add) override;
    LinearOperations visit(const Nodes::NegationNode& neg) override;
    LinearOperations visit(const Nodes::VariableNode& param) override;
    LinearOperations visit(const Nodes::ParameterNode& param) override;
    LinearOperations visit(const Nodes::LiteralNode& lit) override;
    LinearOperations visit(const Nodes::PortFieldNode& port_field_node) override;
    LinearOperations visit(const Nodes::ComponentVariableNode& component_variable_node) override;
    LinearOperations visit(const Nodes::ComponentParameterNode& component_parameter_node) override;
};
} // namespace Antares::Solver::Visitors
