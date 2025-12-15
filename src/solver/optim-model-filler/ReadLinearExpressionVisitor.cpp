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

#include "antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h"

#include <antares/expressions/visitors/NodeVisitor.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/solver/optim-model-filler/TimeDependentLinearExpression.h>
#include "antares/exception/InvalidArgumentError.hpp"
#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/expressions/visitors/EvalVisitor.h"
#include "antares/expressions/visitors/VariadicNodeFunctionVisit.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/study/system-model/component.h"

/**
 * Read Linear Expression Visitor
 * Visits a Node and produces a Linear Expression (defined by an offset and non-zero
 * coefficients of variables)
 * Comparison Nodes are not allowed
 */
using namespace Antares::Expressions;

namespace Antares::Optimisation
{

ReadLinearExpressionVisitor::ReadLinearExpressionVisitor(
  const OptimEntityContainer& optimEntityContainer,
  const LinearProblemApi::FillContext& fillContext,
  const ModelerStudy::SystemModel::Component& component):
    optimEntityContainer_(optimEntityContainer),
    component_(component),
    evalContext_(optimEntityContainer.getEvaluationContext(component)),
    fillContext_(fillContext),
    evalVisitor_(optimEntityContainer, fillContext, component),
    nbtimeSteps_(fillContext.getLocalNumberOfTimeSteps())
{
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visitMergeDuplicates(
  const Nodes::Node* node)
{
    auto expr = dispatch(node);
    expr.mergeDuplicateCoefficients();
    return expr;
}

std::string ReadLinearExpressionVisitor::name() const
{
    return "ReadLinearExpressionVisitor";
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::SumNode* node)
{
    const auto& operands = node->getOperands();
    Optimization::TimeDependentLinearExpression ret(nbtimeSteps_);
    for (auto* operand: operands)
    {
        ret += dispatch(operand);
    }
    return ret;
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::SubtractionNode* node)
{
    auto ret = dispatch(node->left());
    ret -= dispatch(node->right());
    return ret;
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::MultiplicationNode* node)
{
    auto ret = dispatch(node->left());
    ret *= dispatch(node->right());
    return ret;
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::DivisionNode* node)
{
    return dispatch(node->left()) / dispatch(node->right());
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::EqualNode*)
{
    throw Error::InvalidArgumentError("A linear expression can't contain comparison operators.");
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::LessThanOrEqualNode*)
{
    throw Error::InvalidArgumentError("A linear expression can't contain comparison operators.");
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::GreaterThanOrEqualNode*)
{
    throw Error::InvalidArgumentError("A linear expression can't contain comparison operators.");
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::NegationNode* node)
{
    auto ret = dispatch(node->child());
    return -ret;
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::VariableNode* node)
{
    const auto variableStart = optimEntityContainer_.getVariableStartColumn(component_,
                                                                            node->Index());
    if (isTimeConstant(node->variability()))
    {
        return Optimization::TimeDependentLinearExpression({{variableStart, 1.}}, 0.);
    }
    if (isTimeDependent(node->variability())) /* scenario not
                                                                              handled !*/
    {
        Optimization::TimeDependentLinearExpression out(nbtimeSteps_);

        auto variableIndex = variableStart;
        for (int ts = 0; ts < nbtimeSteps_; ts++)
        {
            out[ts].addVariable(variableIndex, 1);
            ++variableIndex;
        }
        return out;
    }
    throw Error::InvalidArgumentError(
      "the support of scenario dependent variables is not available for now");
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::ParameterNode* node)
{
    const auto systemParameter = evalContext_.getParameter(node->value());
    if (systemParameter.type == VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO)
    {
        double value = evalContext_.getSystemParameterValueAsDouble(node->value());

        return Optimization::TimeDependentLinearExpression({}, value);
    }
    if (systemParameter.type == VariabilityType::VARYING_IN_SCENARIO_ONLY)
    {
        double value = evalContext_.getParameterValue(node->value(), fillContext_.getYear(), 0);
        return Optimization::TimeDependentLinearExpression({}, value);
    }
    // only dependent

    // assume global nb timeStep == nbtimeSteps
    const auto& parameters = evalContext_.getParameterValue(node->value(),
                                                            fillContext_.getYear(),
                                                            fillContext_.getGlobalFirstTimeStep(),
                                                            fillContext_.getGlobalLastTimeStep());

    return Optimization::TimeDependentLinearExpression(parameters);
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::LiteralNode* node)
{
    return Optimization::TimeDependentLinearExpression({}, node->value());
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::PortFieldNode*)
{
    throw Error::InvalidArgumentError("ReadLinearExpressionVisitor cannot visit PortFieldNodes");
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::PortFieldSumNode* node)
{
    auto& portId = node->getPortName();
    auto& fieldId = node->getFieldName();

    Optimization::TimeDependentLinearExpression to_return(nbtimeSteps_);

    for (const auto connexion_end: component_.componentConnectionsViaPort(portId))
    {
        auto* component = connexion_end.component();
        auto* port = connexion_end.port();

        ReadLinearExpressionVisitor visitor(optimEntityContainer_, fillContext_, *component);

        const Nodes::Node* node = component->nodeAtPortField(port->Id(), fieldId);
        to_return += visitor.dispatch(node);
    }

    return to_return;
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::TimeShiftNode* node)
{
    auto expression = dispatch(node->left());
    if (expression.isConstant())
    {
        return expression;
    }
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue]
    const auto timeIndex = static_cast<int>(evalVisitor_.dispatch(node->right()).valueAsDouble());
    expression.rotate(timeIndex);
    return expression;
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::TimeIndexNode* node)
{
    auto expression = dispatch(node->left());

    if (expression.isConstant())
    {
        return expression;
    }
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue]
    const auto timeIndex = static_cast<int>(evalVisitor_.dispatch(node->right()).valueAsDouble());
    return Optimization::TimeDependentLinearExpression(std::move(expression[timeIndex]));
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::TimeSumNode* node)
{
    auto expression = dispatch(node->expression());

    const auto from = static_cast<int>(evalVisitor_.dispatch(node->from()).valueAsDouble());
    const auto to = static_cast<int>(evalVisitor_.dispatch(node->to()).valueAsDouble());

    if (expression.isConstant())
    {
        // example from=0, to=2 => length({0, 1, 2}) = 3
        expression *= static_cast<double>(to - from + 1);
        return expression;
    }

    Optimization::TimeDependentLinearExpression ret(nbtimeSteps_);
    expression.rotate(from);
    for (int t = from; t <= to; ++t)
    {
        ret += expression;
        if (t < to)
        {
            expression.rotate(1);
        }
    }
    return ret;
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::AllTimeSumNode* node)
{
    Optimization::LinearExpression ret; // Constant expr
    auto expr = dispatch(node->child());
    for (auto& s: expr)
    {
        ret += s;
    }
    return Optimization::TimeDependentLinearExpression(std::move(ret));
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::handleReducedCost(
  const Nodes::FunctionNode*)
{
    throw Error::InvalidArgumentError(
      "A linear expression can't contain extra output operator reduced_cost.");
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::handleDual(
  const Nodes::FunctionNode*)
{
    throw Error::InvalidArgumentError(
      "A linear expression can't contain extra output operator dual.");
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::handlePow(
  const Nodes::FunctionNode* node)
{
    auto ret(dispatch(node->getOperands().front()));
    auto exponentExpr = dispatch(node->getOperands().at(1));
    if (exponentExpr.size() != 1)
    {
        throw Error::InvalidArgumentError("exponent must be constant");
    }
    const auto& exponent = exponentExpr[0];
    for (auto& s: ret)
    {
        s ^= exponent;
    }
    return ret;
}

Optimization::TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::FunctionNode* node)
{
    switch (node->type())
    {
    case Nodes::FunctionNodeType::reduced_cost:
        return handleReducedCost(node);
    case Nodes::FunctionNodeType::dual:
        return handleDual(node);
    case Nodes::FunctionNodeType::max:
    {
        auto exprs(Visitors::variadicFunction(*this, node));
        return applyOperation(exprs,
                              [](const auto& elements)
                              { return *std::max_element(elements.begin(), elements.end()); });
    }
    case Nodes::FunctionNodeType::min:
    {
        auto exprs(Visitors::variadicFunction(*this, node));
        return applyOperation(exprs,
                              [](const auto& elements)
                              { return *std::min_element(elements.begin(), elements.end()); });
    }
    case Nodes::FunctionNodeType::pow:
        return handlePow(node);
    default:
        throw std::runtime_error("Function " + node->typeToString() + " is not implemented.");
    }
}

} // Namespace Antares::Optimisation
