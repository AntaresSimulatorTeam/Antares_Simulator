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
#include <stdexcept>
#include <variant>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/solver/optim-model-filler/ComponentFiller.h>
#include <antares/study/system-model/variable.h>
#include "antares/expressions/visitors/TimeIndexVisitor.h"
#include "antares/modeler-optimisation-container/scenarioGroupRepo.h"

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

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;

namespace Antares::Optimisation
{
VariablesBulkAddition::VariablesBulkAddition(
  Optimisation::LinearProblemApi::ILinearProblem& linear_problem,
  OptimEntityContainer& optimEntityContainer):
    linear_problem_(linear_problem),
    optimEntityContainer_(optimEntityContainer)
{
}

void VariablesBulkAddition::addVariable(const std::string& compoId,
                                        const std::string& variableId,
                                        double lb,
                                        double ub,
                                        bool integer,
                                        const Optimisation::Dimensions& dim) const
{
    optimEntityContainer_.addStartColumn();
    for (const auto& s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            auto year = buildOptional(dim.isScenarioDependent(),
                                      static_cast<Optimization::MCYearAndTime::MCYear>(s));
            const auto ts = buildOptional(dim.isTimeDependent(), t);

            linear_problem_.addVariable(lb,
                                        ub,
                                        integer,
                                        buildVariableName(compoId, variableId, year, ts));
        }
    }
}

void VariablesBulkAddition::addVariable(const std::string& compoId,
                                        const std::string& variableId,
                                        const std::vector<double>& lb,
                                        double ub,
                                        bool integer,
                                        const Optimisation::Dimensions& dim) const
{
    auto count = dim.getNumberOfTimesteps();
    if (lb.size() != count)
    {
        std::ostringstream errMessage;

        errMessage << "requested " << count << " variables but lb size = " << lb.size();
        throw BoundsSizeMismatch(errMessage.str());
    }
    optimEntityContainer_.addStartColumn();
    for (const auto& s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            auto year = buildOptional<Optimization::MCYearAndTime::MCYear>(
              dim.isScenarioDependent(),
              static_cast<Optimization::MCYearAndTime::MCYear>(s));
            const auto ts = buildOptional(dim.isTimeDependent(), t);
            auto localIndex = s * dim.getNumberOfTimesteps() + t;

            linear_problem_.addVariable(lb[t], /*use localIndex*/
                                        ub,
                                        integer,
                                        buildVariableName(compoId, variableId, year, ts));
        }
    }
}

void VariablesBulkAddition::addVariable(const std::string& compoId,
                                        const std::string& variableId,
                                        double lb,
                                        const std::vector<double>& ub,
                                        bool integer,
                                        const Optimisation::Dimensions& dim) const
{
    auto count = dim.getNumberOfTimesteps();
    if (ub.size() != count)
    {
        std::ostringstream errMessage;
        errMessage << "requested " << count << " variables but ub size = " << ub.size();
        throw BoundsSizeMismatch(errMessage.str());
    }
    optimEntityContainer_.addStartColumn();
    for (const auto& s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            auto year = buildOptional<Optimization::MCYearAndTime::MCYear>(
              dim.isScenarioDependent(),
              static_cast<Optimization::MCYearAndTime::MCYear>(s));
            const auto ts = buildOptional(dim.isTimeDependent(), t);
            auto localIndex = s * dim.getNumberOfTimesteps() + t;

            linear_problem_.addVariable(lb,
                                        ub[t], /*use localIndex*/
                                        integer,
                                        buildVariableName(compoId, variableId, year, ts));
        }
    }
}

