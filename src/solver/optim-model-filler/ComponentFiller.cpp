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
#include <ranges>
#include <variant>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/solver/optim-model-filler/ComponentFiller.h>
#include <antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h>
#include <antares/study/system-model/variable.h>
#include "antares/expressions/visitors/TimeIndexVisitor.h"
#include "antares/solver/optim-model-filler/scenarioGroupRepo.h"
namespace
{
template<typename T>
std::optional<T> buildOptional(bool condition, T value)
{
    if (condition)
    {
        return value;
    }
    else
    {
        return {};
    }
}
} // namespace

namespace Antares::Optimisation
{
VariablesBulkAddition::VariablesBulkAddition(
  Optimisation::LinearProblemApi::ILinearProblem& linear_problem,
  std::vector<std::vector<LinearProblemApi::IMipVariable*>>& variableDictionary):
    linear_problem_(linear_problem),
    variableDictionary(variableDictionary)
{
}

void VariablesBulkAddition::checkVariableDictionary(const std::string& compoId,
                                                    const std::string& variableId,
                                                    unsigned int modelVariableIndex,
                                                    unsigned localIndex) const
{
    // TOdo reserve the dict

    if (const auto size = variableDictionary.size(); size == modelVariableIndex) // create the entry
    {
        variableDictionary.push_back({});
    }
    else if (modelVariableIndex > size)
    {
        throw std::runtime_error("can not add variable " + variableId + "from component "
                                 + compoId);
    }
    auto& entry = variableDictionary[modelVariableIndex];
    if (const auto varsSize = entry.size(); varsSize == localIndex)
    {
        entry.push_back(nullptr);
    }
    else if (localIndex > varsSize)
    {
        throw std::runtime_error("can not add variable " + variableId + "from component "
                                 + compoId);
    }
}

void VariablesBulkAddition::addVariable(const std::string& compoId,
                                        const std::string& variableId,
                                        unsigned int modelVariableIndex,
                                        double lb,
                                        double ub,
                                        bool integer,
                                        const Optimization::Dimensions& dim) const
{
    for (const auto& s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            auto year = buildOptional<Optimization::MCYearAndTime::MCYear>(
              dim.isScenarioDependent(),
              static_cast<Optimization::MCYearAndTime::MCYear>(s));
            const auto ts = buildOptional(dim.isTimeDependent(), t);
            auto localIndex = s * dim.getNumberOfTimesteps() + t;
            checkVariableDictionary(compoId, variableId, modelVariableIndex, localIndex);
            variableDictionary[modelVariableIndex][localIndex] = linear_problem_.addVariable(
              lb,
              ub,
              integer,
              buildVariableName(compoId, variableId, year, ts));
        }
    }
}

void VariablesBulkAddition::addVariable(const std::string& compoId,
                                        const std::string& variableId,
                                        unsigned int modelVariableIndex,
                                        const std::vector<double>& lb,
                                        double ub,
                                        bool integer,
                                        const Optimization::Dimensions& dim) const
{
    auto count = dim.getNumberOfTimesteps();
    if (lb.size() != count)
    {
        std::ostringstream errMessage;

        errMessage << "requested " << count << " variables but lb size = " << lb.size();
        throw BoundsSizeMismatch(errMessage.str());
    }

    for (const auto& s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            auto year = buildOptional<Optimization::MCYearAndTime::MCYear>(
              dim.isScenarioDependent(),
              static_cast<Optimization::MCYearAndTime::MCYear>(s));
            const auto ts = buildOptional(dim.isTimeDependent(), t);
            auto localIndex = s * dim.getNumberOfTimesteps() + t;

            checkVariableDictionary(compoId, variableId, modelVariableIndex, localIndex);
            variableDictionary[modelVariableIndex][localIndex] = linear_problem_.addVariable(
              lb.at(t), /*use localIndex*/
              ub,
              integer,
              buildVariableName(compoId, variableId, year, ts));
        }
    }
}

