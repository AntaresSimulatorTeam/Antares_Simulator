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
  const Optimisation::EvaluationContextProvider& evalContextProvider,
  const Optimisation::LinearProblemApi::FillContext& fillContext,
  const Optimisation::OptimModel& optimModel,
  const Optimisation::VariableContainer& variableContainer):
    evalContextProvider_(evalContextProvider),
    fillContext_(fillContext),
    optimModel_(optimModel),
    nbModelVariables_(variableContainer.variablesSize()),
    variableContainer_(variableContainer)
{
    nbtimeSteps_ = fillContext_.getLocalLastTimeStep() - fillContext_.getLocalFirstTimeStep() + 1;
}

std::string ReadLinearExpressionVisitor::name() const
{
    return "ReadLinearExpressionVisitor";
}

std::vector<LinearExpressionEigen> ReadLinearExpressionVisitor::visit(const SumNode* node)
{
    const auto& operands = node->getOperands();
    std::vector ret(optimModel_.optimComponents.size(),
                    LinearExpressionEigen(nbtimeSteps_, nbModelVariables_));
    for (auto* operand: operands)
    {
        ret += dispatch(operand);
    }
    return ret;
}

std::vector<LinearExpressionEigen> ReadLinearExpressionVisitor::visit(const SubtractionNode* node)
{
    auto ret = dispatch(node->left());
    ret -= dispatch(node->right()); // using -= operator avoid expensive copy
    return ret;
}

std::vector<LinearExpressionEigen> ReadLinearExpressionVisitor::visit(
  const MultiplicationNode* node)
{
    auto ret = dispatch(node->left());
    ret *= dispatch(node->right());
    return ret;
}

std::vector<LinearExpressionEigen> ReadLinearExpressionVisitor::visit(const DivisionNode* node)
{
    auto ret = dispatch(node->left());
    ret /= dispatch(node->right());
    return ret;
}

std::vector<LinearExpressionEigen> ReadLinearExpressionVisitor::visit(const EqualNode*)
{
    throw Error::InvalidArgumentError("A linear expression can't contain comparison operators.");
}

std::vector<LinearExpressionEigen> ReadLinearExpressionVisitor::visit(const LessThanOrEqualNode*)
{
    throw Error::InvalidArgumentError("A linear expression can't contain comparison operators.");
}

std::vector<LinearExpressionEigen> ReadLinearExpressionVisitor::visit(const GreaterThanOrEqualNode*)
{
    throw Error::InvalidArgumentError("A linear expression can't contain comparison operators.");
}

std::vector<LinearExpressionEigen> ReadLinearExpressionVisitor::visit(const NegationNode* node)
{
    auto ret = dispatch(node->child());
    return operator-(ret);
}

std::vector<LinearExpressionEigen> ReadLinearExpressionVisitor::visit(const VariableNode* node)
{
    const auto& optimComponents = optimModel_.optimComponents;
    size_t compoNumber = optimComponents.size();
    std::vector ret(compoNumber, LinearExpressionEigen(nbtimeSteps_, nbModelVariables_));
    const auto& variableStartColumn = variableContainer_.getVariableStartColumn();

    for (int compoLocalId = 0; compoLocalId < compoNumber; ++compoLocalId)
    {
        const auto& optimComponent = optimComponents.at(compoLocalId);
        auto& linearExpressionMatrix = ret.at(compoLocalId);
        linearExpressionMatrix.reserve(nbtimeSteps_);

        const auto globalIndex = optimComponent.variableIndexMap.at(
          node->value()); // the only time we search in a map
        const auto variableStart = variableStartColumn.at(globalIndex);

        auto localFirstTimeStep = fillContext_.getLocalFirstTimeStep();
        auto localLastTimeStep = fillContext_.getLocalLastTimeStep();

        if (node->timeIndex() == TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO)
        {
            for (auto localTimeStep = localFirstTimeStep; localTimeStep <= localLastTimeStep;
                 ++localTimeStep)
            {
                linearExpressionMatrix.setCoeff(localTimeStep, variableStart, 1);
            }
        }

        if (node->timeIndex() == TimeIndex::VARYING_IN_TIME_ONLY
            || node->timeIndex()
                 == TimeIndex::VARYING_IN_TIME_AND_SCENARIO) /* scenario not handled !*/
        {
            auto variableIndex = variableStart;
            for (auto localTimeStep = localFirstTimeStep; localTimeStep <= localLastTimeStep;
                 ++localTimeStep)
            {
                // for (auto variableIndex(variableStart); variableIndex < variableEnd;
                // ++variableIndex)
                // {
                linearExpressionMatrix.setCoeff(localTimeStep, variableIndex, 1);
                //}
                ++variableIndex;
            }
        }
        else
        {
            throw Error::InvalidArgumentError(
              "the support of scenario dependent variables is not available for now :("
              + node->value() + ").");
        }
    }
    return ret;
}

