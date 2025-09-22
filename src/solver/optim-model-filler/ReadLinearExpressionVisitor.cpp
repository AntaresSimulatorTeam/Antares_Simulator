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

#include <antares/exception/InvalidArgumentError.hpp>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/NodeVisitor.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h>
#include "antares/study/system-model/component.h"
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;
using namespace Antares::ModelerStudy;

namespace Antares::Optimization
{

ReadLinearExpressionVisitor::ReadLinearExpressionVisitor(
  const Optimisation::LinearProblemApi::FillContext& fillContext,
  const SystemModel::Component& component,
  const Optimisation::OptimEntityContainer& optimEntityContainer):
    evalContext_(optimEntityContainer.getOptimComponent(component.Index()).evaluationContext),
    fillContext_(fillContext),
    component_(component),
    evalVisitor_(optimEntityContainer, fillContext_, component),
    nbModelVariables_(optimEntityContainer.variablesSize()),
    optimEntityContainer_(optimEntityContainer)
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
    throw Error::InvalidArgumentError("A linear expression can't contain comparison operators.");
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const LessThanOrEqualNode*)
{
    throw Error::InvalidArgumentError("A linear expression can't contain comparison operators.");
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const GreaterThanOrEqualNode*)
{
    throw Error::InvalidArgumentError("A linear expression can't contain comparison operators.");
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const NegationNode* node)
{
    return -dispatch(node->child());
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const VariableNode* node)
{
    LinearExpressionEigen out(nbtimeSteps_, nbModelVariables_);
    out.reserve(nbtimeSteps_);

    const auto& optimComponent = optimEntityContainer_.getOptimComponent(component_.Index());
    const auto globalIndex = optimComponent.variableIndexMap.at(
      node->value()); // the only time we search in a map
    const auto& variableStartColumn = optimEntityContainer_.getVariableStartColumn();
    // const auto variableStart = variableStartColumn.at(globalIndex);
    // const auto globalIndex = optimComponent.variableIndexMap.at(
    //   node->value()); // the only time we search in a map
    // const auto variableStart = variableStartColumn[globalIndex];
    const auto variableStart = optimEntityContainer_.getVariableStartColumn(optimComponent.index,
                                                                            node->value());
    const auto variableEnd = variableStart == *variableStartColumn.rbegin()
                               ? nbModelVariables_
                               : variableStartColumn.at(globalIndex + 1);

    auto localFirstTimeStep = fillContext_.getLocalFirstTimeStep();
    auto localLastTimeStep = fillContext_.getLocalLastTimeStep();

    if (node->timeIndex() == Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO)
    {
        for (auto localTimeStep = localFirstTimeStep; localTimeStep <= localLastTimeStep;
             ++localTimeStep)
        {
            out.setCoeff(localTimeStep, variableStart, 1);
        }
    }
    // else time-dep only hanled    //  check if var is time-dep then nbTimeStep == variableEnd -
    // variableStart+1
    if (node->timeIndex() == Optimisation::TimeIndex::VARYING_IN_TIME_ONLY
        || node->timeIndex()
             == Optimisation::TimeIndex::VARYING_IN_TIME_AND_SCENARIO) /* scenario not handled !*/
    {
        auto variableIndex = variableStart;
        for (auto localTimeStep = localFirstTimeStep; localTimeStep <= localLastTimeStep;
             ++localTimeStep)
        {
            // for (auto variableIndex(variableStart); variableIndex < variableEnd; ++variableIndex)
            // {
            out.setCoeff(localTimeStep, variableIndex, 1);
            //}
            ++variableIndex;
        }
    }
    else
    {
        throw Error::InvalidArgumentError(
          "the support of scenario dependent variables is not available for now :(" + node->value()
          + ").");
    }
    return out;
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const ParameterNode* node)
{
    const auto systemParameter = evalContext_.getParameter(node->value());
    if (node->timeIndex() == Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO
        && systemParameter.type != Antares::ModelerStudy::SystemModel::ParameterType::CONSTANT)
    {
        throw Error::InvalidArgumentError(
          "Parameter " + node->value()
          + " is declared constant in time and scenario in library but not in system");
    }

    // TODO

    LinearExpressionEigen out(nbtimeSteps_, nbModelVariables_);
    if (systemParameter.type == SystemModel::ParameterType::CONSTANT)
    {
        out.setOffset(
          Eigen::VectorXd::Constant(nbtimeSteps_,
                                    evalContext_.getSystemParameterValueAsDouble(node->value())));
        return out;
    }
    // only dependent

    // assume global nb timeStep == nbtimeSteps
    const auto& parameters = evalContext_.getParameterValue(
      node->value(),
      fillContext_.getYear(),
      fillContext_.getGlobalFirstTimeStep() + fillContext_.getLocalFirstTimeStep(),
      fillContext_.getGlobalFirstTimeStep() + fillContext_.getLocalLastTimeStep());
    out.setOffset(Eigen::Map<const Eigen::VectorXd>(parameters.data(), parameters.size()));

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
    throw Error::InvalidArgumentError("ReadLinearExpressionVisitor cannot visit PortFieldNodes");
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const PortFieldSumNode* node)
{
    auto& portId = node->getPortName();
    auto& fieldId = node->getFieldName();

    LinearExpressionEigen to_return(nbtimeSteps_, nbModelVariables_);
    to_return.reserve(nbtimeSteps_ * nbModelVariables_ * 0.2); // 80% sparse
    for (const auto connexion_end: component_.componentConnectionsViaPort(portId))
    {
        auto* component = connexion_end.component();
        auto* port = connexion_end.port();

        ReadLinearExpressionVisitor visitor(fillContext_, *component, optimEntityContainer_);

        const Node* node = component->nodeAtPortField(port->Id(), fieldId);
        to_return += visitor.dispatch(node);
    }

    return to_return;
}