void VariablesBulkAddition::addVariable(const std::string& compoId,
                                        const std::string& variableId,
                                        const std::vector<double>& lb,
                                        const std::vector<double>& ub,
                                        bool integer,
                                        const Optimisation::Dimensions& dim) const
{
    auto count = dim.getNumberOfTimesteps();
    if (lb.size() != ub.size() || lb.size() != count)
    {
        std::ostringstream errMessage;
        errMessage << "requested " << count << " variables but lb size = " << lb.size()
                   << " and ub size = " << ub.size();
        throw BoundsSizeMismatch(errMessage.str());
    }
    optimEntityContainer_.addStartColumn();
    for (const auto& s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            auto year = buildOptional<Optimization::MCYearAndTime::MCYear>(
              dim.isScenarioDependent(),
              static_cast<Optimization::MCYearAndTime::MCYear>(s));
            const auto ts = buildOptional(dim.isTimeDependent(), t);
            auto localIndex = s * dim.getNumberOfTimesteps() + t;

            linear_problem_.addVariable(lb[t], /*use localIndex*/
                                        ub[t], /*use localIndex*/
                                        integer,
                                        buildVariableName(compoId, variableId, year, ts));
        }
    }
}

ComponentFiller::ComponentFiller(const ModelerStudy::SystemModel::Component& component,
                                 OptimEntityContainer& optimEntityContainer,
                                 const LinearProblemApi::ILinearProblemData& data,
                                 const ScenarioGroupRepository& scenarioGroupRepository):
    component_(component),
    optimEntityContainer_(optimEntityContainer),
    data_(data),
    scenarioGroupRepository_(scenarioGroupRepository)
{
}

bool checkTimeSteps(const LinearProblemApi::FillContext& ctx)
{
    return ctx.getLocalFirstTimeStep() <= ctx.getLocalLastTimeStep();
}

void ComponentFiller::addVariables(const LinearProblemApi::FillContext& ctx)
{
    if (!checkTimeSteps(ctx))
    {
        // exception?
        return;
    }

    const auto& evaluationContext = optimEntityContainer_.getEvaluationContext(component_);
    Expressions::Visitors::EvalVisitor evaluator(optimEntityContainer_, ctx, component_);
    auto valueOrDefault = [&evaluator](const auto& node, double defaultValue)
    {
        if (node.Empty())
        {
            return Expressions::Visitors::EvaluationResult(defaultValue);
        }
        return evaluator.dispatch(node.RootNode());
    };
    const auto& variables = component_.getModel()->Variables();
    auto& pb = optimEntityContainer_.Problem();
    for (auto i = 0; i < variables.size(); ++i)
    {
        const auto& variable = variables[i];
        namespace SM = ModelerStudy::SystemModel;
        const auto& lb = valueOrDefault(variable.LowerBound(),
                                        variable.Type() == SM::ValueType::BOOL ? 0
                                                                               : -pb.infinity());
        const auto& ub = valueOrDefault(variable.UpperBound(),
                                        variable.Type() == SM::ValueType::BOOL ? 1 : pb.infinity());
        if (variable.isTimeDependent())
        {
            const Optimisation::Dimensions dim(
              Optimisation::IntegerInterval{ctx.getYear(),
                                            ctx.getYear()}, /*TODO Handle range of year ? */
              Optimisation::IntegerInterval(ctx.getLocalFirstTimeStep(),
                                            ctx.getLocalLastTimeStep()));
            // std::visit to handle the 4 cases: double/double, vector/double,
            // double/vector and vector/vector.
            std::visit(
              [&pb, &variable, this, &dim](const auto& lb_, const auto& ub_)
              {
                  VariablesBulkAddition(pb, optimEntityContainer_)
                    .addVariable(component_.Id(),
                                 variable.Id(),
                                 lb_,
                                 ub_,
                                 variable.Type() != SM::ValueType::FLOAT,
                                 dim);
              },
              lb.value(),
              ub.value());
        }
        else
        {
            // No time component
            const Optimisation::Dimensions dim({}, {});

            VariablesBulkAddition(pb, optimEntityContainer_)
              .addVariable(component_.Id(),
                           variable.Id(),
                           lb.valueAsDouble(),
                           ub.valueAsDouble(),
                           variable.Type() != SM::ValueType::FLOAT,
                           dim);
        }
    }
}

