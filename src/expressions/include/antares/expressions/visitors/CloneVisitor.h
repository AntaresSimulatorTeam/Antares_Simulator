// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
    Nodes::Node* visit(const Nodes::TimeShiftNode* node) override;
    Nodes::Node* visit(const Nodes::TimeIndexNode* node) override;
    Nodes::Node* visit(const Nodes::TimeSumNode* node) override;
    Nodes::Node* visit(const Nodes::AllTimeSumNode* node) override;
    Nodes::Node* visit(const Nodes::FunctionNode* node) override;

private:
    Registry<Nodes::Node>& registry_;
};
} // namespace Antares::Expressions::Visitors
