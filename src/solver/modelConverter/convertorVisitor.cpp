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

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/modelConverter/convertorVisitor.h>
#include "antares/solver/expressions/visitors/TimeIndex.h"

#include "ExprLexer.h"
#include "ExprParser.h"
#include "antlr4-runtime.h"

namespace Antares::Solver::ModelConverter
{

using namespace Antares::Solver::Nodes;

/// Visitor to convert ANTLR expressions to Antares::Solver::Nodes
class ConvertorVisitor: public ExprVisitor
{
public:
    ConvertorVisitor(Registry<Nodes::Node>& registry, const ModelParser::Model& model);

    std::any visit(antlr4::tree::ParseTree* tree) override;

    std::any visitIdentifier(ExprParser::IdentifierContext* context) override;
    std::any visitMuldiv(ExprParser::MuldivContext* context) override;
    std::any visitFullexpr(ExprParser::FullexprContext* context) override;
    std::any visitShift(ExprParser::ShiftContext* context) override;
    std::any visitNegation(ExprParser::NegationContext* context) override;
    std::any visitExpression(ExprParser::ExpressionContext* context) override;
    std::any visitComparison(ExprParser::ComparisonContext* context) override;
    std::any visitAddsub(ExprParser::AddsubContext* context) override;
    std::any visitPortField(ExprParser::PortFieldContext* context) override;
    std::any visitNumber(ExprParser::NumberContext* context) override;
    std::any visitTimeIndex(ExprParser::TimeIndexContext* context) override;
    std::any visitTimeShift(ExprParser::TimeShiftContext* context) override;
    std::any visitFunction(ExprParser::FunctionContext* context) override;