std::vector<LinearExpressionEigen> ReadLinearExpressionVisitor::visit(const ParameterNode* node)
{
    const auto& optimComponents = optimModel_.optimComponents;
    size_t compoNumber = optimComponents.size();
    std::vector ret(compoNumber, LinearExpressionEigen(nbtimeSteps_, nbModelVariables_));

    for (int compoLocalId = 0; compoLocalId < compoNumber; ++compoLocalId)
    {
        const auto& optimComponent = optimComponents.at(compoLocalId);
        const auto& component = optimComponent.component;
        const auto evaluationContext = evalContextProvider_.provide(*component);
        const auto systemParameter = evaluationContext.getParameter(node->value());
        if (node->timeIndex() == TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO
            && systemParameter.type != ParameterType::CONSTANT)
        {
            throw Error::InvalidArgumentError(
              "Parameter " + node->value()
              + " is declared constant in time and scenario in library but not in system");
        }

        auto& linearExpressionMatrix = ret.at(compoLocalId);

        if (systemParameter.type == ParameterType::CONSTANT)
        {
            linearExpressionMatrix.setOffset(Eigen::VectorXd::Constant(
              nbtimeSteps_,
              evaluationContext.getSystemParameterValueAsDouble(node->value())));
            continue;
        }
        // only dependent

        int idx = 0;
        // assume global nb timeStep == nbtimeSteps
        const auto& parameters = evaluationContext.getParameterValue(
          node->value(),
          fillContext_.getYear(),
          fillContext_.getGlobalFirstTimeStep() + fillContext_.getLocalFirstTimeStep(),
          fillContext_.getGlobalFirstTimeStep() + fillContext_.getLocalLastTimeStep());
        linearExpressionMatrix.setOffset(
          Eigen::Map<const Eigen::VectorXd>(parameters.data(), parameters.size()));

    }
    return ret;
}

std::vector<LinearExpressionEigen> ReadLinearExpressionVisitor::visit(const LiteralNode* node)
{
    const auto& optimComponents = optimModel_.optimComponents;
    size_t compoNumber = optimComponents.size();
    std::vector ret(compoNumber, LinearExpressionEigen(nbtimeSteps_, nbModelVariables_));

    for (int compoLocalId = 0; compoLocalId < compoNumber; ++compoLocalId)
    {
        auto& linearExpressionMatrix = ret.at(compoLocalId);
        linearExpressionMatrix.setOffset(Eigen::VectorXd::Constant(nbtimeSteps_, node->value()));
    }
    return ret;
}

std::vector<LinearExpressionEigen> ReadLinearExpressionVisitor::visit(const PortFieldNode*)
{
    throw Error::InvalidArgumentError("ReadLinearExpressionVisitor cannot visit PortFieldNodes");
}

