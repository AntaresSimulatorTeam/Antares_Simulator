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

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/solver/optim-model-filler/ComponentFiller.h>
#include <antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h>
#include <antares/study/system-model/variable.h>
#include "antares/expressions/visitors/TimeIndexVisitor.h"

namespace Antares::Optimization
{

ComponentFiller::ComponentFiller(const Study::SystemModel::Component& component):
    component_(component),
    evaluationContext_(component_.getParameterValues(), {}),
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

    Expressions::Visitors::EvalVisitor evaluator(evaluationContext_);
    for (const auto& variable: component_.getModel()->Variables() | std::views::values)
    {
        if (variable.isTimeDependent())
        {
            pb.addVariable(evaluator.dispatch(variable.LowerBound().RootNode()),
                           evaluator.dispatch(variable.UpperBound().RootNode()),
                           variable.Type() != Study::SystemModel::ValueType::FLOAT,
                           component_.Id() + "." + variable.Id(),
                           ctx.getNumberOfTimestep());
        }
        else
        {
            pb.addVariable(evaluator.dispatch(variable.LowerBound().RootNode()),
                           evaluator.dispatch(variable.UpperBound().RootNode()),
                           variable.Type() != Study::SystemModel::ValueType::FLOAT,
                           component_.Id() + "." + variable.Id());
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
        auto* variable = pb.getVariable(component_.Id() + "." + var_id);
        ct->setCoefficient(variable, coef);
    }
}

void ComponentFiller::addTimeDependentConstraints(
  Optimisation::LinearProblemApi::ILinearProblem& pb,
  const LinearConstraint& linear_constraint,
  const std::string& constraint_id,
  unsigned int nb_cstr) const
{
    auto vect_ct = pb.addConstraint(linear_constraint.lb,
                                    linear_constraint.ub,
                                    component_.Id() + "." + constraint_id,
                                    nb_cstr);
    for (auto cstr(0); cstr < nb_cstr; ++cstr)
    {
        auto* ct = vect_ct[cstr];
        for (const auto& [var_id, coef]: linear_constraint.coef_per_var)
        {
            // TODO FIXME the coefficient needs to be time-dependent
            if (IsThisVariableTimeDependent(var_id))
            {
                auto* variable = pb.getVariable(component_.Id() + "." + var_id + '_'
                                                + std::to_string(cstr));
                ct->setCoefficient(variable, coef);
            }
            else
            {
                auto* variable = pb.getVariable(component_.Id() + "." + var_id);
                ct->setCoefficient(variable, coef);
            }
        }
    }
}

void ComponentFiller::addConstraints(Optimisation::LinearProblemApi::ILinearProblem& pb,
                                     Optimisation::LinearProblemApi::ILinearProblemData& data,
                                     Optimisation::LinearProblemApi::FillContext& ctx)
{
    ReadLinearConstraintVisitor visitor(evaluationContext_);
    for (const auto& constraint: component_.getModel()->getConstraints() | std::views::values)
    {
        auto* root_node = constraint.expression().RootNode();
        auto linear_constraint = visitor.dispatch(root_node);
        // TODO timesteps will be a parameter
        if (checkTimeSteps(ctx))
        {
            if (IsThisConstraintTimeDependent(root_node))

            {
                addTimeDependentConstraints(pb,
                                            linear_constraint,
                                            constraint.Id(),
                                            ctx.getNumberOfTimestep());
            }
            else
            {
                addStaticConstraint(pb, linear_constraint, constraint.Id());
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
    ReadLinearExpressionVisitor visitor(evaluationContext_);
    auto linear_expression = visitor.dispatch(model->Objective().RootNode());
    if (abs(linear_expression.offset()) > 1e-10)
    {
        throw std::invalid_argument("Antares does not support objective offsets (found in model '"
                                    + model->Id() + "' of component '" + component_.Id() + "').");
    }

    for (auto [var_id, coef]: linear_expression.coefPerVar())
    {
        if (IsThisVariableTimeDependent(var_id))
        {
            for (auto var_pos = 0; var_pos != ctx.getNumberOfTimestep(); ++var_pos)
            {
                auto* variable = pb.getVariable(component_.Id() + "." + var_id + '_'
                                                + std::to_string(var_pos));
                pb.setObjectiveCoefficient(variable, coef);
            }
        }
        else
        {
            auto* variable = pb.getVariable(component_.Id() + "." + var_id);
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