    std::any visitTimeSum(ExprParser::TimeSumContext* context) override;
    std::any visitAllTimeSum(ExprParser::AllTimeSumContext* context) override;
    std::any visitSignedAtom(ExprParser::SignedAtomContext* context) override;
    std::any visitUnsignedAtom(ExprParser::UnsignedAtomContext* context) override;
    std::any visitRightAtom(ExprParser::RightAtomContext* context) override;
    std::any visitSignedExpression(ExprParser::SignedExpressionContext* context) override;
    std::any visitShiftAddsub(ExprParser::ShiftAddsubContext* context) override;
    std::any visitShiftMuldiv(ExprParser::ShiftMuldivContext* context) override;
    std::any visitRightMuldiv(ExprParser::RightMuldivContext* context) override;
    std::any visitRightExpression(ExprParser::RightExpressionContext* context) override;

private:
    Registry<Nodes::Node>& registry_;
    const ModelParser::Model& model_;
};

NodeRegistry convertExpressionToNode(const std::string& exprStr, const ModelParser::Model& model)
{
    if (exprStr.empty())
    {
        return {};
    }
    antlr4::ANTLRInputStream input(exprStr);
    ExprLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    ExprParser parser(&tokens);

    ExprParser::ExprContext* tree = parser.expr();
    Antares::Solver::Registry<Node> registry;
    ConvertorVisitor visitor(registry, model);
    auto root = std::any_cast<Node*>(visitor.visit(tree));
    return NodeRegistry(root, std::move(registry));
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

class NoParameterOrVariableWithThisName: public std::runtime_error
{
public:
    explicit NoParameterOrVariableWithThisName(const std::string& name):
        runtime_error("No parameter or variable found for this identifier: " + name)
    {
    }
};

// to silent warning, convert bool to unsigned int
static constexpr unsigned int convertBool(bool in)
{
    return in ? 1 : 0;
}

static constexpr Visitors::TimeIndex convertToTimeIndex(bool timedependent, bool scenariodependent)
{
    return static_cast<Visitors::TimeIndex>((convertBool(scenariodependent) << 1)
                                            | convertBool(timedependent));
}

std::any ConvertorVisitor::visitIdentifier(ExprParser::IdentifierContext* context)
{
    for (const auto& param: model_.parameters)
    {
        if (param.id == context->IDENTIFIER()->getText())
        {
            return static_cast<Node*>(
              registry_.create<ParameterNode>(param.id,
                                              convertToTimeIndex(param.time_dependent,
                                                                 param.scenario_dependent)));
        }
    }

    for (const auto& var: model_.variables)
    {
        if (var.id == context->getText())
        {
            return static_cast<Node*>(
              registry_.create<VariableNode>(var.id,
                                             convertToTimeIndex(var.time_dependent,
                                                                var.scenario_dependent)));
        }
    }

    throw NoParameterOrVariableWithThisName(context->getText());
}

std::any ConvertorVisitor::visitMuldiv(ExprParser::MuldivContext* context)
{
    auto* left = std::any_cast<Node*>(visit(context->expr(0)));
    auto* right = std::any_cast<Node*>(visit(context->expr(1)));

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
    Node* n = std::any_cast<Node*>(context->expr()->accept(this));
    return static_cast<Node*>(registry_.create<NegationNode>(n));
}

std::any ConvertorVisitor::visitExpression(ExprParser::ExpressionContext* context)
{
    return context->expr()->accept(this);
}

std::any ConvertorVisitor::visitComparison(ExprParser::ComparisonContext* context)
{
    Node* left = std::any_cast<Node*>(visit(context->expr(0)));
    Node* right = std::any_cast<Node*>(visit(context->expr(1)));

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
    Node* left = std::any_cast<Node*>(visit(context->expr(0)));
    Node* right = std::any_cast<Node*>(visit(context->expr(1)));

    std::string op = context->op->getText();
    return (op == "+") ? static_cast<Node*>(registry_.create<SumNode>(left, right))
                       : static_cast<Node*>(registry_.create<SubtractionNode>(left, right));
}

class NotImplemented: public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

// TODO implement this
std::any ConvertorVisitor::visitPortField([[maybe_unused]] ExprParser::PortFieldContext* context)
{
    throw NotImplemented("Node portfield not implemented yet");
}

std::any ConvertorVisitor::visitNumber(ExprParser::NumberContext* context)
{
    double d = stod(context->getText());
    return static_cast<Node*>(registry_.create<LiteralNode>(d));
}

// TODO implement this
std::any ConvertorVisitor::visitTimeIndex([[maybe_unused]] ExprParser::TimeIndexContext* context)
{
    throw NotImplemented("Node time index not implemented yet");
}

// TODO implement this
std::any ConvertorVisitor::visitTimeShift([[maybe_unused]] ExprParser::TimeShiftContext* context)
{
    throw NotImplemented("Node time shift not implemented yet");
}

// TODO implement this
std::any ConvertorVisitor::visitFunction([[maybe_unused]] ExprParser::FunctionContext* context)
{
    throw NotImplemented("Node function not implemented yet");
}

// TODO implement this
std::any ConvertorVisitor::visitTimeSum([[maybe_unused]] ExprParser::TimeSumContext* context)
{
    throw NotImplemented("Node time sum not implemented yet");
}

// TODO implement this
std::any ConvertorVisitor::visitAllTimeSum([[maybe_unused]] ExprParser::AllTimeSumContext* context)
{
    throw NotImplemented("Node all time sum  not implemented yet");
}

// shift related, not tested
std::any ConvertorVisitor::visitSignedAtom(ExprParser::SignedAtomContext* context)
{
    auto a = context->atom()->accept(this);
    if (context->op->getText() == "-")
    {
        return static_cast<Node*>(registry_.create<NegationNode>(std::any_cast<Node*>(a)));
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
    throw NotImplemented("Node right atom not implemented yet");
}

// TODO implement this
std::any ConvertorVisitor::visitShift([[maybe_unused]] ExprParser::ShiftContext* context)
{
    throw NotImplemented("Node shift not implemented yet");
}

// TODO implement this
std::any ConvertorVisitor::visitShiftAddsub(
  [[maybe_unused]] ExprParser::ShiftAddsubContext* context)
{
    throw NotImplemented("Node shift add sub not implemented yet");
}

// TODO implement this
std::any ConvertorVisitor::visitShiftMuldiv(
  [[maybe_unused]] ExprParser::ShiftMuldivContext* context)
{
    throw NotImplemented("Node shift mul div not implemented yet");
}

// TODO implement this
std::any ConvertorVisitor::visitRightMuldiv(
  [[maybe_unused]] ExprParser::RightMuldivContext* context)
{
    throw NotImplemented("Node right mul div not implemented yet");
}

// TODO implement this
std::any ConvertorVisitor::visitSignedExpression(
  [[maybe_unused]] ExprParser::SignedExpressionContext* context)
{
    throw NotImplemented("Node signed expression not implemented yet");
}

std::any ConvertorVisitor::visitRightExpression(ExprParser::RightExpressionContext* context)
{
    return context->expr()->accept(this);
}

} // namespace Antares::Solver::ModelConverter
