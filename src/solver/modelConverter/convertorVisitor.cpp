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

#include <iostream>

/* #include <antares/logs/logs.h> */
#include <antares/solver/modelConverter/convertorVisitor.h>

#include "ExprLexer.h"
#include "ExprParser.h"
#include "antlr4-runtime.h"

namespace Antares::Solver::ModelConverter
{

using namespace Antares::Solver::Nodes;

static Node* toNodePtr(const std::any& a)
{
    return std::any_cast<Node*>(a);
}

Node* convertExpressionToNode(const std::string& exprStr,
                              Antares::Solver::Registry<Node>& registry,
                              const ModelParser::Model& model)
{
    if (exprStr.empty())
    {
        return nullptr;
    }

    antlr4::ANTLRInputStream input(exprStr);
    ExprLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    ExprParser parser(&tokens);

    ExprParser::ExprContext* tree = parser.expr();

    ConvertorVisitor visitor(registry, model);
    return toNodePtr(visitor.visit(tree));
}

ConvertorVisitor::ConvertorVisitor(Antares::Solver::Registry<Node>& registry,
                                   const ModelParser::Model& model):
    registry_(registry),
    model_(model)
{
}

std::any ConvertorVisitor::visit(antlr4::tree::ParseTree* tree)
{
    return tree->accept(this);
}

class NoParameterOrVariableWithThisName: public std::exception
{
public:
    NoParameterOrVariableWithThisName(const std::string& id)
    {
        msg_ = "No parameter or variable found for this identifier: " + id;
    }