void VariablesBulkAddition::addVariable(const std::string& compoId,
                                        const std::string& variableId,
                                        unsigned int modelVariableIndex,
                                        double lb,
                                        const std::vector<double>& ub,
                                        bool integer,
                                        const Optimization::Dimensions& dim) const
{
    auto count = dim.getNumberOfTimesteps();
    if (ub.size() != count)
    {
        std::ostringstream errMessage;
        errMessage << "requested " << count << " variables but ub size = " << ub.size();
        throw BoundsSizeMismatch(errMessage.str());
    }

    for (const auto& s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            auto year = buildOptional<Optimization::MCYearAndTime::MCYear>(
              dim.isScenarioDependent(),
              static_cast<Optimization::MCYearAndTime::MCYear>(s));
            const auto ts = buildOptional(dim.isTimeDependent(), t);
            auto localIndex = s * dim.getNumberOfTimesteps() + t;

            checkVariableDictionary(compoId, variableId, modelVariableIndex, localIndex);
            variableDictionary[modelVariableIndex][localIndex] = linear_problem_.addVariable(
              lb,
              ub.at(t), /*use localIndex*/
              integer,
              buildVariableName(compoId, variableId, year, ts));
        }
    }
}

void VariablesBulkAddition::addVariable(const std::string& compoId,
                                        const std::string& variableId,
                                        unsigned int modelVariableIndex,
                                        const std::vector<double>& lb,
                                        const std::vector<double>& ub,
                                        bool integer,
                                        const Optimization::Dimensions& dim) const
{
    auto count = dim.getNumberOfTimesteps();
    if (lb.size() != ub.size() || lb.size() != count)
    {
        std::ostringstream errMessage;
        errMessage << "requested " << count << " variables but lb size = " << lb.size()
                   << " and ub size = " << ub.size();
        throw BoundsSizeMismatch(errMessage.str());
    }

    for (const auto& s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            auto year = buildOptional<Optimization::MCYearAndTime::MCYear>(
              dim.isScenarioDependent(),
              static_cast<Optimization::MCYearAndTime::MCYear>(s));
            const auto ts = buildOptional(dim.isTimeDependent(), t);
            auto localIndex = s * dim.getNumberOfTimesteps() + t;
            checkVariableDictionary(compoId, variableId, modelVariableIndex, localIndex);
            variableDictionary[modelVariableIndex][localIndex] = linear_problem_.addVariable(
              lb.at(t), /*use localIndex*/
              ub.at(t), /*use localIndex*/
              integer,
              buildVariableName(compoId, variableId, year, ts));
        }
    }
}

std::string VariablesBulkAddition::buildVariableName(
  const std::string& compoId,
  const std::string& variableId,
  std::optional<Optimization::MCYearAndTime::MCYear> mcyear,
  std::optional<unsigned int> timestep)
{
    std::string ret = fmt::format("{}.{}", compoId, variableId);
    if (mcyear.has_value())
    {
        ret += "_s" + std::to_string(format_as(mcyear.value()));
    }
    if (timestep.has_value())
    {
        ret += "_t" + std::to_string(*timestep);
    }
    return ret;
}

ComponentFiller::ComponentFiller(const ModelerStudy::SystemModel::Component& component,
  std::vector<std::vector<LinearProblemApi::IMipVariable*>>& variableDictionary,
  const ScenarioGroupRepository& scenarioGroupRepository):
    component_(component),
    variableDictionary_(variableDictionary),
    scenarioGroupRepository_(scenarioGroupRepository)
{
}

bool checkTimeSteps(const Optimisation::LinearProblemApi::FillContext& ctx)
{
    return ctx.getLocalFirstTimeStep() <= ctx.getLocalLastTimeStep();
}

