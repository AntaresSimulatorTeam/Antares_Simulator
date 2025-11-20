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

#include <ExprVisitor.h>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/io/inputs/model-converter/convertorVisitor.h>
#include "antares/expressions/nodes/TimeSumNode.h"

#include "ExprLexer.h"
#include "ExprParser.h"

namespace Antares::IO::Inputs::ModelConverter
{

using namespace Antares::Expressions::Nodes;

/// Visitor to convert ANTLR expressions to Antares::Expressions::Nodes
class ConvertorVisitor final: public ExprVisitor
{
public:
    ConvertorVisitor(Expressions::Registry<Node>& registry, const YmlModel::Model& model);

    std::any visit(antlr4::tree::ParseTree* tree) override;

    Node* convertIdentifier(const std::string& identifier) const;
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
    std::any visitTimeShiftExpr(ExprParser::TimeShiftExprContext* context) override;
    std::any visitTimeIndexExpr(ExprParser::TimeIndexExprContext* context) override;
    std::any visitPortFieldExpr(ExprParser::PortFieldExprContext* context) override;
    std::any visitPortFieldSum(ExprParser::PortFieldSumContext* context) override;
    std::any visitDual(ExprParser::DualContext* context) override;
    std::any visitReducedCost(ExprParser::ReducedCostContext* context) override;

private:
    Expressions::Registry<Node>& registry_;
    const YmlModel::Model& model_;

    std::any buildShiftNode(Node* shifted_expr, ExprParser::ShiftContext* context);
    Node* NodeFromShiftContext(ExprParser::Shift_exprContext* shift_expr);
    PortFieldNode* processPortRule(ExprParser::PortFieldExprContext* context);
};

NoPortWithThisId::NoPortWithThisId(const std::string& name):
    runtime_error("No port found for this identifier: " + name)
{
}

Expressions::NodeRegistry convertExpressionToNode(const std::string& exprStr,
                                                  const YmlModel::Model& model)
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
    Expressions::Registry<Node> registry;
    ConvertorVisitor visitor(registry, model);
    auto root = std::any_cast<Node*>(visitor.visit(tree));
    return Expressions::NodeRegistry(root, std::move(registry));
}

ConvertorVisitor::ConvertorVisitor(Expressions::Registry<Node>& registry,
                                   const YmlModel::Model& model):
    registry_(registry),
    model_(model)
{
}

std::any ConvertorVisitor::visit(antlr4::tree::ParseTree* tree)
{
    return tree->accept(this);
}

class NoParameterOrVariableWithThisName final: public std::runtime_error
{
public:
    explicit NoParameterOrVariableWithThisName(const std::string& name):
        runtime_error("No parameter or variable found for this identifier: " + name)
    {
    }
};

class NoVariableWithThisName final: public std::runtime_error
{
public:
    explicit NoVariableWithThisName(const std::string& name):
        runtime_error("Model doesn't contain this variable in reduced_cost function: " + name)
    {
    }
};

class NoConstraintWithThisName final: public std::runtime_error
{
public:
    explicit NoConstraintWithThisName(const std::string& name):
        runtime_error("Model doesn't contain this constraint in dual function: " + name)
    {
    }
};

// to silent warning, convert bool to unsigned int
static constexpr unsigned int convertBool(bool in)
{
    return in ? 1 : 0;
}

static constexpr Optimisation::TimeIndex convertToTimeIndex(bool timedependent,
                                                            bool scenariodependent)
{
    return static_cast<Optimisation::TimeIndex>((convertBool(scenariodependent) << 1)
                                                | convertBool(timedependent));
}

Node* ConvertorVisitor::convertIdentifier(const std::string& identifier) const
{
    for (const auto& param: model_.parameters)
    {
        if (param.id == identifier)
        {
            return static_cast<Node*>(
              registry_.create<ParameterNode>(param.id,
                                              convertToTimeIndex(param.time_dependent,
                                                                 param.scenario_dependent)));
        }
    }

    const auto& variables = model_.variables;
    for (std::size_t index = 0; index < variables.size(); ++index)
    {
        const auto& var = variables[index];
        if (var.id == identifier)
        {
            return static_cast<Node*>(
              registry_.create<VariableNode>(var.id,
                                             index,
                                             convertToTimeIndex(var.time_dependent,
                                                                var.scenario_dependent)));
        }
    }
    throw NoParameterOrVariableWithThisName(identifier);
}

