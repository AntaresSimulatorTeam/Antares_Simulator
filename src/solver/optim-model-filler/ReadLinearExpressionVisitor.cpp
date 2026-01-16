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

#include <cmath>

#include <antares/expressions/visitors/NodeVisitor.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/solver/optim-model-filler/TimeDependentLinearExpression.h>
#include "antares/exception/InvalidArgumentError.hpp"
#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/expressions/visitors/EvalVisitor.h"
#include "antares/expressions/visitors/HelpVisitNode.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/study/system-model/component.h"

/**
 * Read Linear Expression Visitor
 * Visits a Node and produces a Linear Expression (defined by an offset and non-zero
 * coefficients of variables)
 * Comparison Nodes are not allowed
 */
using namespace Antares::Expressions;
using namespace Antares::Optimization;

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

TimeDependentLinearExpression ReadLinearExpressionVisitor::visitMergeDuplicates(
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

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const Nodes::SumNode* node)
{
    const auto& operands = node->getOperands();
    TimeDependentLinearExpression ret(nbtimeSteps_);
    for (auto* operand: operands)
    {
        ret += dispatch(operand);
    }
    return ret;
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const Nodes::SubtractionNode* node)
{
    auto ret = dispatch(node->left());
    ret -= dispatch(node->right());
    return ret;
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::MultiplicationNode* node)
{
    auto ret = dispatch(node->left());
    ret *= dispatch(node->right());
    return ret;
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const Nodes::DivisionNode* node)
{
    return dispatch(node->left()) / dispatch(node->right());
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const Nodes::EqualNode*)
{
    throw Error::InvalidArgumentError("A linear expression can't contain comparison operators.");
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const Nodes::LessThanOrEqualNode*)
{
    throw Error::InvalidArgumentError("A linear expression can't contain comparison operators.");
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::GreaterThanOrEqualNode*)
{
    throw Error::InvalidArgumentError("A linear expression can't contain comparison operators.");
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const Nodes::NegationNode* node)
{
    auto ret = dispatch(node->child());
    return -ret;
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const Nodes::VariableNode* node)
{
    const auto variableStart = optimEntityContainer_.getVariableStartColumn(component_,
                                                                            node->Index());
    if (isTimeConstant(node->variability()))
    {
        return TimeDependentLinearExpression({{variableStart, 1.}}, 0.);
    }

    // At this point, VariableNode is time dependent (scenario not handled)
    TimeDependentLinearExpression out(nbtimeSteps_);
    auto variableIndex = variableStart;
    for (unsigned ts = 0; ts < nbtimeSteps_; ts++)
    {
        out[ts].addVariable(variableIndex, 1);
        ++variableIndex;
    }
    return out;
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const Nodes::ParameterNode* node)
{
    const auto systemParameter = evalContext_.getParameter(node->value());
    if (systemParameter.type == VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO)
    {
        double value = evalContext_.getSystemParameterValueAsDouble(node->value());

        return TimeDependentLinearExpression({}, value);
    }
    if (systemParameter.type == VariabilityType::VARYING_IN_SCENARIO_ONLY)
    {
        double value = evalContext_.getParameterValue(node->value(), fillContext_.getYear(), 0);
        return TimeDependentLinearExpression({}, value);
    }
    // only dependent

    // assume global nb timeStep == nbtimeSteps
    const auto& parameters = evalContext_.getParameterValue(node->value(),
                                                            fillContext_.getYear(),
                                                            fillContext_.getGlobalFirstTimeStep(),
                                                            fillContext_.getGlobalLastTimeStep());

    return TimeDependentLinearExpression(parameters);
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const Nodes::LiteralNode* node)
{
    return TimeDependentLinearExpression({}, node->value());
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const Nodes::PortFieldNode*)
{
    throw Error::InvalidArgumentError("ReadLinearExpressionVisitor cannot visit PortFieldNodes");
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(
  const Nodes::PortFieldSumNode* node)
{
    auto& portId = node->getPortName();
    auto& fieldId = node->getFieldName();

    TimeDependentLinearExpression to_return(nbtimeSteps_);

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

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const Nodes::TimeShiftNode* node)
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

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const Nodes::TimeIndexNode* node)
{
    auto expression = dispatch(node->left());

    if (expression.isConstant())
    {
        return expression;
    }
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue]
    const auto timeIndex = static_cast<int>(evalVisitor_.dispatch(node->right()).valueAsDouble());
    return TimeDependentLinearExpression(std::move(expression[timeIndex]));
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const Nodes::TimeSumNode* node)
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

    TimeDependentLinearExpression ret(nbtimeSteps_);
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

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const Nodes::AllTimeSumNode* node)
{
    LinearExpression ret; // Constant expr
    auto expr = dispatch(node->child());
    for (auto& s: expr)
    {
        ret += s;
    }
    return TimeDependentLinearExpression(std::move(ret));
}

