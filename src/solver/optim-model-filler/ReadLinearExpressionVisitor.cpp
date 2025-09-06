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

#include <numeric>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvaluationContext.h>
#include <antares/expressions/visitors/NodeVisitor.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h>
#include "antares/expressions/RotateIndex.h"
#include "antares/study/system-model/component.h"
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;
using namespace Antares::ModelerStudy;

namespace Antares::Optimization
{
ReadLinearExpressionVisitor::ReadLinearExpressionVisitor(
  EvaluationContext evalContext,
  Optimisation::LinearProblemApi::FillContext fillContext,
  const SystemModel::Component& component,
  unsigned int nbModelVariables,
  const std::vector<unsigned int>& variableStartColumn):
    fillContext_(std::move(fillContext)),
    evalContext_(std::move(evalContext)),
    component_(component),
    evalVisitor_(evalContext_, fillContext_),
    nbModelVariables_(nbModelVariables),
    variableStartColumn_(variableStartColumn)
{
    nbtimeSteps_ = fillContext_.getLocalLastTimeStep() - fillContext_.getLocalFirstTimeStep() + 1;
}

std::string ReadLinearExpressionVisitor::name() const
{
    return "ReadLinearExpressionVisitor";
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const SumNode* node)
{
    const auto& operands = node->getOperands();
    LinearExpressionEigen ret(nbtimeSteps_, nbModelVariables_);
    for (auto* operand: operands)
    {
        ret += dispatch(operand);
    }
    return ret;
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const SubtractionNode* node)
{
    auto ret = dispatch(node->left());
    ret -= dispatch(node->right()); // using -= operator avoid expensive copy
    return ret;
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const MultiplicationNode* node)
{
    auto ret = dispatch(node->left());
    ret *= dispatch(node->right());
    return ret;
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const DivisionNode* node)
{
    auto ret = dispatch(node->left());
    ret /= dispatch(node->right());
    return ret;
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const EqualNode*)
{
    throw std::invalid_argument("A linear expression can't contain comparison operators.");
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const LessThanOrEqualNode*)
{
    throw std::invalid_argument("A linear expression can't contain comparison operators.");
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const GreaterThanOrEqualNode*)
{
    throw std::invalid_argument("A linear expression can't contain comparison operators.");
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const NegationNode* node)
{
    return -dispatch(node->child());
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const VariableNode* node)
{
    const auto& variables = component_.getModel()->Variables();
    const auto& it = std::ranges::find_if(variables,
                                          [&node](const auto& variable)
                                          { return variable.Id() == node->value(); });
    if (it == variables.end())
    {
        throw std::invalid_argument("Variable (" + node->value() + ") not found.");
    }
    LinearExpressionEigen out(nbtimeSteps_, nbModelVariables_);

    const auto& modelVariablesGlobalIndices = component_.ModelVariablesGlobalIndices();
    const auto globalIndex = modelVariablesGlobalIndices.at(std::distance(variables.begin(), it));
    const auto variableStart = variableStartColumn_.at(globalIndex);
    const auto variableEnd = variableStart == *variableStartColumn_.rbegin()
                               ? nbModelVariables_
                               : variableStartColumn_.at(globalIndex + 1);

    if (node->timeIndex() == TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO)
    {
        for (auto localTimeStep = fillContext_.getLocalFirstTimeStep();
             localTimeStep < fillContext_.getLocalLastTimeStep();
             ++localTimeStep)
        {
            out.addCoeff(localTimeStep, variableStart, 1);
        }
    }
    // else time-dep only hanled    //  check if var is time-dep then nbTimeStep == variableEnd -
    // variableStart+1
    if (node->timeIndex() == TimeIndex::VARYING_IN_TIME_ONLY
        || node->timeIndex() == TimeIndex::VARYING_IN_TIME_AND_SCENARIO) /* scenario not handled !*/
    {
        for (auto localTimeStep = fillContext_.getLocalFirstTimeStep();
             localTimeStep < fillContext_.getLocalLastTimeStep();
             ++localTimeStep)
        {
            for (auto variableIndex(variableStart); variableIndex < variableEnd; ++variableIndex)
            {
                out.addCoeff(localTimeStep, variableIndex, 1);
            }
        }
    }
    else
    {
        throw std::invalid_argument(
          "the support of scenario dependent variables is not available for now :(" + node->value()
          + ").");
    }
    return out;
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const ParameterNode* node)
{
    const auto systemParameter = evalContext_.getParameter(node->value());
    if (node->timeIndex() == TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO
        && systemParameter.type != ParameterType::CONSTANT)
    {
        throw std::invalid_argument(
          "Parameter " + node->value()
          + " is declared constant in time and scenario in library but not in system");
    }

    // TODO

    LinearExpressionEigen out(nbtimeSteps_, nbModelVariables_);
    if (systemParameter.type == ParameterType::CONSTANT)
    {
        out.setOffset(
          Eigen::VectorXd::Constant(nbtimeSteps_,
                                    evalContext_.getSystemParameterValueAsDouble(node->value())));
        return out;
    }
    // only dependent

    int idx = 0;
    // assume glo nb timeStep == nbtimeSteps
    const auto& parmeters = evalContext_.getParameterValue(node->value(),
                                                           fillContext_.getYear(),
                                                           fillContext_.getGlobalFirstTimeStep(),
                                                           fillContext_.getGlobalLastTimeStep());
    out.setOffset(
      {nbtimeSteps_, Eigen::Map<const Eigen::VectorXd>(parmeters.data(), parmeters.size())});

    return out;
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const LiteralNode* node)
{
    // TODO
    LinearExpressionEigen out(nbtimeSteps_, nbModelVariables_);
    out.setOffset(Eigen::VectorXd::Constant(nbtimeSteps_, node->value()));
    return out;
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const PortFieldNode*)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit PortFieldNodes");
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const PortFieldSumNode* node)
{
    auto& portId = node->getPortName();
    auto& fieldId = node->getFieldName();

    LinearExpressionEigen to_return(nbtimeSteps_, nbModelVariables_);
    for (const auto connexion_end: component_.componentConnectionsViaPort(portId))
    {
        auto* component = connexion_end.component();
        auto* port = connexion_end.port();

        const EvaluationContext connectedComponentEvalContext(component->getParameterValues(),
                                                              {},
                                                              evalContext_.data(),
                                                              evalContext_.scenario());
        ReadLinearExpressionVisitor visitor(connectedComponentEvalContext,
                                            fillContext_,
                                            *component,
                                            nbModelVariables_,
                                            variableStartColumn_);

        const Node* node = component->nodeAtPortField(port->Id(), fieldId);
        to_return += visitor.dispatch(node);
    }

    return to_return;
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const ComponentVariableNode*)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit ComponentVariableNodes");
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const ComponentParameterNode*)
{
    throw std::invalid_argument("ReadLinearExpressionVisitor cannot visit ComponentParameterNodes");
}
template<typename Derived>
requires(std::same_as<Derived, Eigen::MatrixXd> || std::same_as<Derived, Eigen::VectorXd>)
Derived cyclicRowShiftPerm(const Eigen::MatrixBase<Derived>& m, int shift)
{
    int n = m.rows();
    if (n == 0)
    {
        return Derived(m);
    }
    int s = ((shift % n) + n) % n;
    Eigen::PermutationMatrix<Eigen::Dynamic> perm(n);
    for (int i = 0; i < n; ++i)
    {
        perm.indices()[i] = (i + s) % n;
    }
    return (perm * m).eval(); // permutes rows
}