std::any ConvertorVisitor::visitIdentifier(ExprParser::IdentifierContext* context)
{
    return convertIdentifier(context->getText());
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

void extractSumOperands(Node* node, std::vector<Node*>& operands)
{
    if (auto* sumNode = dynamic_cast<SumNode*>(node))
    {
        for (auto* operand: sumNode->getOperands())
        {
            extractSumOperands(operand, operands);
        }
    }
    else
    {
        operands.emplace_back(node);
    }
}

std::any ConvertorVisitor::visitAddsub(ExprParser::AddsubContext* context)
{
    Node* left = std::any_cast<Node*>(visit(context->expr(0)));
    Node* right = std::any_cast<Node*>(visit(context->expr(1)));

    std::string op = context->op->getText();
    if (op == "-")
    {
        return static_cast<Node*>(registry_.create<SubtractionNode>(left, right));
    }
    // Sum node, flatten all operands in one vector if possible
    std::vector<Node*> operands;
    extractSumOperands(left, operands);
    extractSumOperands(right, operands);
    return static_cast<Node*>(registry_.create<SumNode>(operands));
}

class NotImplemented final: public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

static bool isThePortIsRegistered(const std::string& portId,
                                  const std::vector<YmlModel::Port>& ports)
{
    for (const auto& [id, _]: ports)
    {
        if (id == portId)
        {
            return true;
        }
    }
    return false;
}

PortFieldNode* ConvertorVisitor::processPortRule(ExprParser::PortFieldExprContext* context)
{
    const auto [portId, portField] = std::any_cast<std::pair<std::string, std::string>>(
      context->accept(this));

    if (isThePortIsRegistered(portId, model_.ports))
    {
        return registry_.create<PortFieldNode>(portId, portField);
    }
    throw NoPortWithThisId(portId);
}

std::any ConvertorVisitor::visitPortField(ExprParser::PortFieldContext* context)
{
    return static_cast<Node*>(processPortRule(context->portFieldExpr()));
}

std::any ConvertorVisitor::visitNumber(ExprParser::NumberContext* context)
{
    double d = stod(context->getText());
    return static_cast<Node*>(registry_.create<LiteralNode>(d));
}

std::any ConvertorVisitor::visitTimeIndex([[maybe_unused]] ExprParser::TimeIndexContext* context)
{
    Node* expr = convertIdentifier(context->IDENTIFIER()->getText());
    auto* index = registry_.create<LiteralNode>(std::stoi(context->expr()->getText()));
    return static_cast<Node*>(registry_.create<TimeIndexNode>(expr, index));
}

std::any ConvertorVisitor::buildShiftNode(Node* shifted_expr, ExprParser::ShiftContext* context)
{
    if (auto shift_maybe = context->shift_expr()) // [t+1], [t-1], etc.
    {
        Node* shift = std::any_cast<Node*>(shift_maybe->accept(this));
        return static_cast<Node*>(registry_.create<TimeShiftNode>(shifted_expr, shift));
    }
    else // [t]
    {
        return shifted_expr; // implicit conversion to std::any
    }
}

std::any ConvertorVisitor::visitTimeShift([[maybe_unused]] ExprParser::TimeShiftContext* context)
{
    return buildShiftNode(convertIdentifier(context->IDENTIFIER()->getText()), context->shift());
}

std::any ConvertorVisitor::visitTimeShiftExpr(ExprParser::TimeShiftExprContext* context)
{
    return buildShiftNode(std::any_cast<Node*>(context->expr()->accept(this)), context->shift());
}

std::any ConvertorVisitor::visitTimeIndexExpr(ExprParser::TimeIndexExprContext* context)
{
    Node* left = std::any_cast<Node*>(visit(context->expr(0)));
    Node* right = std::any_cast<Node*>(visit(context->expr(1)));
    return static_cast<Node*>(registry_.create<TimeIndexNode>(left, right));
}

std::any ConvertorVisitor::visitPortFieldExpr(ExprParser::PortFieldExprContext* context)
{
    return std::make_pair(context->IDENTIFIER()[0]->getText(), context->IDENTIFIER()[1]->getText());
}

std::any ConvertorVisitor::visitPortFieldSum(ExprParser::PortFieldSumContext* context)
{
    const auto port = processPortRule(context->portFieldExpr());
    const auto portName = port->getPortName();
    const auto fieldName = port->getFieldName();

    return static_cast<Node*>(registry_.create<PortFieldSumNode>(portName, fieldName));
}

std::any ConvertorVisitor::visitDual(ExprParser::DualContext* context)
{
    const std::string constraint_id = context->IDENTIFIER()->getText();
    unsigned index = 0;
    const auto search_constraint = [&](const auto& constraints) -> Node*
    {
        for (const auto& c: constraints)
        {
            if (c.id == constraint_id)
            {
                return static_cast<Node*>(registry_.create<DualNode>(c.id, index));
            }
            ++index;
        }
        return nullptr;
    };

    if (Node* node = search_constraint(model_.constraints))
    {
        return node;
    }
    if (Node* node = search_constraint(model_.binding_constraints))
    {
        return node;
    }

    throw NoConstraintWithThisName(constraint_id);
}

std::any ConvertorVisitor::visitReducedCost(ExprParser::ReducedCostContext* context)
{
    const auto& variables = model_.variables;
    for (std::size_t index = 0; index < variables.size(); ++index)
    {
        const auto& var = variables[index];
        if (var.id == context->IDENTIFIER()->getText())
        {
            return static_cast<Node*>(
              registry_.create<ReducedCostNode>(var.id,
                                                index,
                                                convertToTimeIndex(var.time_dependent,
                                                                   var.scenario_dependent)));
        }
    }

    throw NoVariableWithThisName(context->IDENTIFIER()->getText());
}

// TODO implement this
std::any ConvertorVisitor::visitFunction([[maybe_unused]] ExprParser::FunctionContext* context)
{
    throw NotImplemented("This function doesn't exist: " + context->IDENTIFIER()->getText());
}

Node* ConvertorVisitor::NodeFromShiftContext(ExprParser::Shift_exprContext* shift_expr)
{
    if (shift_expr)
    {
        return std::any_cast<Node*>(shift_expr->accept(this));
    }
    else
    {
        return registry_.create<LiteralNode>(0);
    }
}

std::any ConvertorVisitor::visitTimeSum([[maybe_unused]] ExprParser::TimeSumContext* context)
{
    auto* from = NodeFromShiftContext(context->from->shift_expr());

    auto* to = NodeFromShiftContext(context->to->shift_expr());

    auto* expr = std::any_cast<Node*>(context->expr()->accept(this));

    return static_cast<Node*>(registry_.create<TimeSumNode>(from, to, expr));
}

std::any ConvertorVisitor::visitAllTimeSum([[maybe_unused]] ExprParser::AllTimeSumContext* context)
{
    auto expr = std::any_cast<Node*>(context->expr()->accept(this));
    return static_cast<Node*>(registry_.create<AllTimeSumNode>(expr));
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
    return context->atom()->accept(this);
}

std::any ConvertorVisitor::visitShift([[maybe_unused]] ExprParser::ShiftContext* context)
{
    return std::any_cast<Node*>(visit(context->shift_expr()));
}

std::any ConvertorVisitor::visitShiftAddsub(
  [[maybe_unused]] ExprParser::ShiftAddsubContext* context)
{
    Node* left = std::any_cast<Node*>(visit(context->shift_expr()));
    Node* right = std::any_cast<Node*>(visit(context->right_expr()));
    const auto op = context->op->getText();
    return (op == "+") ? static_cast<Node*>(registry_.create<SumNode>(left, right))
                       : static_cast<Node*>(registry_.create<SubtractionNode>(left, right));
}

// TODO implement this
std::any ConvertorVisitor::visitShiftMuldiv(
  [[maybe_unused]] ExprParser::ShiftMuldivContext* context)
{
    Node* left = std::any_cast<Node*>(visit(context->shift_expr()));
    Node* right = std::any_cast<Node*>(visit(context->right_expr()));
    const auto op = context->op->getText();
    return (op == "*") ? static_cast<Node*>(registry_.create<MultiplicationNode>(left, right))
                       : static_cast<Node*>(registry_.create<DivisionNode>(left, right));
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
    auto a = context->expr()->accept(this);
    if (context->op->getText() == "-")
    {
        return static_cast<Node*>(registry_.create<NegationNode>(std::any_cast<Node*>(a)));
    }
    return a;
}

std::any ConvertorVisitor::visitRightExpression(ExprParser::RightExpressionContext* context)
{
    return context->expr()->accept(this);
}

} // namespace Antares::IO::Inputs::ModelConverter