void ComponentFiller::addVariables(Optimisation::LinearProblemApi::ILinearProblem& pb,
                                   Optimisation::LinearProblemApi::ILinearProblemData& data,
                                   const Optimisation::LinearProblemApi::FillContext& ctx)
{
    if (!checkTimeSteps(ctx))
    {
        // exception?
        return;
    }

    const auto& scenario = scenarioGroupRepository_.scenario(component_.getScenarioGroupId());
    Expressions::Visitors::EvaluationContext evaluationContext(component_.getParameterValues(),
                                                               {},
                                                               data,
                                                               scenario);

    Expressions::Visitors::EvalVisitor evaluator(evaluationContext, ctx);
    auto valueOrDefault = [&evaluator](const auto& node, double defaultValue)
    {
        if (node.Empty())
        {
            return Expressions::Visitors::EvaluationResult(defaultValue);
        }
        return evaluator.dispatch(node.RootNode());
    };
    const auto& variables = component_.getModel()->Variables();
    for (auto i = 0; i < variables.size(); ++i)
    {
        const auto& variable = variables.at(i);
        const auto modelVariableGlobalIndex = component_.ModelVariablesGlobalIndices().at(i);
        namespace SM = ModelerStudy::SystemModel;
        const auto& lb = valueOrDefault(variable.LowerBound(),
                                        variable.Type() == SM::ValueType::BOOL ? 0
                                                                               : -pb.infinity());
        const auto& ub = valueOrDefault(variable.UpperBound(),
                                        variable.Type() == SM::ValueType::BOOL ? 1 : pb.infinity());
        const Optimization::PartialKey key(component_.Id(), variable.Id());
        if (variable.isTimeDependent())
        {
            const Optimization::Dimensions dim(
              Optimization::IntegerInterval{ctx.getYear(),
                                            ctx.getYear()}, /*TODO Handle range of year ? */
              Optimization::IntegerInterval(ctx.getLocalFirstTimeStep(),
                                            ctx.getLocalLastTimeStep()));
            // std::visit to handle the 4 cases: double/double, vector/double,
            // double/vector and vector/vector.
            std::visit(
              [&pb, &variable, this, &dim, &modelVariableGlobalIndex](const auto& lb_,
                                                                      const auto& ub_)
              {
                  VariablesBulkAddition(pb, variableDictionary_)
                    .addVariable(component_.Id(),
                                 variable.Id(),
                                 modelVariableGlobalIndex,
                                 lb_,
                                 ub_,
                                 variable.Type() != ModelerStudy::SystemModel::ValueType::FLOAT,
                                 dim);
              },
              lb.value(),
              ub.value());
        }
        else
        {
            // No time component
            const Optimization::Dimensions dim({}, {});

            VariablesBulkAddition(pb, variableDictionary_)
              .addVariable(component_.Id(),
                           variable.Id(),
                           modelVariableGlobalIndex,
                           lb.valueAsDouble(),
                           ub.valueAsDouble(),
                           variable.Type() != ModelerStudy::SystemModel::ValueType::FLOAT,
                           dim);
        }
    }
}

void ComponentFiller::addStaticConstraint(Optimisation::LinearProblemApi::ILinearProblem& pb,
                                          const Optimization::LinearConstraint& linear_constraint,
                                          const std::string& constraint_id) const
{
    auto* ct = pb.addConstraint(linear_constraint.lb(0),
                                linear_constraint.ub(0),
                                component_.Id() + "." + constraint_id);

    for (Eigen::SparseMatrix<double>::InnerIterator it(linear_constraint.coef_per_var, 0); it; ++it)
    {
        const auto& variables = variableDictionary_.at(it.col());

        ct->setCoefficient(variables.at(0), it.value());
    }
}

void ComponentFiller::addTimeDependentConstraints(
  Optimisation::LinearProblemApi::ILinearProblem& pb,
  const Optimization::LinearConstraint& linear_constraints,
  const std::string& constraint_id,
  const Optimisation::LinearProblemApi::FillContext& ctx) const
{
    const Optimization::Dimensions dim(
      Optimization::IntegerInterval{ctx.getYear(), ctx.getYear()}, /*TODO Handle range of year ? */
      Optimization::IntegerInterval(ctx.getLocalFirstTimeStep(), ctx.getLocalLastTimeStep()));
    for (const auto s: dim.getScenarioIndices()) // TODO
    {
        for (const auto t: dim.getTimesteps())
        {
            const auto localIndex = s * dim.getNumberOfTimesteps() + t;
            auto* ct = pb.addConstraint(linear_constraints.lb(localIndex),
                                        linear_constraints.ub(localIndex),
                                        component_.Id() + "." + constraint_id + '_'
                                          + std::to_string(t));

            for (Eigen::SparseMatrix<double>::InnerIterator it(linear_constraints.coef_per_var,
                                                               localIndex);
                 it;
                 ++it)
            {
                const auto& variables = variableDictionary_.at(it.col());

                ct->setCoefficient(variables.at(localIndex), it.value());
            }
        }
    }
}

