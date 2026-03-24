// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <fmt/format.h>
#include <optional>
#include <ranges>

#include <antares/expressions/nodes/NodesForwardDeclaration.h>
#include <antares/expressions/visitors/NodeVisitor.h>
#include <antares/io/inputs/forbidden-nodes/ForbiddenNodes.h>
#include "antares/study/system-model/component.h"

namespace Antares::IO::Inputs::ForbidNodes
{

class ForbiddenNodeFound final: public std::invalid_argument
{
public:
    explicit ForbiddenNodeFound(const std::string expr,
                                const std::string node,
                                const std::string parent = "");
};

class ForbiddenNodesVisitor: public Expressions::Visitors::NodeVisitor<void>
{
public:
    explicit ForbiddenNodesVisitor(const ForbiddenNodes& forbid, const std::string& expression);

    [[nodiscard]] std::string name() const override;

    void visit(const Expressions::Nodes::SumNode*) override;
    void visit(const Expressions::Nodes::SubtractionNode*) override;
    void visit(const Expressions::Nodes::MultiplicationNode*) override;
    void visit(const Expressions::Nodes::DivisionNode*) override;
    void visit(const Expressions::Nodes::EqualNode*) override;
    void visit(const Expressions::Nodes::LessThanOrEqualNode*) override;
    void visit(const Expressions::Nodes::GreaterThanOrEqualNode*) override;
    void visit(const Expressions::Nodes::NegationNode*) override;
    void visit(const Expressions::Nodes::LiteralNode*) override;
    void visit(const Expressions::Nodes::VariableNode*) override;
    void visit(const Expressions::Nodes::ParameterNode*) override;
    void visit(const Expressions::Nodes::PortFieldNode*) override;
    void visit(const Expressions::Nodes::PortFieldSumNode*) override;
    void visit(const Expressions::Nodes::TimeShiftNode*) override;
    void visit(const Expressions::Nodes::TimeIndexNode*) override;
    void visit(const Expressions::Nodes::TimeSumNode*) override;
    void visit(const Expressions::Nodes::AllTimeSumNode*) override;
    void visit(const Expressions::Nodes::FunctionNode*) override;

private:
    // Member functions
    void checkIsForbidden(const Expressions::Nodes::Node* node,
                          const std::type_index& nodeTypeId) const;

    void checkIsGloballyForbidden(const std::type_index& nodeTypeId,
                                  const Antares::Expressions::Nodes::Node* node) const;

    void checkIsForbiddenByParent(const std::type_index& nodeTypeId,
                                  const Antares::Expressions::Nodes::Node* node) const;

    void visitChildren(const Expressions::Nodes::ParentNode* node,
                       const std::type_index& nodeTypeId);

protected:
    // Data members
    const ForbiddenNodes& forbiddenNodes_;
    std::vector<std::pair<std::string, std::type_index>> parentsStack_;
    const std::string& expression_;
};

class ForbiddenNodesInComponentVisitor final: public ForbiddenNodesVisitor
{
public:
    ForbiddenNodesInComponentVisitor(const ForbiddenNodes& forbid,
                                     const std::string& expression,
                                     const ModelerStudy::SystemModel::Component& component);

    void visit(const Expressions::Nodes::PortFieldSumNode* portFieldSumNode) override;

private:
    const ModelerStudy::SystemModel::Component& component_;
};

} // namespace Antares::IO::Inputs::ForbidNodes
