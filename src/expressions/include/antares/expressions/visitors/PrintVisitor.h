// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/expressions/visitors/NodeVisitor.h"

namespace Antares::Expressions::Visitors
{

std::string trimAndFormat(std::string s);

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
    std::string visit(const Nodes::TimeShiftNode* node) override;
    std::string visit(const Nodes::TimeIndexNode* node) override;
    std::string visit(const Nodes::TimeSumNode* node) override;
    std::string visit(const Nodes::AllTimeSumNode* node) override;
    std::string visit(const Nodes::FunctionNode* node) override;
};
} // namespace Antares::Expressions::Visitors
