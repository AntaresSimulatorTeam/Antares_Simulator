/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once
#include <fmt/format.h>
#include <optional>
#include <ranges>

#include <antares/expressions/nodes/NodesForwardDeclaration.h>
#include <antares/expressions/visitors/NodeVisitor.h>
#include <antares/io/inputs/model-converter/ForbiddenNodes.h>

namespace Antares::IO::Inputs::ModelConverter
{
class NodeChecker final: public Expressions::Visitors::NodeVisitor<void>
{
public:
    explicit NodeChecker(const ForbiddenNodes& forbid, const std::string& expression);
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
    template<Expressions::Nodes::FunctionNodeType func>
    void checkConsistencyWithParents(const std::string& childName) const;

    template<class Child>
    void checkConsistencyWithParents(const std::string& childName) const;

    template<class Parent>
    void visitChildren(const std::string& parentName,
                       const std::vector<Expressions::Nodes::Node*>& children,
                       bool validateConsistencyWithParents);

    template<Expressions::Nodes::FunctionNodeType func>
    void visitChildren(const std::string& parentName,
                       const std::vector<Expressions::Nodes::Node*>& children,
                       bool validateConsistencyWithParents);

    template<class NodeType>
    void visitComparisonNode(const std::string& op,
                             const std::vector<Expressions::Nodes::Node*>& children);

    // Data members
    const ForbiddenNodes& forbiddenNodes_;
    std::vector<std::pair<std::string, std::type_index>> parentsStack_;
    const std::string& expression_;
};

struct ErrorInfo
{
    std::string expression;
    std::string childName;
    std::optional<std::string> parentName;
};

class ForbiddenNodeFound final: public std::invalid_argument
{
public:
    std::string ErrorMessage(const ErrorInfo& errorInfo)
    {
        if (errorInfo.parentName.has_value())
        {
            return fmt::format("'{}' is not allowed to contain '{}' in this context '{}'",
                               errorInfo.parentName.value(),
                               errorInfo.childName,
                               errorInfo.expression);
        }
        return fmt::format("'{}' is not allowed in this context '{}'",
                           errorInfo.childName,
                           errorInfo.expression);
    }

    explicit ForbiddenNodeFound(const ErrorInfo& errorInfo):
        invalid_argument(ErrorMessage(errorInfo))
    {
    }
};

template<typename Child>
void NodeChecker::checkConsistencyWithParents(const std::string& childName) const
{
    if (forbiddenNodes_.isGloballyForbidden<Child>())
    {
        throw ForbiddenNodeFound({.expression = expression_, .childName = childName});
    }

    for (const auto& [parentName, typeIndex]: std::ranges::reverse_view(parentsStack_))
    {
        if (forbiddenNodes_.isForbiddenFor<Child>(typeIndex))
        {
            throw ForbiddenNodeFound({expression_, childName, parentName});
        }
    }
}

template<Expressions::Nodes::FunctionNodeType func>
void NodeChecker::checkConsistencyWithParents(const std::string& childName) const
{
    if (forbiddenNodes_.isGloballyForbidden<func>())
    {
        throw ForbiddenNodeFound({.expression = expression_, .childName = childName});
    }

    for (const auto& [parentName, typeIndex]: std::ranges::reverse_view(parentsStack_))
    {
        if (forbiddenNodes_.isForbiddenFor<func>(typeIndex))
        {
            throw ForbiddenNodeFound({expression_, childName, parentName});
        }
    }
}

template<typename Parent>
void NodeChecker::visitChildren(const std::string& parentName,
                                const std::vector<Expressions::Nodes::Node*>& children,
                                bool validateConsistencyWithParents)
{
    if (validateConsistencyWithParents)
    {
        checkConsistencyWithParents<Parent>(parentName);
    }
    parentsStack_.emplace_back(parentName, typeid(Parent));

    for (const auto* child: children)
    {
        dispatch(child);
    }
    parentsStack_.pop_back();
}

template<Expressions::Nodes::FunctionNodeType func>
void NodeChecker::visitChildren(const std::string& parentName,
                                const std::vector<Expressions::Nodes::Node*>& children,
                                bool validateConsistencyWithParents)
{
    if (validateConsistencyWithParents)
    {
        checkConsistencyWithParents<func>(parentName);
    }
    parentsStack_.emplace_back(
      parentName,
      typeid(std::integral_constant<Expressions::Nodes::FunctionNodeType, func>));

    for (const auto* child: children)
    {
        dispatch(child);
    }
    parentsStack_.pop_back();
}

template<typename NodeType>
void NodeChecker::visitComparisonNode(const std::string& sign,
                                      const std::vector<Expressions::Nodes::Node*>& children)
{
    visitChildren<NodeType>("expression with " + sign, children, true);
}

} // namespace Antares::IO::Inputs::ModelConverter