// Eigen::VectorXd cyclicRowShiftPerm(const Eigen::VectorXd& v, int shift)
// {
//     int n = v.size();
//     if (n == 0)
//     {
//         return v;
//     }
//     int s = ((shift % n) + n) % n; // normalize shift to [0,n-1]
//
//     Eigen::PermutationMatrix<Eigen::Dynamic> perm(n);
//     for (int i = 0; i < n; ++i)
//     {
//         perm.indices()[i] = (i + s) % n;
//     }
//
//     return perm * v; // permutes entries of v
// }

// template <typename Derived>
// auto cyclicRowShift(const Eigen::MatrixBase<Derived>& m, int shift) {
//     int n = m.rows();
//     if (n == 0) return m;
//
//     int s = ((shift % n) + n) % n; // 1. Normalize the shift
//     return (Eigen::VectorXi::LinSpaced(n, s, s + n - 1).array() % n).matrix()(Eigen::all); // 2.
//     The magic line
// }

// LinearExpressionEigen ReadLinearExpressionVisitor::TimeShift(const LinearExpressionEigen& left,
//                                                              int timeShift) const
// {
//     // TODO to be continued ...
//     LinearExpressionEigen to_return(nbtimeSteps_, nbModelVariables_);
//     // to_return.setCol()
//     for (auto localTimeStep = fillContext_.getLocalFirstTimeStep();
//          localTimeStep <= fillContext_.getLocalLastTimeStep();
//          ++localTimeStep)
//     {
//         auto shiftedIndex = rotatedIndex(localTimeStep, timeShift, fillContext_);
//         to_return.setRow(localTimeStep, left.coefPerVar().row(shiftedIndex));
//     }
//     return to_return;
// }

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const TimeShiftNode* node)
{
    const auto expression = dispatch(node->left());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto timeShift = static_cast<int>(evalVisitor_.dispatch(node->right()).valueAsDouble());
    return {cyclicRowShiftPerm(expression.coefPerVar(), timeShift),
            cyclicRowShiftPerm(expression.offset(), timeShift)};
}