void ComponentFiller::addConstraints(Optimisation::LinearProblemApi::ILinearProblem& pb,
                                     Optimisation::LinearProblemApi::ILinearProblemData& data,
                                     const Optimisation::LinearProblemApi::FillContext& ctx)
{
    const auto& scenario = scenarioGroupRepository_.scenario(component_.getScenarioGroupId());
    Expressions::Visitors::EvaluationContext evaluationContext(component_.getParameterValues(),
                                                               {},
                                                               data,
                                                               scenario);
    Optimization::ReadLinearConstraintVisitor visitor(evaluationContext,
                                                      ctx,
                                                      component_,
                                                      getNbVars(),
                                                      getVariableStartColumn());
    for (const auto& constraint: component_.getModel()->Constraints() | std::views::values)
    {
        auto* root_node = constraint.expression().RootNode();
        auto linear_constraints = visitor.dispatch(root_node);
        if (checkTimeSteps(ctx))
        {
            if (IsThisConstraintTimeDependent(root_node))
            {
                addTimeDependentConstraints(pb, linear_constraints, constraint.Id(), ctx);
            }
            else
            {
                addStaticConstraint(pb, linear_constraints, constraint.Id());
            }
        }
    }
}

size_t ComponentFiller::getNbVars() const
{
    return std::accumulate(variableDictionary_.begin(),
                           variableDictionary_.end(),
                           0,
                           [](size_t nbVars,
                              const std::vector<LinearProblemApi::IMipVariable*>& modelVar)
                           { return nbVars += modelVar.size(); });
}

const std::vector<unsigned>& ComponentFiller::getVariableStartColumn() const
{
    static std::vector<unsigned> startColumn(variableDictionary_.size());
    unsigned i = 0;
    for (const auto& variables: variableDictionary_)
    {
        if (i == 0)
        {
            startColumn[i] = 0;
        }
        else
        {
            startColumn[i] = startColumn.at(i - 1) + variableDictionary_.at(i - 1).size();
        }
        ++i;
    }
    return startColumn;
}

void ComponentFiller::addObjective(Optimisation::LinearProblemApi::ILinearProblem& pb,
                                   Optimisation::LinearProblemApi::ILinearProblemData& data,
                                   const Optimisation::LinearProblemApi::FillContext& ctx)
{
    auto model = component_.getModel();
    if (model->Objective().Empty())
    {
        return;
    }
    const auto& scenario = scenarioGroupRepository_.scenario(component_.getScenarioGroupId());
    Expressions::Visitors::EvaluationContext evaluationContext(component_.getParameterValues(),
                                                               {},
                                                               data,
                                                               scenario);

    Optimization::ReadLinearExpressionVisitor visitor(evaluationContext,
                                                      ctx,
                                                      component_,
                                                      getNbVars(),
                                                      getVariableStartColumn());

    const auto linearExpression = visitor.dispatch(model->Objective().RootNode());

    if (linearExpression.offset().nonZeros() > 0)
    {
        throw std::invalid_argument("Antares does not support objective offsets (found in model '"
                                    + model->Id() + "' of component '" + component_.Id() + "').");
    }

    const auto& coefPerVars = linearExpression.coefPerVar();
    const Optimization::Dimensions dim(Optimization::IntegerInterval{ctx.getYear(), ctx.getYear()},
                                       Optimization::IntegerInterval(ctx.getLocalFirstTimeStep(),
                                                                     ctx.getLocalLastTimeStep()));

    for (const auto s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            const auto localIndex = s * dim.getNumberOfTimesteps() + t;
            for (Eigen::SparseMatrix<double>::InnerIterator it(coefPerVars, localIndex); it; ++it)
            {
                const auto& variables = variableDictionary_.at(it.col());
                pb.setObjectiveCoefficient(variables.at(localIndex), it.value());
            }
        }
    }
}

bool ComponentFiller::IsThisConstraintTimeDependent(const Expressions::Nodes::Node* node) const
{
    Expressions::Visitors::TimeIndexVisitor timeIndexVisitor(component_);
    const auto ret = timeIndexVisitor.dispatch(node);
    return ret == Expressions::Visitors::TimeIndex::VARYING_IN_TIME_ONLY
           || ret == Expressions::Visitors::TimeIndex::VARYING_IN_TIME_AND_SCENARIO;
}
} // namespace Antares::Optimisation
