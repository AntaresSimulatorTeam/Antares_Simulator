/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#include "antares/modeler-optimisation-container/EvaluationContext.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/study/system-model/component.h"

namespace Antares::Expressions::Visitors
{
/**
 * @brief Represents a visitor for determining the time and scenario dependency of nodes in a syntax
 * tree.
 */
class VariabilityVisitor: public NodeVisitor<Optimisation::VariabilityType>
{
public:
    explicit VariabilityVisitor(const Optimisation::OptimEntityContainer& optimEntityContainer,
                                const ModelerStudy::SystemModel::Component& component);

    std::string name() const override;

private:
    Optimisation::VariabilityType visit(const Nodes::SumNode* add) override;
    Optimisation::VariabilityType visit(const Nodes::SubtractionNode* add) override;
    Optimisation::VariabilityType visit(const Nodes::MultiplicationNode* add) override;
    Optimisation::VariabilityType visit(const Nodes::DivisionNode* add) override;
    Optimisation::VariabilityType visit(const Nodes::EqualNode* add) override;
    Optimisation::VariabilityType visit(const Nodes::LessThanOrEqualNode* add) override;
    Optimisation::VariabilityType visit(const Nodes::GreaterThanOrEqualNode* add) override;
    Optimisation::VariabilityType visit(const Nodes::NegationNode* neg) override;
    Optimisation::VariabilityType visit(const Nodes::VariableNode* param) override;
    Optimisation::VariabilityType visit(const Nodes::ParameterNode* param) override;
    Optimisation::VariabilityType visit(const Nodes::LiteralNode* lit) override;
    Optimisation::VariabilityType visit(const Nodes::PortFieldNode* port_field_node) override;
    Optimisation::VariabilityType visit(const Nodes::PortFieldSumNode* port_field_node) override;
    Optimisation::VariabilityType visit(const Nodes::TimeShiftNode* timeShiftNode) override;
    Optimisation::VariabilityType visit(const Nodes::TimeIndexNode* timeIndexNode) override;
    Optimisation::VariabilityType visit(const Nodes::TimeSumNode* timeSumNode) override;
    Optimisation::VariabilityType visit(const Nodes::AllTimeSumNode* timeSumNode) override;
    Optimisation::VariabilityType visit(const Nodes::FunctionNode* node) override;

    Optimisation::VariabilityType visitReducedCost(const Nodes::FunctionNode* node);
    Optimisation::VariabilityType visitDual(const Nodes::FunctionNode* node);
    Optimisation::VariabilityType visitPow(const Nodes::FunctionNode* node);
    Optimisation::VariabilityType visitChildrenNodes(const Nodes::ParentNode* node);

    const Optimisation::OptimEntityContainer& optimEntityContainer_;
    const ModelerStudy::SystemModel::Component& component_;
    const Optimisation::EvaluationContext& context_;
};
} // namespace Antares::Expressions::Visitors
