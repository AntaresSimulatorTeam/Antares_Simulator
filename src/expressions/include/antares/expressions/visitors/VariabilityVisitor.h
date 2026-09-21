// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/expressions/visitors/NodeVisitor.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/study/system-model/component.h"

namespace Antares::Expressions::Visitors
{
/**
 * @brief Represents a visitor for determining the time and scenario dependency of nodes in a syntax
 * tree.
 */
class VariabilityVisitor: public NodeVisitor<LinearProblem::VariabilityType>
{
public:
    explicit VariabilityVisitor(const LinearProblem::OptimEntityContainer& optimEntityContainer,
                                const ModelerStudy::SystemModel::Component& component);

    std::string name() const override;

private:
    LinearProblem::VariabilityType visit(const Nodes::SumNode* add) override;
    LinearProblem::VariabilityType visit(const Nodes::SubtractionNode* add) override;
    LinearProblem::VariabilityType visit(const Nodes::MultiplicationNode* add) override;
    LinearProblem::VariabilityType visit(const Nodes::DivisionNode* add) override;
    LinearProblem::VariabilityType visit(const Nodes::EqualNode* add) override;
    LinearProblem::VariabilityType visit(const Nodes::LessThanOrEqualNode* add) override;
    LinearProblem::VariabilityType visit(const Nodes::GreaterThanOrEqualNode* add) override;
    LinearProblem::VariabilityType visit(const Nodes::NegationNode* neg) override;
    LinearProblem::VariabilityType visit(const Nodes::VariableNode* param) override;
    LinearProblem::VariabilityType visit(const Nodes::ParameterNode* param) override;
    LinearProblem::VariabilityType visit(const Nodes::LiteralNode* lit) override;
    LinearProblem::VariabilityType visit(const Nodes::PortFieldNode* port_field_node) override;
    LinearProblem::VariabilityType visit(const Nodes::PortFieldSumNode* port_field_node) override;
    LinearProblem::VariabilityType visit(const Nodes::TimeShiftNode* timeShiftNode) override;
    LinearProblem::VariabilityType visit(const Nodes::TimeIndexNode* timeIndexNode) override;
    LinearProblem::VariabilityType visit(const Nodes::TimeSumNode* timeSumNode) override;
    LinearProblem::VariabilityType visit(const Nodes::TPlusNode* node) override;
    LinearProblem::VariabilityType visit(const Nodes::AllTimeSumNode* timeSumNode) override;
    LinearProblem::VariabilityType visit(const Nodes::FunctionNode* node) override;

    LinearProblem::VariabilityType visitReducedCost(const Nodes::FunctionNode* node);
    LinearProblem::VariabilityType visitDual(const Nodes::FunctionNode* node);
    LinearProblem::VariabilityType visitPow(const Nodes::FunctionNode* node);
    LinearProblem::VariabilityType visitChildrenNodes(const Nodes::ParentNode* node);

    const LinearProblem::OptimEntityContainer& optimEntityContainer_;
    const ModelerStudy::SystemModel::Component& component_;
};
} // namespace Antares::Expressions::Visitors
