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
    SetComponentBlocks();
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
void ReadLinearExpressionVisitor::SetComponentBlocks()
{
    const auto& variableStartColumn = variableContainer_.getVariableStartColumn();

    const auto& optimComponents = optimModel_.optimComponents;
    size_t optimCompoSize = optimComponents.size();

    const auto globalIndexFirstVarFirstCompo = optimComponents.at(0).modelVariablesGlobalIndices.at(
      0);
    const auto firstCompoFirstVarCol = variableStartColumn.at(
                                      globalIndexFirstVarFirstCompo;
    const auto lastOptimCompo = optimComponents.back();
    const auto lastOptimCompoLastGlobalIndex = optimComponents.back()
                                                 .modelVariablesGlobalIndices.back();
    const auto lastOptimCompoLastVariableStart = variableStartColumn.at(
      lastOptimCompoLastGlobalIndex);

    const auto lastCompoLastVarCol= lastOptimCompoLastVariableStart
                                         == *variableStartColumn.rbegin()
                                       ? nbModelVariables_ - 1
                                       : variableStartColumn.at(lastOptimCompoLastGlobalIndex + 1)
                                           - 1;


                componentBlocks_={ .blockSize = optimCompoSize,
                                .componentColsSize = (lastCompoLastVarCol-firstCompoFirstVarCol+1) / optimCompoSize,
                                .blockFirstColumn = firstCompoFirstVarCol,
                                .blockLastColumn = lastCompoLastVarCol}  ;
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const VariableNode* node)
{
    // const auto globalIndexlastVarlastCompo = (optimCompoSize == 1)
    //  ? optimComponents.at(0).modelVariablesGlobalIndices.at(0);

    LinearExpressionEigen out(nbtimeSteps_, nbModelVariables_, componentBlocks_);
    out.reserve(nbtimeSteps_);
    const auto& variableStartColumn = variableContainer_.getVariableStartColumn();
    const auto& optimComponents = optimModel_.optimComponents;
    for (const auto& optimComponent: optimComponents)
    {
        const auto globalIndex = optimComponent.variableIndexMap.at(
          node->value()); // the only time we search in a map
        const auto variableStart = variableStartColumn.at(globalIndex);
        // const auto variableEnd = variableStart == *variableStartColumn.rbegin()
        //                            ? nbModelVariables_
        //                            : variableStartColumn.at(globalIndex + 1);

        auto localFirstTimeStep = fillContext_.getLocalFirstTimeStep();
        auto localLastTimeStep = fillContext_.getLocalLastTimeStep();

        if (node->timeIndex() == TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO)
        {
            for (auto localTimeStep = localFirstTimeStep; localTimeStep <= localLastTimeStep;
                 ++localTimeStep)
            {
                out.setCoeff(localTimeStep, variableStart, 1);
            }
        }
        // else time-dep only hanled    //  check if var is time-dep then nbTimeStep == variableEnd
        // - variableStart+1
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
                out.setCoeff(localTimeStep, variableIndex, 1);
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
    return out;
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const ParameterNode* node)
{
    LinearExpressionEigen out(nbtimeSteps_, nbModelVariables_, componentBlocks_);
    auto compoLocalId = 0;
    for (const auto& optimComponent: optimModel_.optimComponents)
    {
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

        // TODO

        if (systemParameter.type == ParameterType::CONSTANT)
        {
            out.setOffset(compoLocalId,
                          Eigen::VectorXd::Constant(
                            nbtimeSteps_,
                            evaluationContext.getSystemParameterValueAsDouble(node->value())));
            return out;
        }
        // only dependent

        int idx = 0;
        // assume global nb timeStep == nbtimeSteps
        const auto& parameters = evaluationContext.getParameterValue(
          node->value(),
          fillContext_.getYear(),
          fillContext_.getGlobalFirstTimeStep() + fillContext_.getLocalFirstTimeStep(),
          fillContext_.getGlobalFirstTimeStep() + fillContext_.getLocalLastTimeStep());
        out.setOffset(compoLocalId,
                      Eigen::Map<const Eigen::VectorXd>(parameters.data(), parameters.size()));

        ++compoLocalId;
    }
    return out;
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const LiteralNode* node)
{
    // TODO
    LinearExpressionEigen out(nbtimeSteps_, nbModelVariables_, componentBlocks_);
    auto compoLocalId = 0;
    for (const auto& optimComponent: optimModel_.optimComponents)
    {
        out.setOffset(compoLocalId, Eigen::VectorXd::Constant(nbtimeSteps_, node->value()));
        ++compoLocalId;
    }
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

    LinearExpressionEigen to_return(nbtimeSteps_, nbModelVariables_, componentBlocks_);
    to_return.reserve(nbtimeSteps_ * nbModelVariables_ * 0.2); // 80% sparse
    for (const auto& optimComponent: optimModel_.optimComponents) // TODO cache
    {
        const auto& component_ = optimComponent.component;
        for (const auto connexion_end: component_->componentConnectionsViaPort(portId))
        {
            auto* component = connexion_end.component();
            auto* port = connexion_end.port();
            const Optimisation::OptimModel& optimModelAtModel = variableContainer_.getOptimModels()
                                                                  .at(
                                                                    component->getModel()->Index());
            // const auto& optimComponentAtModel = optimModelAtModel.optimComponents.at(
            //   component->Index());
            auto it = std::ranges::find_if(optimModelAtModel.optimComponents,
                                           [&component](const auto& optCompo)
                                           { return optCompo.component == component; });
            if (it != optimModelAtModel.optimComponents.end())
            {
                Optimisation::OptimModel optimModel;
                optimModel.optimComponents.push_back(
                  {.index = component->Index(),
                   .component = component,
                   .modelVariablesGlobalIndices = it->modelVariablesGlobalIndices,
                   .variableIndexMap = it->variableIndexMap});
                ReadLinearExpressionVisitor visitor(evalContextProvider_,
                                                    fillContext_,
                                                    optimModel,
                                                    variableContainer_);

                const Node* node = component->nodeAtPortField(port->Id(), fieldId);
                to_return += visitor.dispatch(node);
            }
        }
    }

    return to_return;
}

Eigen::VectorXd cyclicRowShiftPerm(const Eigen::VectorXd& m, int shift)
{
    int n = m.rows();
    if (n == 0)
    {
        return Eigen::VectorXd(m);
    }

    int s = ((shift % n) + n) % n;
    Eigen::PermutationMatrix<Eigen::Dynamic> perm(n);
    for (int i = 0; i < n; ++i)
    {
        perm.indices()[(i + s) % n] = i;
    }

    return (perm * m).eval();
}

#include <Eigen/Sparse>

Eigen::SparseMatrix<double, Eigen::RowMajor> cyclicRowShiftPerm(
  const Eigen::SparseMatrix<double, Eigen::RowMajor>& m,
  int shift,
  int fromCol,
  int toCol)
{
    int n = m.rows();
    if (n == 0 || fromCol > toCol)
    {
        return m; // nothing to do
    }

    // normalize shift into [0, n)
    int s = ((shift % n) + n) % n;

    auto res(m);
    fromCol = std::max(0, fromCol);
    toCol = std::min(toCol, static_cast<int>(m.cols()) - 1);
    for (int col = fromCol; col < toCol; ++col)
    {
        for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator it(m, col); it; ++it)
        {
            int row = it.row();
            int newRow = row;

            // shift only if col is inside the requested block
            if (col >= fromCol && col <= toCol)
            {
                newRow = (row + s) % n;
                res.coeffRef(newRow, col) = it.value();
            }
        }
    }

    res.makeCompressed();
    return res;
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const TimeShiftNode* node)
{
    auto expression = dispatch(node->left());

    for (const auto& optimComponent: optimModel_.optimComponents)
    {
        const auto& component = optimComponent.component;
        // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
        const auto evaluationContext = evalContextProvider_.provide(*component);
        EvalVisitor visitor(evaluationContext, fillContext_);
        const auto timeShift = static_cast<int>(visitor.dispatch(node->right()).valueAsDouble());
        expression.setCoefPerVar(cyclicRowShiftPerm(expression.coefPerVar(), timeShift),
                                 firstCol,
                                 lastCol);
        expression.setOffset(compoLocalId,
                             cyclicRowShiftPerm(expression.offset().at(compLocalId), timeShift));
    }
}

LinearExpressionEigen ReadLinearExpressionVisitor::TimeIndex(
  const LinearExpressionEigen& expression,
  int timeIndex) const
{
    const auto& modelVariablesGlobalIndices = variableContainer_
                                                .getOptimComponent(component.Index())
                                                .modelVariablesGlobalIndices;
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

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const TimeIndexNode* node)
{
    const auto expression = dispatch(node->left());

    const auto evaluationContext = evalContextProvider_.provide(component);
    EvalVisitor visitor(evaluationContext, fillContext_);
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto timeIndex = static_cast<int>(visitor.dispatch(node->right()).valueAsDouble());

    return TimeIndex(expression, timeIndex);
}

LinearExpressionEigen ReadLinearExpressionVisitor::visit(const TimeSumNode* node)
{
    const auto expression = dispatch(node->expression());

    const auto evaluationContext = evalContextProvider_.provide(component);
    EvalVisitor visitor(evaluationContext, fillContext_);
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto from = static_cast<int>(visitor.dispatch(node->from()).valueAsDouble());

    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto to = static_cast<int>(visitor.dispatch(node->to()).valueAsDouble());
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

    std::vector<Eigen::Triplet<double>> triplets;
    triplets.reserve(expression.coefPerVar().nonZeros());

    Eigen::VectorXd offsets = Eigen::VectorXd::Zero(nbtimeSteps_);

    // On accumule les contributions de chaque pas de temps
    for (auto t = fillContext_.getLocalFirstTimeStep(); t <= fillContext_.getLocalLastTimeStep();
         ++t)
    {
        // extraire la ligne t de expression
        for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator it(expression.coefPerVar(),
                                                                            t);
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
        offsets.array() += expression.offset()[t];
    }

    Eigen::SparseMatrix<double, Eigen::RowMajor> coeffs(nbtimeSteps_, nbModelVariables_);
    coeffs.setFromTriplets(triplets.begin(), triplets.end());

    return LinearExpressionEigen(coeffs, offsets);
}

} // namespace Antares::Optimization
