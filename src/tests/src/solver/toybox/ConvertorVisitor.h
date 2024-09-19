
/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include "antares/antlr-interface/ExprVisitor.h"
#include "antares/solver/expressions/Registry.hxx"

// Visitor to convert nodes to Antares::Solver::Nodes
// TODO add reference to model to be able to resolve names as either parameters or variables
class ConvertorVisitor: public ExprVisitor
{
public:
    ConvertorVisitor(Antares::Solver::Registry<Antares::Solver::Nodes::Node>& registry):
        registry_(registry)
    {
    }

    virtual antlrcpp::Any visitChildren(antlr4::tree::ParseTree* node) override
    {
        for (auto child: node->children)
        {
            child->accept(this);
        }
        return antlrcpp::Any();
    }

    std::any visit(antlr4::tree::ParseTree* tree) override
    {
        return tree->accept(this);
    }

    std::any visitTerminal(antlr4::tree::TerminalNode* node) override
    {
        return std::any();
    }

    std::any visitErrorNode(antlr4::tree::ErrorNode* node) override
    {
        return std::any();
    }

    std::any visitIdentifier(ExprParser::IdentifierContext* context) override
    {
        auto variable_node = registry_.create<Antares::Solver::Nodes::ParameterNode>(
          context->getText());
        return variable_node;
    }

    std::any visitMuldiv(ExprParser::MuldivContext* context) override
    {
        // Meh
        // Having to know the underlying type of the node is not great. We can eitgher return
        // expression node containing the concrete node to be able to always anycast<Expression> Or
        // we can return a pair Node/type (difficult to return a type in c++)
        auto left = std::any_cast<Antares::Solver::Nodes::ParameterNode*>(visit(context->expr(0)));
        auto right = std::any_cast<Antares::Solver::Nodes::ParameterNode*>(visit(context->expr(1)));
        auto mult_node = registry_.create<Antares::Solver::Nodes::MultiplicationNode>(left, right);
        return mult_node;
    }

    std::any visitFullexpr(ExprParser::FullexprContext* context) override
    {
        return std::any();
    }

    std::any visitShift(ExprParser::ShiftContext* context) override
    {
        return std::any();
    }

    std::any visitNegation(ExprParser::NegationContext* context) override
    {
        return std::any();
    }

    std::any visitExpression(ExprParser::ExpressionContext* context) override
    {
        return std::any();
    }

    std::any visitComparison(ExprParser::ComparisonContext* context) override
    {
        return std::any();
    }

    std::any visitAddsub(ExprParser::AddsubContext* context) override
    {
        return std::any();
    }

    std::any visitPortField(ExprParser::PortFieldContext* context) override
    {
        return std::any();
    }

    std::any visitNumber(ExprParser::NumberContext* context) override
    {
        return std::any();
    }

    std::any visitTimeIndex(ExprParser::TimeIndexContext* context) override
    {
        return std::any();
    }

    std::any visitTimeShift(ExprParser::TimeShiftContext* context) override
    {
        return std::any();
    }

    std::any visitFunction(ExprParser::FunctionContext* context) override
    {
        return std::any();
    }

    std::any visitTimeShiftRange(ExprParser::TimeShiftRangeContext* context) override
    {
        return std::any();
    }

    std::any visitTimeRange(ExprParser::TimeRangeContext* context) override
    {
        return std::any();
    }

    Antares::Solver::Registry<Antares::Solver::Nodes::Node>& registry_;
};
