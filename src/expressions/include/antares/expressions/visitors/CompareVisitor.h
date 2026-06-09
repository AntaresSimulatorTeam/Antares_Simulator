// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/expressions/Registry.hxx>
#include "antares/expressions/visitors/NodeVisitor.h"

namespace Antares::Expressions::Visitors
{
/**
 * @brief Represents a visitor for comparing nodes in a syntax tree.
 */
class CompareVisitor: public NodeVisitor<bool, const Nodes::Node*>
{
public:
    CompareVisitor() = default;
    std::string name() const override;

    bool visit(const Nodes::SumNode* add, const Nodes::Node* other) override;
    bool visit(const Nodes::SubtractionNode* add, const Nodes::Node* other) override;
    bool visit(const Nodes::MultiplicationNode* add, const Nodes::Node* other) override;
    bool visit(const Nodes::DivisionNode* add, const Nodes::Node* other) override;
    bool visit(const Nodes::EqualNode* add, const Nodes::Node* other) override;
    bool visit(const Nodes::LessThanOrEqualNode* add, const Nodes::Node* other) override;
    bool visit(const Nodes::GreaterThanOrEqualNode* add, const Nodes::Node* other) override;
    bool visit(const Nodes::NegationNode* neg, const Nodes::Node* other) override;
    bool visit(const Nodes::VariableNode* param, const Nodes::Node* other) override;
    bool visit(const Nodes::ParameterNode* param, const Nodes::Node* other) override;
    bool visit(const Nodes::LiteralNode* param, const Nodes::Node* other) override;
    bool visit(const Nodes::PortFieldNode* port_field_node, const Nodes::Node* other) override;
    bool visit(const Nodes::PortFieldSumNode* port_field_node, const Nodes::Node* other) override;
    bool visit(const Nodes::TimeShiftNode* timeShiftNode, const Nodes::Node* other) override;
    bool visit(const Nodes::TimeIndexNode* timeIndexNode, const Nodes::Node* other) override;
    bool visit(const Nodes::TimeSumNode* timeSumNode, const Nodes::Node* other) override;
    bool visit(const Nodes::TPlusNode* node, const Nodes::Node* other) override;
    bool visit(const Nodes::AllTimeSumNode* alltimeSumNode, const Nodes::Node* other) override;
    bool visit(const Nodes::FunctionNode* node, const Nodes::Node* other) override;
};
} // namespace Antares::Expressions::Visitors
