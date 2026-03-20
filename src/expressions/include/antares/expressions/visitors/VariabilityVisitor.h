// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
                                const ModelerStudy::SystemModel::Component& component,
                                const Optimisation::LinearProblemApi::ILinearProblemData* data,
                                const Optimisation::LinearProblemApi::IScenario* scenario);

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
    const Optimisation::LinearProblemApi::ILinearProblemData* data_;
    const Optimisation::LinearProblemApi::IScenario* scenario_;
    const Optimisation::EvaluationContext evalContext_;
};
} // namespace Antares::Expressions::Visitors
