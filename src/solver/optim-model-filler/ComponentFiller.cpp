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

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/expressions/visitors/EvalVisitor.h>
#include <antares/solver/optim-model-filler/ComponentFiller.h>
#include <antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h>
#include <antares/study/system-model/variable.h>

namespace Antares::Optimization
{

ComponentFiller::ComponentFiller(const Study::SystemModel::Component& component):
    component_(component),
    evaluationContext_(component_.getParameterValues(), {})
{
}
bool isTimeDependent(Solver::Modeler::Api::FillContext& ctx)
{
    return ctx.getFirstTimeStep() != ctx.getLastTimeStep();
}

bool checkTimeSteps(Solver::Modeler::Api::FillContext& ctx)
{
    return ctx.getFirstTimeStep() <= ctx.getLastTimeStep();
}

void ComponentFiller::addStaticVariable(Solver::Modeler::Api::ILinearProblem& pb,
                                        const std::unique_ptr<Solver::Visitors::EvalVisitor>&
                                        evaluator) const
{
    for (const auto& variable: component_.getModel()->Variables() | std::views::values)
    {
        pb.addVariable(evaluator->dispatch(variable.LowerBound().RootNode()),
                       evaluator->dispatch(variable.UpperBound().RootNode()),
                       variable.Type() != Study::SystemModel::ValueType::FLOAT,
                       component_.Id() + "." + variable.Id());
    }
}

void ComponentFiller::addTimeDependentVariables(Solver::Modeler::Api::ILinearProblem& pb,
                                                const std::unique_ptr<Solver::Visitors::EvalVisitor>
                                                & evaluator,
                                                unsigned int nb_vars) const
{
    for (const auto& variable: component_.getModel()->Variables() | std::views::values)
    {
        pb.addVariable(evaluator->dispatch(variable.LowerBound().RootNode()),
                       evaluator->dispatch(variable.UpperBound().RootNode()),
                       variable.Type() != Study::SystemModel::ValueType::FLOAT,
                       component_.Id() + "." + variable.Id(),
                       nb_vars
                );
    }
}

static unsigned int getNumberOfTimestep(const Solver::Modeler::Api::FillContext& ctx)
{
    return ctx.getLastTimeStep() - ctx.getFirstTimeStep() + 1;
}

void ComponentFiller::addVariables(Solver::Modeler::Api::ILinearProblem& pb,
                                   Solver::Modeler::Api::LinearProblemData& data,
                                   Solver::Modeler::Api::FillContext& ctx)
{
    auto evaluator = std::make_unique<Solver::Visitors::EvalVisitor>(evaluationContext_);
    if (checkTimeSteps(ctx))
    {
        if (isTimeDependent(ctx))
        {
        }
        else
        {
            addTimeDependentVariables(pb,
                                      evaluator,
                                      getNumberOfTimestep(ctx));
        }
    }
    else
    {
        // exception?
    }
}
void ComponentFiller::addStaticConstraint(
        Solver::Modeler::Api::ILinearProblem& pb,
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
        Solver::Modeler::Api::ILinearProblem& pb,
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
        for (auto [var_id, coef]: linear_constraint.coef_per_var)
        {
            auto* ct = vect_ct[cstr];
            auto* variable = pb.getVariable(
                    component_.Id() + "." + var_id + '_' + std::to_string(cstr));
            ct->setCoefficient(variable, coef);
        }
    }
}

void ComponentFiller::addConstraints(Solver::Modeler::Api::ILinearProblem& pb,
                                     Solver::Modeler::Api::LinearProblemData& data,
                                     Solver::Modeler::Api::FillContext& ctx)
{
    ReadLinearConstraintVisitor visitor(evaluationContext_);
    for (const auto& constraint: component_.getModel()->getConstraints() | std::views::values)
    {
        auto linear_constraint = visitor.dispatch(constraint.expression().RootNode());
        if (checkTimeSteps(ctx))
        {
            if (isTimeDependent(ctx))
            {
                addTimeDependentConstraints(pb,
                                            linear_constraint,
                                            constraint.Id(),
                                            getNumberOfTimestep(ctx));
            }
            else
            {
                addStaticConstraint(pb, linear_constraint, constraint.Id());
            }
        }
    }
}

void ComponentFiller::addObjective(Solver::Modeler::Api::ILinearProblem& pb,
                                   Solver::Modeler::Api::LinearProblemData& data,
                                   Solver::Modeler::Api::FillContext& ctx)
{
    if (component_.getModel()->Objective().Empty())
    {
        return;
    }
    ReadLinearExpressionVisitor visitor(evaluationContext_);
    auto linear_expression = visitor.dispatch(component_.getModel()->Objective().RootNode());
    if (abs(linear_expression.offset()) > 1e-10)
    {
        throw std::invalid_argument("Antares does not support objective offsets (found in model '"
                                    + component_.getModel()->Id() + "' of component '"
                                    + component_.Id() + "').");
    }
    for (auto [var_id, coef]: linear_expression.coefPerVar())
    {
        auto* variable = pb.getVariable(component_.Id() + "." + var_id);
        pb.setObjectiveCoefficient(variable, coef);
    }
}

} // namespace Antares::Optimization
