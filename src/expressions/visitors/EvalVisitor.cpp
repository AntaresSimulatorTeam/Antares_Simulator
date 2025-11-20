/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/expressions/visitors/EvalVisitor.h"

#include <numeric>
#include <stdexcept>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include "antares/expressions/ShiftVector.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"

namespace Antares::Expressions::Visitors
{

EvalVisitor::EvalVisitor(const Optimisation::OptimEntityContainer& optimContainer,
                         const Optimisation::LinearProblemApi::FillContext& fillContext,
                         const ModelerStudy::SystemModel::Component& component):
    // TODO put component or its id inside context, it is already component-bound.
    // Plus it is mandatory to visit Variables & PortFieldSums
    // Else, create a PostOptimEvalVisitor that inherits from EvalVisitor & has a different ctor
    optimContainer_(optimContainer),
    context_(optimContainer.getEvaluationContext(component)),
    fillContext_(fillContext),
    component_(component)
{
}

EvaluationResult EvalVisitor::visit(const Nodes::SumNode* node)
{
    const auto& operands = node->getOperands();
    return std::accumulate(std::begin(operands),
                           std::end(operands),
                           EvaluationResult{0.},
                           [this](const EvaluationResult& sum, const Nodes::Node* operand)
                           { return sum + dispatch(operand); });
}

EvaluationResult EvalVisitor::visit(const Nodes::SubtractionNode* node)
{
    return dispatch(node->left()) - dispatch(node->right());
}

EvaluationResult EvalVisitor::visit(const Nodes::MultiplicationNode* node)
{
    return dispatch(node->left()) * dispatch(node->right());
}

EvaluationResult EvalVisitor::visit(const Nodes::DivisionNode* node)
{
    return dispatch(node->left()) / dispatch(node->right());
}

EvaluationResult EvalVisitor::visit(const Nodes::EqualNode* node)
{
    return dispatch(node->left()) == dispatch(node->right());
}

EvaluationResult EvalVisitor::visit(const Nodes::LessThanOrEqualNode* node)
{
    return dispatch(node->left()) <= dispatch(node->right());
}

EvaluationResult EvalVisitor::visit(const Nodes::GreaterThanOrEqualNode* node)
{
    return dispatch(node->left()) >= dispatch(node->right());
}

EvaluationResult EvalVisitor::visit(const Nodes::VariableNode* node)
{
    if (node->timeIndex() == Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO
        || node->timeIndex() == Optimisation::TimeIndex::VARYING_IN_SCENARIO_ONLY)
    {
        const std::span componentVariables = optimContainer_.getComponentVariable(
          component_,
          node->Index(),
          1 /* single timestep*/);
        return EvaluationResult(componentVariables[0]->solutionValue());
    }
    // VARYING_IN_TIME_ONLY or VARYING_IN_TIME_AND_SCENARIO)
    const unsigned nbTimeStep = fillContext_.getLocalNumberOfTimeSteps();
    std::vector<double> varValues(nbTimeStep, 0.0);
    const std::span componentVariables = optimContainer_.getComponentVariable(component_,
                                                                              node->Index(),
                                                                              nbTimeStep);
    for (unsigned varInd = 0; varInd < nbTimeStep; ++varInd)
    {
        varValues[varInd] = componentVariables[varInd]->solutionValue();
    }

    return EvaluationResult{varValues};
}

EvaluationResult EvalVisitor::visit(const Nodes::ParameterNode* node)
{
    const auto systemParameter = context_.getParameter(node->value());
    if (node->timeIndex() == Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO
        && systemParameter.type != ModelerStudy::SystemModel::ParameterType::CONSTANT)
    {
        std::string msg = "Parameter " + node->value() + " is declared constant in time and"
                          + " scenario in library but not in system";
        throw std::invalid_argument(msg);
    }
    if (systemParameter.type == ModelerStudy::SystemModel::ParameterType::CONSTANT)
    {
        return EvaluationResult{context_.getSystemParameterValueAsDouble(node->value())};
    }
    std::vector<double> params;
    params.reserve(fillContext_.getLocalNumberOfTimeSteps());
    for (auto timeStep = fillContext_.getGlobalFirstTimeStep();
         timeStep <= fillContext_.getGlobalLastTimeStep();
         ++timeStep)
    {
        params.emplace_back(
          context_.getParameterValue(node->value(), fillContext_.getYear(), timeStep));
    }
    return EvaluationResult{params};
}

EvaluationResult EvalVisitor::visit(const Nodes::LiteralNode* node)
{
    return EvaluationResult{node->value()};
}

EvaluationResult EvalVisitor::visit(const Nodes::NegationNode* node)
{
    return -dispatch(node->child());
}

EvaluationResult EvalVisitor::visit(const Nodes::PortFieldNode* node)
{
    std::string portId = node->getPortName();
    std::string fieldId = node->getFieldName();

    const auto* nodeToVisit = component_.nodeAtPortField(portId, fieldId);
    return dispatch(nodeToVisit);
}

EvaluationResult EvalVisitor::visit(const Nodes::PortFieldSumNode* node)
{
    std::string portId = node->getPortName();
    std::string fieldId = node->getFieldName();

    EvaluationResult result(0.);
    for (const auto connectionEnd: component_.componentConnectionsViaPort(portId))
    {
        auto* component = connectionEnd.component();
        auto* port = connectionEnd.port();
        EvalVisitor visitor(optimContainer_, fillContext_, *component);
        const auto* nodeToVisit = component->nodeAtPortField(port->Id(), fieldId);
        auto dispatchResult = visitor.dispatch(nodeToVisit);
        result += dispatchResult;
    }
    return result;
}

EvaluationResult EvalVisitor::visit(const Nodes::TimeShiftNode* node)
{
    const auto ret = dispatch(node->left());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto timeShift = static_cast<int>(dispatch(node->right()).valueAsDouble());
    return ret.timeShift(timeShift);
}

EvaluationResult EvalVisitor::visit(const Nodes::TimeIndexNode* node)
{
    const auto ret = dispatch(node->left());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto timeIndex = static_cast<int>(dispatch(node->right()).valueAsDouble());
    return ret[timeIndex];
}

EvaluationResult EvalVisitor::visit(const Nodes::TimeSumNode* node)
{
    const auto expression = dispatch(node->expression());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto from = static_cast<int>(dispatch(node->from()).valueAsDouble());

    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto to = static_cast<int>(dispatch(node->to()).valueAsDouble());
    return expression.timeSum(from, to);
}

EvaluationResult EvalVisitor::visit(const Nodes::AllTimeSumNode* node)
{
    const EvaluationResult expression = dispatch(node->child());
    return expression.alltimeSum(fillContext_.getLocalNumberOfTimeSteps());
}

EvaluationResult EvalVisitor::visit(const Nodes::DualNode* node)
{
    const auto& [_, timeIndex] = optimContainer_.getConstraintData(component_, node->index());

    if (timeIndex == Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO
        || timeIndex == Optimisation::TimeIndex::VARYING_IN_SCENARIO_ONLY)
    {
        const auto componentConstraints = optimContainer_.getComponentConstraint(
          component_,
          node->index(),
          1 /* single timestep*/);
        return EvaluationResult(componentConstraints.first[0]->dual());
    }
    // VARYING_IN_TIME_ONLY or VARYING_IN_TIME_AND_SCENARIO)
    const unsigned nbTimeStep = fillContext_.getLocalNumberOfTimeSteps();
    std::vector<double> constraintValues(nbTimeStep, 0.0);
    const auto componentConstraints = optimContainer_.getComponentConstraint(component_,
                                                                             node->index(),
                                                                             nbTimeStep);
    for (unsigned constraintInd = 0; constraintInd < nbTimeStep; ++constraintInd)
    {
        constraintValues[constraintInd] = componentConstraints.first[constraintInd]->dual();
    }

    return EvaluationResult{constraintValues};
}

EvaluationResult EvalVisitor::visit(const Nodes::ReducedCostNode* node)
{
    if (node->timeIndex() == Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO
        || node->timeIndex() == Optimisation::TimeIndex::VARYING_IN_SCENARIO_ONLY)
    {
        const std::span componentVariables = optimContainer_.getComponentVariable(
          component_,
          node->index(),
          1 /* single timestep*/);
        return EvaluationResult(componentVariables[0]->reducedCost());
    }
    // VARYING_IN_TIME_ONLY or VARYING_IN_TIME_AND_SCENARIO)
    const unsigned nbTimeStep = fillContext_.getLocalNumberOfTimeSteps();
    std::vector<double> varValues(nbTimeStep, 0.0);
    const std::span componentVariables = optimContainer_.getComponentVariable(component_,
                                                                              node->index(),
                                                                              nbTimeStep);
    for (unsigned varInd = 0; varInd < nbTimeStep; ++varInd)
    {
        varValues[varInd] = componentVariables[varInd]->reducedCost();
    }
    return EvaluationResult{varValues};
}

std::string EvalVisitor::name() const
{
    return "EvalVisitor";
}

EvalVisitorDivisionException::EvalVisitorDivisionException(double left,
                                                           double right,
                                                           const std::string& message):
    std::runtime_error("DivisionNode: Error while evaluating : " + std::to_string(left) + "/"
                       + std::to_string(right) + " " + message)
{
}

EvalVisitorNotImplemented::EvalVisitorNotImplemented(const std::string& visitor,
                                                     const std::string& node):
    std::invalid_argument("Visitor" + visitor + " not implemented for node type " + node)
{
}

EvaluationResult::EvaluationResult(double value):
    value_(value)
{
}

EvaluationResult::EvaluationResult(const std::vector<double>& values):
    value_(values)
{
}

EvaluationResult::EvaluationResult(const std::variant<double, std::vector<double>>& value):
    value_(value)
{
}

EvaluationResult EvaluationResult::timeShift(int time_shift) const
{
    return EvaluationResult(
      std::visit([&time_shift](const auto& l) -> std::variant<double, std::vector<double>>
                 { return shift(l, time_shift); },
                 value_));
}

EvaluationResult EvaluationResult::timeSum(int from, int to) const
{
    EvaluationResult ret(0.);
    for (int shift = from; shift <= to; ++shift)
    {
        ret += timeShift(shift);
    }
    return ret;
}

EvaluationResult EvaluationResult::alltimeSum(int numberOfTimeStep) const
{
    EvaluationResult ret(0.);
    for (auto t = 0; t < numberOfTimeStep; ++t)
    {
        ret += operator[](t);
    }
    return ret;
}

EvaluationResult EvaluationResult::operator[](int timeIndex) const
{
    if (std::holds_alternative<double>(value_))
    {
        return *this;
    }
    const auto& vec = std::get<std::vector<double>>(value_);
    if (timeIndex < 0 || (unsigned)timeIndex >= vec.size())
    {
        throw EvalResultTimeIndexOutOfRange("timeIndex is out of range");
    }
    return EvaluationResult(vec.at(timeIndex));
}

std::vector<double> EvaluationResult::shift(const std::vector<double>& values, int timeShift)
{
    return shiftVector(values, timeShift);
}

} // namespace Antares::Expressions::Visitors