    const char* what() const noexcept override
    {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

std::any ConvertorVisitor::visitIdentifier(ExprParser::IdentifierContext* context)
{
    for (const auto& param: model_.parameters)
    {
        if (param.id == context->IDENTIFIER()->getText())
        {
            return static_cast<Node*>(registry_.create<ParameterNode>(param.id));
        }
    }

    for (const auto& var: model_.variables)
    {
        if (var.id == context->getText())
        {
            return static_cast<Node*>(registry_.create<VariableNode>(var.id));
        }
    }

    /* logs.error() << "No parameter or variable found with this name: " << context->getText(); */
    throw NoParameterOrVariableWithThisName(context->getText());
}

std::any ConvertorVisitor::visitMuldiv(ExprParser::MuldivContext* context)
{
    auto* left = toNodePtr(visit(context->expr(0)));
    auto* right = toNodePtr(visit(context->expr(1)));

    std::string op = context->op->getText();
    return (op == "*") ? static_cast<Node*>(registry_.create<MultiplicationNode>(left, right))
                       : static_cast<Node*>(registry_.create<DivisionNode>(left, right));
}

std::any ConvertorVisitor::visitFullexpr(ExprParser::FullexprContext* context)
{
    return context->expr()->accept(this);
}

std::any ConvertorVisitor::visitNegation(ExprParser::NegationContext* context)
{
    auto n = toNodePtr(context->expr()->accept(this));
    return static_cast<Node*>(registry_.create<NegationNode>(n));
}

std::any ConvertorVisitor::visitExpression(ExprParser::ExpressionContext* context)
{
    return context->expr()->accept(this);
}

std::any ConvertorVisitor::visitComparison(ExprParser::ComparisonContext* context)
{
    auto* left = toNodePtr(visit(context->expr(0)));
    auto* right = toNodePtr(visit(context->expr(1)));

    std::string op = context->COMPARISON()->getText();
    if (op == "=")
    {
        return static_cast<Node*>(registry_.create<EqualNode>(left, right));
    }
    else if (op == "<=")
    {
        return static_cast<Node*>(registry_.create<LessThanOrEqualNode>(left, right));
    }
    else
    {
        return static_cast<Node*>(registry_.create<GreaterThanOrEqualNode>(left, right));
    }
}

std::any ConvertorVisitor::visitAddsub(ExprParser::AddsubContext* context)
{
    auto* left = toNodePtr(visit(context->expr(0)));
    auto* right = toNodePtr(visit(context->expr(1)));

    std::string op = context->op->getText();
    return (op == "+") ? static_cast<Node*>(registry_.create<SumNode>(left, right))
                       : static_cast<Node*>(registry_.create<SubtractionNode>(left, right));
}

// TODO implement this
std::any ConvertorVisitor::visitPortField([[maybe_unused]] ExprParser::PortFieldContext* context)
{
    throw std::runtime_error(std::string("Node portfield not implemented yet"));
    return std::any();
}

std::any ConvertorVisitor::visitNumber(ExprParser::NumberContext* context)
{
    double d = stod(context->getText());
    return static_cast<Node*>(registry_.create<LiteralNode>(d));
}

// TODO implement this
std::any ConvertorVisitor::visitTimeIndex([[maybe_unused]] ExprParser::TimeIndexContext* context)
{
    throw std::runtime_error(std::string("Node time index not implemented yet"));
    return std::any();
}

// TODO implement this
std::any ConvertorVisitor::visitTimeShift([[maybe_unused]] ExprParser::TimeShiftContext* context)
{
    throw std::runtime_error(std::string("Node time shift not implemented yet"));
    return std::any();
}

// TODO implement this
std::any ConvertorVisitor::visitFunction([[maybe_unused]] ExprParser::FunctionContext* context)
{
    throw std::runtime_error(std::string("Node function not implemented yet"));
    return std::any();
}

// TODO implement this
std::any ConvertorVisitor::visitTimeSum([[maybe_unused]] ExprParser::TimeSumContext* context)
{
    throw std::runtime_error(std::string("Node time sum not implemented yet"));
    return std::any();
}

// TODO implement this
std::any ConvertorVisitor::visitAllTimeSum([[maybe_unused]] ExprParser::AllTimeSumContext* context)
{
    throw std::runtime_error(std::string("Node all time sum  not implemented yet"));
    return std::any();
}

// shift related, not tested
std::any ConvertorVisitor::visitSignedAtom(ExprParser::SignedAtomContext* context)
{
    auto a = context->atom()->accept(this);
    if (context->op->getText() == "-")
    {
        return static_cast<Node*>(registry_.create<NegationNode>(toNodePtr(a)));
    }
    return a;
}

std::any ConvertorVisitor::visitUnsignedAtom(ExprParser::UnsignedAtomContext* context)
{
    return context->atom()->accept(this);
}

// TODO implement this
std::any ConvertorVisitor::visitRightAtom([[maybe_unused]] ExprParser::RightAtomContext* context)
{
    throw std::runtime_error(std::string("Node right atom not implemented yet"));
    return std::any();
}

// TODO implement this
std::any ConvertorVisitor::visitShift([[maybe_unused]] ExprParser::ShiftContext* context)
{
    throw std::runtime_error(std::string("Node shift not implemented yet"));
    return std::any();
}

// TODO implement this
std::any ConvertorVisitor::visitShiftAddsub(
  [[maybe_unused]] ExprParser::ShiftAddsubContext* context)
{
    throw std::runtime_error(std::string("Node shift add sub not implemented yet"));
    return std::any();
}

// TODO implement this
std::any ConvertorVisitor::visitShiftMuldiv(
  [[maybe_unused]] ExprParser::ShiftMuldivContext* context)
{
    throw std::runtime_error(std::string("Node shift mul div not implemented yet"));
    return std::any();
}

// TODO implement this
std::any ConvertorVisitor::visitRightMuldiv(
  [[maybe_unused]] ExprParser::RightMuldivContext* context)
{
    throw std::runtime_error(std::string("Node right mul div not implemented yet"));
    return std::any();
}

// TODO implement this
std::any ConvertorVisitor::visitSignedExpression(
  [[maybe_unused]] ExprParser::SignedExpressionContext* context)
{
    throw std::runtime_error(std::string("Node signed expression not implemented yet"));
    return std::any();
}

std::any ConvertorVisitor::visitRightExpression(ExprParser::RightExpressionContext* context)
{
    return context->expr()->accept(this);
}

} // namespace Antares::Solver::ModelConverter