void ComponentFiller::addStaticConstraint(const Optimisation::LinearConstraint& linear_constraint,
                                          const std::string& constraint_id)
{
    auto* ct = optimEntityContainer_.Problem().addConstraint(linear_constraint.lb[0],
                                                             linear_constraint.ub[0],
                                                             component_.Id() + "." + constraint_id);

    const auto& solverVariables = optimEntityContainer_.getVariables();
    const auto& coefsPerVar = linear_constraint.coef_per_var[0];

    for (const auto& [index, value]: coefsPerVar)
    {
        ct->setCoefficient(solverVariables[index].get(), value);
    }
}

void ComponentFiller::addTimeDependentConstraints(
  const Optimisation::LinearConstraint& linear_constraints,
  const std::string& constraint_id,
  const Optimisation::LinearProblemApi::FillContext& ctx)
{
    auto& pb = optimEntityContainer_.Problem();
    const Optimisation::Dimensions dim(
      Optimisation::IntegerInterval{ctx.getYear(), ctx.getYear()}, /*TODO Handle range of year ? */
      Optimisation::IntegerInterval(ctx.getLocalFirstTimeStep(), ctx.getLocalLastTimeStep()));

    const auto& solverVariables = optimEntityContainer_.getVariables();
    for (const auto s: dim.getScenarioIndices()) // TODO
    {
        for (const auto t: dim.getTimesteps())
        {
            auto* ct = pb.addConstraint(linear_constraints.lb[t],
                                        linear_constraints.ub[t],
                                        component_.Id() + "." + constraint_id + '_'
                                          + std::to_string(t));

            const auto& coefsPerVar = linear_constraints.coef_per_var[t];
            for (const auto& [index, value]: coefsPerVar)
            {
                ct->setCoefficient(solverVariables[index].get(), value);
            }
        }
    }
}

void ComponentFiller::addConstraints(const LinearProblemApi::FillContext& ctx)
{
    Optimisation::ReadLinearConstraintVisitor visitor(ctx, component_, optimEntityContainer_);

    const auto& modelConstraints = component_.getModel()->Constraints();
    for (auto constraintLocalIndex = 0; constraintLocalIndex < modelConstraints.size();
         ++constraintLocalIndex)
    {
        const auto& constraint = modelConstraints[constraintLocalIndex];
        auto* root_node = constraint.expression().RootNode();
        auto linear_constraints = visitor.dispatch(root_node);
        const auto timeIndex = getConstraintTimeIndex(root_node, component_);

        optimEntityContainer_.registerConstraint(component_, timeIndex);

        if (timeIndex == TimeIndex::VARYING_IN_TIME_ONLY
            || timeIndex == TimeIndex::VARYING_IN_TIME_AND_SCENARIO)
        {
            addTimeDependentConstraints(linear_constraints, constraint.Id(), ctx);
        }
        else
        {
            addStaticConstraint(linear_constraints, constraint.Id());
        }
    }
}

void ComponentFiller::addObjective(const Optimisation::LinearProblemApi::FillContext& ctx)
{
    auto model = component_.getModel();
    if (model->Objective().Empty())
    {
        return;
    }

    const auto& solverVariables = optimEntityContainer_.getVariables();
    ReadLinearExpressionVisitor visitor(optimEntityContainer_, component_, ctx);

    const auto linearExpression = visitor.visitRemoveDuplicates(model->Objective().RootNode());

    auto& pb = optimEntityContainer_.Problem();
    for (const auto& expr: linearExpression)
    {
        for (const auto& [index, value]: expr)
        {
            pb.setObjectiveCoefficient(solverVariables[index].get(), value);
        }
    }
}

TimeIndex ComponentFiller::getConstraintTimeIndex(
  const Expressions::Nodes::Node* node,
  const ModelerStudy::SystemModel::Component& component) const
{
    Expressions::Visitors::TimeIndexVisitor timeIndexVisitor(optimEntityContainer_, component);
    return timeIndexVisitor.dispatch(node);
}
} // namespace Antares::Optimisation