LinearExpressionEigen ReadLinearExpressionVisitor::TimeIndex(
  const LinearExpressionEigen& expression,
  int timeIndex) const
{
    // TODO to be continued ...
    LinearExpressionEigen to_return(nbtimeSteps_, nbModelVariables_);
    const auto& expressionMatrix = expression.coefPerVar();

    for (auto i(0); i < variableStartColumn_.size(); ++i)
    {
        const auto variableStart = variableStartColumn_.at(i);
        const auto timeIndexCol = variableStart + timeIndex;
        Eigen::VectorXd col = Eigen::VectorXd::Constant(nbtimeSteps_,
                                                        expressionMatrix.coeff(timeIndex,
                                                                               timeIndexCol));
        to_return.setCol(timeIndexCol, col);
    }
    return to_return;
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const TimeIndexNode* node)
{
    const auto expression = dispatch(node->left());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto timeIndex = static_cast<int>(evalVisitor_.dispatch(node->right()).valueAsDouble());

    return TimeIndex(expression, timeIndex);
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const TimeSumNode* node)
{
    const auto expression = dispatch(node->expression());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto from = static_cast<int>(evalVisitor_.dispatch(node->from()).valueAsDouble());

    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto to = static_cast<int>(evalVisitor_.dispatch(node->to()).valueAsDouble());
    LinearExpressionEigen to_return(nbtimeSteps_, nbModelVariables_);
    for (auto timeShift = from; timeShift <= to; ++timeShift)
    {
        to_return += {cyclicRowShiftPerm(expression.coefPerVar(), timeShift),
                      cyclicRowShiftPerm(expression.offset(), timeShift)};
    }
    return to_return;
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const AllTimeSumNode* node)
{
    const auto expression = dispatch(node->child());
    LinearExpressionEigen to_return(nbtimeSteps_, nbModelVariables_);

    for (auto localTimeStep = fillContext_.getLocalFirstTimeStep();
         localTimeStep <= fillContext_.getLocalLastTimeStep();
         ++localTimeStep)
    {
        to_return += TimeIndex(expression, localTimeStep);
    }
    return to_return;
}
} // namespace Antares::Optimization
