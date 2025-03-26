/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
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

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include "antares/expressions/RotateIndex.h"
#include "antares/expressions/ShiftVector.h"

namespace Antares::Expressions::Visitors
{
EvalVisitor::EvalVisitor(EvaluationContext context,
                         Optimisation::LinearProblemApi::FillContext fillContext):
    context_(std::move(context)),
    fillContext_(std::move(fillContext))
{
}

EvaluationResult EvalVisitor::visit(const Nodes::SumNode* node)
{
    auto operands = node->getOperands();
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

EvaluationResult EvalVisitor::visit(const Nodes::MultiplicationNode* node

)
{
    return dispatch(node->left()) * dispatch(node->right());
}

EvaluationResult EvalVisitor::visit(const Nodes::DivisionNode* node)
{
    return dispatch(node->left()) / dispatch(node->right());
}

EvaluationResult EvalVisitor::visit(const Nodes::EqualNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

EvaluationResult EvalVisitor::visit(const Nodes::LessThanOrEqualNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

EvaluationResult EvalVisitor::visit(const Nodes::GreaterThanOrEqualNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

EvaluationResult EvalVisitor::visit(const Nodes::VariableNode* node)
{
    return EvaluationResult{context_.getVariableValue(node->value())};
}

EvaluationResult EvalVisitor::visit(const Nodes::ParameterNode* node)
{
    if (node->timeIndex() == TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO)
    {
        return EvaluationResult{context_.getSystemParameterValueAsDouble(node->value())};
    }
    else
    {
        std::vector<double> params;
        params.reserve(fillContext_.getNumberOfTimestep());
        for (auto timeStep = fillContext_.getFirstTimeStep();
             timeStep <= fillContext_.getLastTimeStep();
             ++timeStep)
        {
            params.emplace_back(context_.getParameterValue(node->value(), "", 0, timeStep));
        }
        return EvaluationResult{params};
    }
}

EvaluationResult EvalVisitor::visit(const Nodes::LiteralNode* node)
{
    return

      EvaluationResult{node->value()};
}

EvaluationResult EvalVisitor::visit(const Nodes::NegationNode* node)
{
    return -dispatch(node->child());
}

EvaluationResult EvalVisitor::visit(const Nodes::PortFieldNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

EvaluationResult EvalVisitor::visit(const Nodes::PortFieldSumNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

EvaluationResult EvalVisitor::visit(const Nodes::ComponentVariableNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
}

EvaluationResult EvalVisitor::visit(const Nodes::ComponentParameterNode* node)
{
    throw EvalVisitorNotImplemented(name(), node->name());
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
    return expression.alltimeSum(fillContext_.getNumberOfTimestep());
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
    if (timeIndex < 0 || timeIndex >= vec.size())
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
