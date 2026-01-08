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
    template<Expressions::Nodes::FunctionNodeType>
    void checkIsForbidden(const std::string& childName) const;

    template<class Node>
    void checkIsForbidden(const std::string& childName) const;

    template<class Node>
    void visitChildren(const std::string& nodeName,
                       const std::vector<Expressions::Nodes::Node*>& children);

    template<Expressions::Nodes::FunctionNodeType>
    void visitChildren(const std::string& parentName,
                       const std::vector<Expressions::Nodes::Node*>& children);

    // Data members
    const ForbiddenNodes& forbiddenNodes_;
    std::vector<std::pair<std::string, std::type_index>> parentsStack_;
    const std::string& expression_;
};

class ForbiddenNodeFound final: public std::invalid_argument
{
public:
    explicit ForbiddenNodeFound(const std::string expr,
                                const std::string node,
                                const std::string parent = "");
};

template<typename Node>
void NodeChecker::checkIsForbidden(const std::string& nodeName) const
{
    std::type_index typeId = typeIndexOf<Node>();

    if (forbiddenNodes_.isGloballyForbidden(typeId))
    {
        throw ForbiddenNodeFound(expression_, nodeName);
    }

    for (const auto& [parentNodeName, parentTypeIndex]: std::ranges::reverse_view(parentsStack_))
    {
        if (forbiddenNodes_.isForbiddenByParent(parentTypeIndex, typeId))
        {
            throw ForbiddenNodeFound(expression_, nodeName, parentNodeName);
        }
    }
}

template<Expressions::Nodes::FunctionNodeType functionNodeType>
void NodeChecker::checkIsForbidden(const std::string& nodeName) const
{
    std::type_index typeId = typeIndexOf<functionNodeType>();
    
    if (forbiddenNodes_.isGloballyForbidden(typeId))
    {
        throw ForbiddenNodeFound(expression_, nodeName);
    }

    for (const auto& [parentNodeName, parentTypeIndex]: std::ranges::reverse_view(parentsStack_))
    {
        if (forbiddenNodes_.isForbiddenByParent(parentTypeIndex, typeId))
        {
            throw ForbiddenNodeFound(expression_, nodeName, parentNodeName);
        }
    }
}

template<typename Node>
void NodeChecker::visitChildren(const std::string& nodeName,
                                const std::vector<Expressions::Nodes::Node*>& children)
{
    parentsStack_.emplace_back(nodeName, typeIndexOf<Node>());
    for (const auto* child: children)
    {
        dispatch(child);
    }
    parentsStack_.pop_back();
}

template<Expressions::Nodes::FunctionNodeType functionNodeType>
void NodeChecker::visitChildren(const std::string& nodeName,
                                const std::vector<Expressions::Nodes::Node*>& children)
{
    parentsStack_.emplace_back(nodeName, typeIndexOf<functionNodeType>());
    for (const auto* child: children)
    {
        dispatch(child);
    }
    parentsStack_.pop_back();
}

} // namespace Antares::IO::Inputs::ModelConverter