template<typename Derived>
requires(std::same_as<Derived, Eigen::SparseMatrix<double, Eigen::RowMajor>>
         || std::same_as<Derived, Eigen::VectorXd>)
Derived cyclicRowShiftPerm(const Derived& m, int shift)
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
        perm.indices()[(i + s) % n] = i;
    }

    return (perm * m).eval();
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const TimeShiftNode* node)
{
    auto expression = dispatch(node->left());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto timeShift = static_cast<int>(evalVisitor_.dispatch(node->right()).valueAsDouble());
    return {cyclicRowShiftPerm(expression.coefPerVar(), timeShift),
            cyclicRowShiftPerm(expression.offset(), timeShift)};
}

LinearExpressionEigen ReadLinearExpressionVisitor::TimeIndex(
  const LinearExpressionEigen& expression,
  int timeIndex) const
{
    const auto& modelVariablesGlobalIndices = optimEntityContainer_
                                                .getOptimComponent(component_.Index())
                                                .modelVariablesGlobalIndices;
    const auto& variableStartColumn = optimEntityContainer_.getVariableStartColumn();

    LinearExpressionEigen to_return(nbtimeSteps_, nbModelVariables_);
    to_return.reserve(modelVariablesGlobalIndices.size() * nbtimeSteps_);
    const auto offset = expression.offset()(timeIndex);
    to_return.setOffset(Eigen::VectorXd::Constant(nbtimeSteps_, offset));
    const auto& expressionMatrix = expression.coefPerVar();
    for (auto globalIndex: modelVariablesGlobalIndices)
    {
        const auto variableStart = variableStartColumn.at(globalIndex);
        const auto variableEnd = variableStart == *variableStartColumn.rbegin()
                                   ? nbModelVariables_
                                   : variableStartColumn.at(globalIndex + 1);

        if (variableEnd - variableStart > 1)
        {
            const auto timeIndexCol = variableStart + timeIndex;
            if (const auto value = expressionMatrix.coeff(timeIndex, timeIndexCol);
                std::abs(value) > 1e-16)
            {
                Eigen::VectorXd col = Eigen::VectorXd::Constant(nbtimeSteps_, value);
                to_return.setCol(timeIndexCol, col.sparseView());
            }
        }
        else
        {
            to_return.setCol(variableStart, expressionMatrix.coeff(timeIndex, variableStart));
        }
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
    to_return.reserve((nbtimeSteps_ * nbModelVariables_) * 0.2);
    for (auto timeShift = from; timeShift <= to; ++timeShift)
    {
        to_return += {cyclicRowShiftPerm(expression.coefPerVar(), timeShift),
                      cyclicRowShiftPerm(expression.offset(), timeShift)};
    }
    return to_return;
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const AllTimeSumNode* node)
{
    auto expression = dispatch(node->child());

    Eigen::VectorXd offsets = Eigen::VectorXd::Zero(nbtimeSteps_);
    for (int t = fillContext_.getLocalFirstTimeStep(); t <= fillContext_.getLocalLastTimeStep();
         ++t)
    {
        offsets.array() += expression.offset()[t];
    }

    std::vector<double> colSums(nbModelVariables_, 0.0);
    for (int row = 0; row < nbtimeSteps_; ++row)
    {
        for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator it(expression.coefPerVar(),
                                                                            row);
             it;
             ++it)
        {
            colSums[it.col()] += it.value();
        }
    }

    std::vector<Eigen::Triplet<double>> triplets;
    for (int col = 0; col < nbModelVariables_; ++col)
    {
        const double sum = colSums[col];
        if (std::abs(sum) > 1e-16)
        {
            for (int row = 0; row < nbtimeSteps_; ++row)
            {
                triplets.emplace_back(row, col, sum);
            }
        }
    }

    Eigen::SparseMatrix<double, Eigen::RowMajor> coeffs(nbtimeSteps_, nbModelVariables_);
    coeffs.setFromTriplets(triplets.begin(), triplets.end());

    return LinearExpressionEigen(coeffs, offsets);
}

} // namespace Antares::Optimization