TimeDependentLinearExpression visitReducedCost(const Nodes::FunctionNode*)
{
    throw Error::InvalidArgumentError(
      "A linear expression can't contain extra output operator reduced_cost.");
}

TimeDependentLinearExpression visitDual(const Nodes::FunctionNode*)
{
    throw Error::InvalidArgumentError(
      "A linear expression can't contain extra output operator dual.");
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visitPower(
  const Nodes::FunctionNode* node)
{
    auto to_return(dispatch(node->getOperands()[0]));
    auto exponentExpr = dispatch(node->getOperands()[1]);
    if (exponentExpr.size() != 1)
    {
        throw Error::InvalidArgumentError("exponent must be constant");
    }

    const auto& exponent = exponentExpr[0];
    for (auto& s: to_return)
    {
        s ^= exponent;
    }
    return to_return;
}

auto checkIsConstant = [](const std::string& op, const auto& expression)
{
    if (expression.hasCoefs())
    {
        std::string err_msg = op + " operator: its argument is not constant, but has to be.";
        throw std::invalid_argument(err_msg);
    }
};

auto checkExpressionIsConstantForFloor = [](const auto& expr) { checkIsConstant("floor", expr); };
auto checkExpressionIsConstantForCeil = [](const auto& expr) { checkIsConstant("ceil", expr); };

auto floorExpression = [](auto& expr) { expr.constant(std::floor(expr.constant())); };
auto ceilExpression = [](auto& expr) { expr.constant(std::ceil(expr.constant())); };

TimeDependentLinearExpression ReadLinearExpressionVisitor::visitFloor(
  const Nodes::FunctionNode* node)
{
    auto expressions = dispatch(node->getOperands()[0]);
    std::ranges::for_each(expressions, checkExpressionIsConstantForFloor);
    std::ranges::for_each(expressions, floorExpression);
    return expressions;
}

TimeDependentLinearExpression ReadLinearExpressionVisitor::visitCeil(
  const Nodes::FunctionNode* node)
{
    auto expressions = dispatch(node->getOperands()[0]);
    std::ranges::for_each(expressions, checkExpressionIsConstantForCeil);
    std::ranges::for_each(expressions, ceilExpression);
    return expressions;
}

auto max_element_of_vector = [](const auto& v) { return *std::ranges::max_element(v); };
auto min_element_of_vector = [](const auto& v) { return *std::ranges::min_element(v); };

TimeDependentLinearExpression ReadLinearExpressionVisitor::visit(const Nodes::FunctionNode* node)
{
    switch (node->type())
    {
    case Nodes::FunctionNodeType::reduced_cost:
        return visitReducedCost(node);
    case Nodes::FunctionNodeType::dual:
        return visitDual(node);
    case Nodes::FunctionNodeType::max:
        return applyOperation(visitChildrenNodes(node), max_element_of_vector);
    case Nodes::FunctionNodeType::min:
        return applyOperation(visitChildrenNodes(node), min_element_of_vector);
    case Nodes::FunctionNodeType::pow:
        return visitPower(node);
    case Nodes::FunctionNodeType::floor:
        return visitFloor(node);
    case Nodes::FunctionNodeType::ceil:
        return visitCeil(node);
    default:
        throw std::runtime_error("Function " + node->typeToString() + " is not implemented.");
    }
}

} // Namespace Antares::Optimisation
