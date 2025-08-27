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

#include <numeric>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvaluationContext.h>
#include <antares/expressions/visitors/NodeVisitor.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/solver/optim-model-filler/TimeDependentLinearExpression.h>
#include "antares/expressions/visitors/EvalVisitor.h"
#include "antares/optimisation/linear-problem-api/IScenario.h"
#include "antares/optimisation/linear-problem-api/mipVariable.h"
#include "antares/solver/optim-model-filler/linearTypes.h"
#include "antares/study/system-model/component.h"

using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;
using namespace Antares::ModelerStudy;

/**
 * Read Linear Expression Visitor
 * Visits a Node and produces a Linear Expression (defined by an offset and non-zero
 * coefficients of variables)
 * Comparison Nodes are not allowed
 */
namespace Antares::Optimization
{
template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
class VariableDictionary;

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
class ReadLinearExpressionVisitor
    : public Expressions::Visitors::NodeVisitor<TimeDependentLinearExpression>
{
public:
    explicit ReadLinearExpressionVisitor(
      Expressions::Visitors::EvaluationContext evalContext,
      Optimisation::LinearProblemApi::FillContext fillContext,
      const Antares::ModelerStudy::SystemModel::Component& component,
      const VariableDictionary<InnerSolverVariable>& variable_dictionary);

    ReadLinearExpressionVisitor() = default;
    std::string name() const override;

private:
    TimeDependentLinearExpression visit(const Expressions::Nodes::SumNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::SubtractionNode* node) override;
    TimeDependentLinearExpression visit(
      const Expressions::Nodes::MultiplicationNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::DivisionNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::EqualNode* node) override;
    TimeDependentLinearExpression visit(
      const Expressions::Nodes::LessThanOrEqualNode* node) override;
    TimeDependentLinearExpression visit(
      const Expressions::Nodes::GreaterThanOrEqualNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::NegationNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::VariableNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::ParameterNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::LiteralNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::PortFieldNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::PortFieldSumNode* node) override;
    TimeDependentLinearExpression visit(
      const Expressions::Nodes::ComponentVariableNode* node) override;
    TimeDependentLinearExpression visit(
      const Expressions::Nodes::ComponentParameterNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::TimeShiftNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::TimeIndexNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::TimeSumNode* node) override;
    TimeDependentLinearExpression visit(const Expressions::Nodes::AllTimeSumNode* node) override;

    Optimisation::LinearProblemApi::FillContext fillContext_;
    const Expressions::Visitors::EvaluationContext evalContext_;
    const Antares::ModelerStudy::SystemModel::Component& component_;
    Expressions::Visitors::EvalVisitor evalVisitor_;
    const VariableDictionary<InnerSolverVariable>& dictionary_;
};

// Implémentation template

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
ReadLinearExpressionVisitor<InnerSolverVariable>::ReadLinearExpressionVisitor(
  EvaluationContext evalContext,
  Optimisation::LinearProblemApi::FillContext fillContext,
  const SystemModel::Component& component,
  const VariableDictionary<InnerSolverVariable>& variable_dictionary):
    fillContext_(std::move(fillContext)),
    evalContext_(std::move(evalContext)),
    component_(component),
    evalVisitor_(evalContext_, fillContext_),
    dictionary_(variable_dictionary)
{
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
std::string ReadLinearExpressionVisitor<InnerSolverVariable>::name() const
{
    return "ReadLinearExpressionVisitor";
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const SumNode* node)
{
    const auto& operands = node->getOperands();
    TimeDependentLinearExpression ret(fillContext_);
    for (auto* operand: operands)
    {
        ret += dispatch(operand);
    }
    return ret;
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const SubtractionNode* node)
{
    return dispatch(node->left()) - dispatch(node->right());
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const MultiplicationNode* node)
{
    return dispatch(node->left()) * dispatch(node->right());
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const DivisionNode* node)
{
    return dispatch(node->left()) / dispatch(node->right());
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const EqualNode*)
{
    throw std::invalid_argument("A linear expression can't contain comparison operators.");
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const LessThanOrEqualNode*)
{
    throw std::invalid_argument("A linear expression can't contain comparison operators.");
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const GreaterThanOrEqualNode*)
{
    throw std::invalid_argument("A linear expression can't contain comparison operators.");
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const NegationNode* node)
{
    return -dispatch(node->child());
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const VariableNode* node)
{
    if (node->timeIndex() == TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO)
    {
        FullKey key = dictionary_.buildFullKey(component_.Id(),
                                               node->value(),
                                               MCYearAndTime::MCYear{fillContext_.getYear()});
        return TimeDependentLinearExpression(fillContext_, LinearExpression(0, {{key, 1}}));
    }

    // only dependent
    LinearExpressionMap linearExpressions;

    for (unsigned int timeStep = fillContext_.getLocalFirstTimeStep();
         timeStep <= fillContext_.getLocalLastTimeStep();
         ++timeStep)
    {
        FullKey key = dictionary_.buildFullKey(component_.Id(),
                                               node->value(),
                                               MCYearAndTime::MCYear{fillContext_.getYear()},
                                               timeStep);
        linearExpressions[timeStep] = LinearExpression(0, {{key, 1}});
    }
    return TimeDependentLinearExpression(fillContext_, linearExpressions);
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const ParameterNode* node)
{
    const auto systemParameter = evalContext_.getParameter(node->value());
    if (node->timeIndex() == TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO
        && systemParameter.type != ParameterType::CONSTANT)
    {
        throw std::invalid_argument(
          "Parameter " + node->value()
          + " is declared constant in time and scenario in library but not in system");
    }
    if (systemParameter.type == ParameterType::CONSTANT)
    {
        return TimeDependentLinearExpression(
          fillContext_,
          LinearExpression(evalContext_.getSystemParameterValueAsDouble(node->value()), {}));
    }
    // only dependent
    LinearExpressionMap linearExpressions;

    int idx = 0;
    for (auto localTimeStep = fillContext_.getLocalFirstTimeStep();
         localTimeStep <= fillContext_.getLocalLastTimeStep();
         ++localTimeStep)
    {
        auto globalTimeStep = fillContext_.getGlobalFirstTimeStep() + idx;
        linearExpressions[localTimeStep] = LinearExpression(
          evalContext_.getParameterValue(node->value(), fillContext_.getYear(), globalTimeStep),
          {});
        idx++;
    }
    return TimeDependentLinearExpression(fillContext_, linearExpressions);
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const LiteralNode* node)
{
    return TimeDependentLinearExpression(fillContext_, LinearExpression(node->value(), {}));
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const PortFieldNode*)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit PortFieldNodes");
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const PortFieldSumNode* node)
{
    auto& portId = node->getPortName();
    auto& fieldId = node->getFieldName();

    TimeDependentLinearExpression to_return(fillContext_);
    for (const auto connexion_end: component_.componentConnectionsViaPort(portId))
    {
        auto* component = connexion_end.component();
        auto* port = connexion_end.port();

        const EvaluationContext connectedComponentEvalContext(component->getParameterValues(),
                                                              {},
                                                              evalContext_.data(),
                                                              evalContext_.scenario());
        ReadLinearExpressionVisitor<InnerSolverVariable> visitor(connectedComponentEvalContext,
                                                                 fillContext_,
                                                                 *component,
                                                                 dictionary_);

        const Node* node = component->nodeAtPortField(port->Id(), fieldId);
        to_return += visitor.dispatch(node);
    }

    return to_return;
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const ComponentVariableNode*)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit ComponentVariableNodes");
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const ComponentParameterNode*)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit ComponentParameterNodes");
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const TimeShiftNode* node)
{
    const auto ret = dispatch(node->left());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto timeShift = static_cast<int>(evalVisitor_.dispatch(node->right()).valueAsDouble());
    return ret.shiftLinearExpressions(timeShift);
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const TimeIndexNode* node)
{
    const auto ret = dispatch(node->left());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto timeIndex = static_cast<int>(evalVisitor_.dispatch(node->right()).valueAsDouble());
    return ret[timeIndex];
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const TimeSumNode* node)
{
    const auto expression = dispatch(node->expression());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto from = static_cast<int>(evalVisitor_.dispatch(node->from()).valueAsDouble());

    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto to = static_cast<int>(evalVisitor_.dispatch(node->to()).valueAsDouble());

    return expression.timeSumLinearExpressions(from, to);
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
TimeDependentLinearExpression ReadLinearExpressionVisitor<InnerSolverVariable>::visit(
  const AllTimeSumNode* node)
{
    const auto expression = dispatch(node->child());
    return expression.allTimeSumLinearExpressions();
}

} // namespace Antares::Optimization
