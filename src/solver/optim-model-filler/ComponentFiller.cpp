/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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

#include <ranges>
#include <variant>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/solver/optim-model-filler/ComponentFiller.h>
#include <antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h>
#include <antares/study/system-model/variable.h>
#include "antares/expressions/visitors/TimeIndexVisitor.h"

namespace Antares::Optimization
{

VariablesBulkAddition::VariablesBulkAddition(
  Optimisation::LinearProblemApi::ILinearProblem& linear_problem,
  VariableDictionary& variableDictionary):
    linear_problem_(linear_problem),
    variableDictionary(variableDictionary)
{
}

void VariablesBulkAddition::addVariable(double lb,
                                        double ub,
                                        bool integer,
                                        const Dimensions& dim,
                                        const PartialKey& key) const
{
    variableDictionary.addVariable(dim,
                                   key,
                                   [this, lb, ub, integer](const TimeAndScenario&,
                                                           const std::string& name)
                                   { return linear_problem_.addVariable(lb, ub, integer, name); });
}

void VariablesBulkAddition::addVariable(const std::vector<double>& lb,
                                        double ub,
                                        bool integer,
                                        const Dimensions& dim,
                                        const PartialKey& key) const
{
    auto count = dim.getNumberOfTimesteps();
    if (lb.size() != count)
    {
        throw std::invalid_argument("requested " + std::to_string(count)
                                    + " variables but lb size = " + std::to_string(lb.size()));
    }
    const auto offset = *dim.getTimesteps().begin();

    variableDictionary.addVariable(
      dim,
      key,
      [this, &lb, ub, integer, offset](const TimeAndScenario& timeAndScenario,
                                       const std::string& name) {
          return linear_problem_.addVariable(lb[timeAndScenario.timestep - offset],
                                             ub,
                                             integer,
                                             name);
      });
}

void VariablesBulkAddition::addVariable(double lb,
                                        const std::vector<double>& ub,
                                        bool integer,
                                        const Dimensions& dim,
                                        const PartialKey& key) const
{
    auto count = dim.getNumberOfTimesteps();
    if (ub.size() != count)
    {
        throw std::invalid_argument("requested " + std::to_string(count)
                                    + " variables but ub size = " + std::to_string(ub.size()));
    }
    const auto offset = *dim.getTimesteps().begin();
    variableDictionary.addVariable(
      dim,
      key,
      [this, lb, &ub, integer, offset](const TimeAndScenario& timeAndScenario,
                                       const std::string& name) {
          return linear_problem_.addVariable(lb,
                                             ub[timeAndScenario.timestep - offset],
                                             integer,
                                             name);
      });
}

void VariablesBulkAddition::addVariable(const std::vector<double>& lb,
                                        const std::vector<double>& ub,
                                        bool integer,
                                        const Dimensions& dim,
                                        const PartialKey& key) const
{
    auto count = dim.getNumberOfTimesteps();
    if (lb.size() != ub.size() || lb.size() != count)
    {
        throw std::invalid_argument("requested " + std::to_string(count)
                                    + " variables but lb size = " + std::to_string(lb.size())
                                    + " and ub size = " + std::to_string(ub.size()));
    }
    const auto offset = *dim.getTimesteps().begin();

    variableDictionary.addVariable(
      dim,
      key,
      [this, &lb, &ub, integer, offset](const TimeAndScenario& timeAndScenario,
                                        const std::string& name)
      {
          return linear_problem_.addVariable(lb[timeAndScenario.timestep - offset],
                                             ub[timeAndScenario.timestep - offset],
                                             integer,
                                             name);
      });
}

ComponentFiller::ComponentFiller(const Study::SystemModel::Component& component):
    component_(component),
    modelVariable_(component.getModel()->Variables())

{
}

bool checkTimeSteps(Optimisation::LinearProblemApi::FillContext& ctx)
{
    return ctx.getFirstTimeStep() <= ctx.getLastTimeStep();
}

void ComponentFiller::addVariables(Optimisation::LinearProblemApi::ILinearProblem& pb,
                                   Optimisation::LinearProblemApi::ILinearProblemData& data,
                                   Optimisation::LinearProblemApi::FillContext& ctx)
{
    if (!checkTimeSteps(ctx))
    {
        // exception?
        return;
    }

    Expressions::Visitors::EvaluationContext evaluationContext(component_.getParameterValues(),
                                                               {},
                                                               data);

    Expressions::Visitors::EvalVisitor evaluator(evaluationContext, ctx);
    for (const auto& variable: component_.getModel()->Variables() | std::views::values)
    {
        const auto& lb = evaluator.dispatch(variable.LowerBound().RootNode());
        const auto& ub = evaluator.dispatch(variable.UpperBound().RootNode());
        const PartialKey key(component_.Id(), variable.Id());
        if (variable.isTimeDependent())
        {
            const Dimensions dim({},
                                 IntegerInterval(ctx.getFirstTimeStep(), ctx.getLastTimeStep()));
            // std::visit to handle the 4 cases: double/double, vector/double,
            // double/vector and vector/vector.
            std::visit(
              [&pb, &variable, this, &key, &dim](const auto& lb_, const auto& ub_)
              {
                  VariablesBulkAddition(pb, variableDictionary)
                    .addVariable(lb_,
                                 ub_,
                                 variable.Type() != Study::SystemModel::ValueType::FLOAT,
                                 dim,
                                 key);
              },
              lb.value(),
              ub.value());
        }
        else
        {
            // No time component
            const Dimensions dim({}, {});

            variableDictionary.addVariable(
              dim,
              key,
              [&pb, &lb, &ub, &variable](const TimeAndScenario&, const std::string& name)
              {
                  return pb.addVariable(lb.valueAsDouble(),
                                        ub.valueAsDouble(),
                                        variable.Type() != Study::SystemModel::ValueType::FLOAT,
                                        name);
              });
        }
    }
}

void ComponentFiller::addStaticConstraint(Optimisation::LinearProblemApi::ILinearProblem& pb,
                                          const LinearConstraint& linear_constraint,
                                          const std::string& constraint_id) const
{
    auto* ct = pb.addConstraint(linear_constraint.lb,
                                linear_constraint.ub,
                                component_.Id() + "." + constraint_id);
    for (auto [var_id, coef]: linear_constraint.coef_per_var)
    {
        auto* variable = variableDictionary(component_.Id(), var_id);
        ct->setCoefficient(variable, coef);
    }
}

void ComponentFiller::addTimeDependentConstraints(
  Optimisation::LinearProblemApi::ILinearProblem& pb,
  const std::vector<LinearConstraint>& linear_constraints,
  const std::string& constraint_id) const
{
    for (const auto& linear_constraint: linear_constraints)
    {
        auto* ct = pb.addConstraint(linear_constraint.lb,
                                    linear_constraint.ub,
                                    component_.Id() + "." + constraint_id + '_'
                                      + std::to_string(linear_constraint.timeStep));
        for (const auto& [var_id, coef]: linear_constraint.coef_per_var)
        {
            if (IsThisVariableTimeDependent(var_id))
            {
                // TODO scenario = 0
                auto* variable = variableDictionary(component_.Id(),
                                                    var_id,
                                                    0,
                                                    linear_constraint.timeStep);

                ct->setCoefficient(variable, coef);
            }
            else
            {
                auto* variable = variableDictionary(component_.Id(), var_id);
                ct->setCoefficient(variable, coef);
            }
        }
    }
}

void ComponentFiller::addConstraints(Optimisation::LinearProblemApi::ILinearProblem& pb,
                                     Optimisation::LinearProblemApi::ILinearProblemData& data,
                                     Optimisation::LinearProblemApi::FillContext& ctx)
{
    Expressions::Visitors::EvaluationContext evaluationContext(component_.getParameterValues(),
                                                               {},
                                                               data);
    ReadLinearConstraintVisitor visitor(evaluationContext, ctx);
    for (const auto& constraint: component_.getModel()->getConstraints() | std::views::values)
    {
        auto* root_node = constraint.expression().RootNode();
        auto linear_constraints = visitor.dispatch(root_node);
        if (checkTimeSteps(ctx))
        {
            if (IsThisConstraintTimeDependent(root_node))

            {
                addTimeDependentConstraints(pb, linear_constraints, constraint.Id());
            }
            else
            {
                addStaticConstraint(pb, linear_constraints[0], constraint.Id());
            }
        }
    }
}

void ComponentFiller::addObjective(Optimisation::LinearProblemApi::ILinearProblem& pb,
                                   Optimisation::LinearProblemApi::ILinearProblemData& data,
                                   Optimisation::LinearProblemApi::FillContext& ctx)
{
    auto model = component_.getModel();
    if (model->Objective().Empty())
    {
        return;
    }
    Expressions::Visitors::EvaluationContext evaluationContext(component_.getParameterValues(),
                                                               {},
                                                               data);

    ReadLinearExpressionVisitor visitor(evaluationContext, ctx);

    auto linear_expressions = visitor.dispatch(model->Objective().RootNode())
                                .GetLinearExpressions();

    const auto& linear_expression = linear_expressions[ctx.getFirstTimeStep()];
    if (abs(linear_expression.offset()) > 1e-10)
    {
        throw std::invalid_argument("Antares does not support objective offsets (found in model '"
                                    + model->Id() + "' of component '" + component_.Id() + "').");
    }

    for (auto [var_id, coef]: linear_expression.coefPerVar())
    {
        if (IsThisVariableTimeDependent(var_id))
        {
            for (auto var_pos = ctx.getFirstTimeStep(); var_pos <= ctx.getLastTimeStep(); ++var_pos)
            {
                // TODO scenario = 0
                auto* variable = variableDictionary(component_.Id(), var_id, 0, var_pos);
                pb.setObjectiveCoefficient(variable,
                                           linear_expressions.at(var_pos).coefPerVar()[var_id]);
            }
        }
        else
        {
            // TODO scenario = 0
            auto* variable = variableDictionary(component_.Id(), var_id);
            pb.setObjectiveCoefficient(variable, coef);
        }
    }
}

bool ComponentFiller::IsThisConstraintTimeDependent(const Expressions::Nodes::Node* node)
{
    Expressions::Visitors::TimeIndexVisitor timeIndexVisitor;
    const auto ret = timeIndexVisitor.dispatch(node);
    return ret == Expressions::Visitors::TimeIndex::VARYING_IN_TIME_ONLY
           || ret == Expressions::Visitors::TimeIndex::VARYING_IN_TIME_AND_SCENARIO;
}

// return false if the variable with the id var_id is not found or if it is not time-dependent
bool ComponentFiller::IsThisVariableTimeDependent(const std::string& var_id) const
{
    if (const auto& it = modelVariable_.find(var_id); it != modelVariable_.end())
    {
        return it->second.isTimeDependent();
    }
    return false;
}
} // namespace Antares::Optimization
