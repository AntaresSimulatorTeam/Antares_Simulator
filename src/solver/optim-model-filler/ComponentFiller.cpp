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
                                        const Optimization::Dimensions& dim) const
{
    optimEntityContainer_.addStartColumn();
    for (const auto& s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            auto year = buildOptional<Optimization::MCYearAndTime::MCYear>(
              dim.isScenarioDependent(),
              static_cast<Optimization::MCYearAndTime::MCYear>(s));
            const auto ts = buildOptional(dim.isTimeDependent(), t);
            optimEntityContainer_.registerVariable(
              linear_problem_.addVariable(lb,
                                          ub,
                                          integer,
                                          buildVariableName(compoId, variableId, year, ts)));
        }
    }
}

void VariablesBulkAddition::addVariable(const std::string& compoId,
                                        const std::string& variableId,
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

            optimEntityContainer_.registerVariable(
              linear_problem_.addVariable(lb[t], /*use localIndex*/
                                          ub,
                                          integer,
                                          buildVariableName(compoId, variableId, year, ts)));
        }
    }
}

void VariablesBulkAddition::addVariable(const std::string& compoId,
                                        const std::string& variableId,
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

            optimEntityContainer_.registerVariable(
              linear_problem_.addVariable(lb,
                                          ub[t], /*use localIndex*/
                                          integer,
                                          buildVariableName(compoId, variableId, year, ts)));
        }
    }
}

void VariablesBulkAddition::addVariable(const std::string& compoId,
                                        const std::string& variableId,
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
            optimEntityContainer_.registerVariable(
              linear_problem_.addVariable(lb[t], /*use localIndex*/
                                          ub[t], /*use localIndex*/
                                          integer,
                                          buildVariableName(compoId, variableId, year, ts)));
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

void ComponentFiller::addVariables(
                                   const LinearProblemApi::FillContext& ctx)
{
    if (!checkTimeSteps(ctx))
    {
        // exception?
        return;
    }


    Expressions::Visitors::EvalVisitor evaluator(optimEntityContainer_, evaluationContext, ctx);
     auto& pb = optimEntityContainer_.getLinearProblem();
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
            const Optimization::Dimensions dim(
              Optimization::IntegerInterval{ctx.getYear(),
                                            ctx.getYear()}, /*TODO Handle range of year ? */
              Optimization::IntegerInterval(ctx.getLocalFirstTimeStep(),
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

            VariablesBulkAddition(pb, optimEntityContainer_)
              .addVariable(component_.Id(),
                           variable.Id(),
                           lb.valueAsDouble(),
                           ub.valueAsDouble(),
                           variable.Type() != ModelerStudy::SystemModel::ValueType::FLOAT,
                           dim);
        }
    }
}

void ComponentFiller::addStaticConstraint(
                                          const Optimization::LinearConstraint& linear_constraint,
                                          const std::string& constraint_id) 
{
    
    auto* ct = optimEntityContainer_.Problem().addConstraint(linear_constraint.lb(0),
                                linear_constraint.ub(0),
                                component_.Id() + "." + constraint_id);

                                optimEntityContainer_.registerConstraint(ct);
    const auto& solverVariables = optimEntityContainer_.getVariables();

    for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator
           it(linear_constraint.coef_per_var, 0);
         it;
         ++it)
    {
        ct->setCoefficient(solverVariables[it.col()], it.value());
    }
}

void ComponentFiller::addTimeDependentConstraints(
  const Optimization::LinearConstraint& linear_constraints,
  const std::string& constraint_id,
  const Optimisation::LinearProblemApi::FillContext& ctx) 
{
    auto& pb = optimEntityContainer_.Problem();
    const Optimization::Dimensions dim(
      Optimization::IntegerInterval{ctx.getYear(), ctx.getYear()}, /*TODO Handle range of year ? */
      Optimization::IntegerInterval(ctx.getLocalFirstTimeStep(), ctx.getLocalLastTimeStep()));

        const auto& solverVariables = optimEntityContainer_.getVariables();
    for (const auto s: dim.getScenarioIndices()) // TODO
    {
        for (const auto t: dim.getTimesteps())
        {
            auto* ct = pb.addConstraint(linear_constraints.lb(t),
                                        linear_constraints.ub(t),
                                        component_.Id() + "." + constraint_id + '_'
                                          + std::to_string(t));
                optimEntityContainer_.registerConstraint(ct);

            for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator
                   it(linear_constraints.coef_per_var, t);
                 it;
                 ++it)
            {
                ct->setCoefficient(solverVariables[it.col()], it.value());
            }
        }
    }
}

void ComponentFiller::addConstraints(
                                     const LinearProblemApi::FillContext& ctx)
{
    Optimization::ReadLinearConstraintVisitor visitor(
                                                      ctx,
                                                      component_,
                                                      optimEntityContainer_);

     const auto& modelConstraints = component_.getModel()->Constraints();
    for (auto constraintLocalIndex = 0; constraintLocalIndex < modelConstraints.size();
         ++constraintLocalIndex)
    {
        const auto& constraint = modelConstraints[constraintLocalIndex];
        auto* root_node = constraint.expression().RootNode();
        auto linear_constraints = visitor.dispatch(root_node);
     const auto gLobalIndex = optimEntityContainer_.ConstraintGLobalIndex();
     auto& optimComponent = optimEntityContainer_.getOptimComponent(component_.Index());
     optimComponent.modelConstraintsGlobalIndices.push_back(gLobalIndex);
      const auto timeIndex = getConstraintTimeIndex(root_node, component_);
            optimComponent.modelConstraintsTimeIndex.push_back(timeIndex);

            optimEntityContainer_.IncrementConstraintGLobalIndex();
            optimEntityContainer_.addStartLine();
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

void ComponentFiller::addObjective(
                                   const Optimisation::LinearProblemApi::FillContext& ctx)
{
    auto model = component_.getModel();
    if (model->Objective().Empty())
    {
        return;
    }

    const auto& solverVariables = optimEntityContainer_.getVariables();
    Optimization::ReadLinearExpressionVisitor visitor(evaluationContextProvider_,
                                                      ctx,
                                                      component_,
                                                      optimEntityContainer_);

    const auto linearExpression = visitor.dispatch(model->Objective().RootNode());
    const auto& offset = linearExpression.offset();
    // this is the simplest way to check if any entry of the offset is zero
    // Eigen::VectorXd returns the number of non-zero elements in the vector, based on the internal
    // storage. It does not use a tolerance for floating-point comparisons by default.
    for (auto i = 0; i < offset.size(); ++i)
    {
        if (std::abs(offset[i]) > 1e-10)
        {
            throw std::invalid_argument(
              "Antares does not support objective offsets (found in model '" + model->Id()
              + "' of component '" + component_.Id() + "').");
        }
    }

    const auto& coefPerVars = linearExpression.coefPerVar();
    const Optimization::Dimensions dim(Optimization::IntegerInterval{ctx.getYear(), ctx.getYear()},
                                       Optimization::IntegerInterval(ctx.getLocalFirstTimeStep(),
                                                                     ctx.getLocalLastTimeStep()));
    auto& pb = optimEntityContainer_.Problem()
    for (const auto s: dim.getScenarioIndices())
    {
        for (const auto t: dim.getTimesteps())
        {
            for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator it(coefPerVars, t); it;
                 ++it)
            {
                pb.setObjectiveCoefficient(solverVariables[it.col()], it.value());
            }
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