std::vector<LinearExpressionEigen> ReadLinearExpressionVisitor::visit(const PortFieldSumNode* node)
{
    auto& portId = node->getPortName();
    auto& fieldId = node->getFieldName();
    const auto& optimComponents = optimModel_.optimComponents;
    size_t compoNumber = optimComponents.size();
    std::vector ret(compoNumber, LinearExpressionEigen(nbtimeSteps_, nbModelVariables_));

    for (int compoLocalId = 0; compoLocalId < compoNumber; ++compoLocalId)
    {
        const auto& optimComponent = optimComponents.at(compoLocalId);
        const auto& component = optimComponent.component;

        auto& linearExpressionMatrix = ret.at(compoLocalId);
        linearExpressionMatrix.reserve(nbtimeSteps_ * nbModelVariables_ * 0.2); // 80% sparse
        for (const auto connexion_end: component->componentConnectionsViaPort(portId))
        {
            auto* connectedComponent = connexion_end.component();
            auto* port = connexion_end.port();
            const Optimisation::OptimModel& optimModelAtModel = variableContainer_.getOptimModels()
                                                                  .at(connectedComponent->getModel()
                                                                        ->Index());
            // const auto& optimComponentAtModel = optimModelAtModel.optimComponents.at(
            //   component->Index());
            auto it = std::ranges::find_if(optimModelAtModel.optimComponents,
                                           [&connectedComponent](const auto& optCompo)
                                           { return optCompo.component == connectedComponent; });
            if (it != optimModelAtModel.optimComponents.end())
            {
                Optimisation::OptimModel optimModel;
                optimModel.optimComponents.push_back(
                  {.index = connectedComponent->Index(),
                   .component = connectedComponent,
                   .modelVariablesGlobalIndices = it->modelVariablesGlobalIndices,
                   .variableIndexMap = it->variableIndexMap});
                ReadLinearExpressionVisitor visitor(evalContextProvider_,
                                                    fillContext_,
                                                    optimModel,
                                                    variableContainer_);

                const Node* node = connectedComponent->nodeAtPortField(port->Id(), fieldId);
                linearExpressionMatrix += visitor.dispatch(node).at(0);
            }
        }
    }

    return ret;
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

std::vector<LinearExpressionEigen> ReadLinearExpressionVisitor::visit(const TimeShiftNode* node)
{
    auto expression = dispatch(node->left());
    const auto& optimComponents = optimModel_.optimComponents;
    size_t compoNumber = optimComponents.size();
    std::vector ret(compoNumber, LinearExpressionEigen(nbtimeSteps_, nbModelVariables_));

    for (int compoLocalId = 0; compoLocalId < compoNumber; ++compoLocalId)
    {
        const auto& optimComponent = optimComponents.at(compoLocalId);
        const auto& component = optimComponent.component;
        const auto evaluationContext = evalContextProvider_.provide(*component);
        EvalVisitor visitor(evaluationContext, fillContext_);
        // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
        const auto timeShift = static_cast<int>(visitor.dispatch(node->right()).valueAsDouble());
        auto& linearExpressionMatrix = ret.at(compoLocalId);
        linearExpressionMatrix.setCoefPerVar(
          cyclicRowShiftPerm(expression.at(compoLocalId).coefPerVar(), timeShift));
        linearExpressionMatrix.setOffset(
          cyclicRowShiftPerm(expression.at(compoLocalId).offset(), timeShift));
    }
    return ret;
}

LinearExpressionEigen ReadLinearExpressionVisitor::TimeIndex(
  const LinearExpressionEigen& expression,
  int timeIndex,
  const Optimisation::OptimComponent& optimComponent) const
{
    const auto& modelVariablesGlobalIndices = optimComponent.modelVariablesGlobalIndices;
    const auto& variableStartColumn = variableContainer_.getVariableStartColumn();

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

std::vector<LinearExpressionEigen> ReadLinearExpressionVisitor::visit(const TimeIndexNode* node)
{
    const auto expression = dispatch(node->left());

    const auto& optimComponents = optimModel_.optimComponents;
    size_t compoNumber = optimComponents.size();
    std::vector ret(compoNumber, LinearExpressionEigen(nbtimeSteps_, nbModelVariables_));

    for (int compoLocalId = 0; compoLocalId < compoNumber; ++compoLocalId)
    {
        const auto& optimComponent = optimComponents.at(compoLocalId);
        const auto& component = optimComponent.component;
        const auto evaluationContext = evalContextProvider_.provide(*component);
        EvalVisitor visitor(evaluationContext, fillContext_);
        // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
        const auto timeIndex = static_cast<int>(visitor.dispatch(node->right()).valueAsDouble());
        ret[compoLocalId] = TimeIndex(expression.at(compoLocalId), timeIndex, optimComponent);
    }
    return ret;
}

std::vector<LinearExpressionEigen> ReadLinearExpressionVisitor::visit(const TimeSumNode* node)
{
    const auto expression = dispatch(node->expression());

    const auto& optimComponents = optimModel_.optimComponents;
    size_t compoNumber = optimComponents.size();
    std::vector ret(compoNumber, LinearExpressionEigen(nbtimeSteps_, nbModelVariables_));

    for (int compoLocalId = 0; compoLocalId < compoNumber; ++compoLocalId)
    {
        const auto& optimComponent = optimComponents.at(compoLocalId);
        const auto& component = optimComponent.component;
        const auto evaluationContext = evalContextProvider_.provide(*component);
        EvalVisitor visitor(evaluationContext, fillContext_);
        // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
        const auto from = static_cast<int>(visitor.dispatch(node->from()).valueAsDouble());

        // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
        const auto to = static_cast<int>(visitor.dispatch(node->to()).valueAsDouble());
        auto& linearExpressionMatrix = ret.at(compoLocalId);
        linearExpressionMatrix.reserve((nbtimeSteps_ * nbModelVariables_) * 0.2);
        for (auto timeShift = from; timeShift <= to; ++timeShift)
        {
            linearExpressionMatrix += {cyclicRowShiftPerm(expression.at(compoLocalId).coefPerVar(),
                                                          timeShift),
                                       cyclicRowShiftPerm(expression.at(compoLocalId).offset(),
                                                          timeShift)};
        }
    }
    return ret;
}

std::vector<LinearExpressionEigen> ReadLinearExpressionVisitor::visit(const AllTimeSumNode* node)
{
    auto expression = dispatch(node->child());
    const auto& optimComponents = optimModel_.optimComponents;
    size_t compoNumber = optimComponents.size();
    std::vector ret(compoNumber, LinearExpressionEigen(nbtimeSteps_, nbModelVariables_));

    for (int compoLocalId = 0; compoLocalId < compoNumber; ++compoLocalId)
    {
        const auto& optimComponent = optimComponents.at(compoLocalId);
        const auto& component = optimComponent.component;
        const auto evaluationContext = evalContextProvider_.provide(*component);
        std::vector<Eigen::Triplet<double>> triplets;
        const auto& compoExpression = expression.at(compoLocalId);
        triplets.reserve(compoExpression.coefPerVar().nonZeros());

        Eigen::VectorXd offsets = Eigen::VectorXd::Zero(nbtimeSteps_);

        // On accumule les contributions de chaque pas de temps
        for (auto t = fillContext_.getLocalFirstTimeStep();
             t <= fillContext_.getLocalLastTimeStep();
             ++t)
        {
            // extraire la ligne t de expression
            for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator
                   it(compoExpression.coefPerVar(), t);
                 it;
                 ++it)
            {
                // Ici on "réplique" la ligne t dans toutes les lignes du résultat
                for (int row = 0; row < nbtimeSteps_; ++row)
                {
                    triplets.emplace_back(row, it.col(), it.value());
                }
            }

            // offsets
            offsets.array() += compoExpression.offset()[t];
        }

        Eigen::SparseMatrix<double, Eigen::RowMajor> coeffs(nbtimeSteps_, nbModelVariables_);
        coeffs.setFromTriplets(triplets.begin(), triplets.end());

        ret[compoLocalId] = LinearExpressionEigen(coeffs, offsets);
    }
    return ret;
}

} // namespace Antares::Optimization
