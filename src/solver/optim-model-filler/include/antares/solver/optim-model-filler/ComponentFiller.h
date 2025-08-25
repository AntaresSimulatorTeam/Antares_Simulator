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

#pragma once

#include <antares/expressions/visitors/TimeIndexVisitor.h>
#include <antares/optimisation/linear-problem-api/linearProblemFiller.h>
#include <antares/study/system-model/component.h>
#include "antares/expressions/visitors/EvaluationContext.h"
#include "antares/solver/optim-model-filler/VariableDictionary.h"

#include "ReadLinearConstraintVisitor.h"

namespace Antares::ModelerStudy::SystemModel
{
class Component;
class Variable;
} // namespace Antares::ModelerStudy::SystemModel

namespace Antares::Expressions::Visitors
{
class EvalVisitor;
}

namespace Antares::Optimisation
{
class ScenarioGroupRepository;

/**
 * Component filler
 * Implements LinearProblemFiller interface.
 * Fills a LinearProblem with variables, constraints, and objective coefficients of a Component
 */
template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
class ComponentFiller: public Optimisation::LinearProblemApi::LinearProblemFiller<SolverTagType>
{
public:
    ComponentFiller() = delete;

    ComponentFiller(ComponentFiller& other) = delete;

    /// Create a ComponentFiller for a Component
    ComponentFiller(
      const ModelerStudy::SystemModel::Component& component,
      Optimization::VariableDictionary<typename SolverTagType::VariableType>& variableDictionary,
      const ScenarioGroupRepository& scenarioGroupRepository):
        component_(component),
        variableDictionary_(variableDictionary),
        scenarioGroupRepository_(scenarioGroupRepository)
    {
    }

    bool checkTimeSteps(Optimisation::LinearProblemApi::FillContext& ctx)
    {
        return ctx.getLocalFirstTimeStep() <= ctx.getLocalLastTimeStep();
    }

    void addVariables(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
                      Optimisation::LinearProblemApi::ILinearProblemData& data,
                      Optimisation::LinearProblemApi::FillContext& ctx)
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
        for (const auto& variable: component_.getModel()->Variables() | std::views::values)
        {
            namespace SM = ModelerStudy::SystemModel;
            const auto& lb = valueOrDefault(variable.LowerBound(),
                                            variable.Type() == SM::ValueType::BOOL
                                              ? 0
                                              : -pb.infinity());
            const auto& ub = valueOrDefault(variable.UpperBound(),
                                            variable.Type() == SM::ValueType::BOOL ? 1
                                                                                   : pb.infinity());
            const Optimization::PartialKey key = variableDictionary_.buildKey(component_.Id(),
                                                                              variable.Id());
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
                  [&pb, &variable, this, &key, &dim](const auto& lb_, const auto& ub_)
                  {
                      VariablesBulkAddition<SolverTagType>(pb, variableDictionary_)
                        .addVariable(lb_,
                                     ub_,
                                     variable.Type() != ModelerStudy::SystemModel::ValueType::FLOAT,
                                     dim,
                                     key);
                  },
                  lb.value(),
                  ub.value());
            }
            else
            {
                // No time component
                const Optimization::Dimensions dim({}, {});

                variableDictionary_.addVariable(
                  dim,
                  key,
                  [&pb, &lb, &ub, &variable](const Optimization::MCYearAndTime&,
                                             const std::string& name)
                  {
                      return pb.addVariable(lb.valueAsDouble(),
                                            ub.valueAsDouble(),
                                            variable.Type()
                                              != ModelerStudy::SystemModel::ValueType::FLOAT,
                                            name);
                  });
            }
        }
    }

    void addConstraints(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
                        Optimisation::LinearProblemApi::ILinearProblemData& data,
                        Optimisation::LinearProblemApi::FillContext& ctx)
    {
        const auto& scenario = scenarioGroupRepository_.scenario(component_.getScenarioGroupId());
        Expressions::Visitors::EvaluationContext evaluationContext(component_.getParameterValues(),
                                                                   {},
                                                                   data,
                                                                   scenario);
        Optimization::ReadLinearConstraintVisitor visitor(evaluationContext,
                                                          ctx,
                                                          component_,
                                                          variableDictionary_);
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

    void addObjective(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
                      Optimisation::LinearProblemApi::ILinearProblemData& data,
                      Optimisation::LinearProblemApi::FillContext& ctx)
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
                                                          variableDictionary_);

        const auto timeDependentLinearExpression = visitor.dispatch(model->Objective().RootNode());
        const auto& linear_expressions = timeDependentLinearExpression.GetLinearExpressions();

        if (abs(linear_expressions.at(ctx.getLocalFirstTimeStep()).offset()) > 1e-10)
        {
            throw std::invalid_argument(
              "Antares does not support objective offsets (found in model '" + model->Id()
              + "' of component '" + component_.Id() + "').");
        }

        for (const auto& linear_expression: linear_expressions | std::views::values)
        {
            for (const auto& [variableFullKey, coefficient]: linear_expression.coefPerVar())
            {
                auto* variable = variableDictionary_(variableFullKey);
                pb.setObjectiveCoefficient(variable, coefficient);
            }
        }
    }

private:
    void addStaticConstraint(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
                             const Optimization::LinearConstraint& linear_constraint,
                             const std::string& constraint_id) const
    {
        auto* ct = pb.addConstraint(linear_constraint.lb,
                                    linear_constraint.ub,
                                    component_.Id() + "." + constraint_id);
        for (const auto& [variableFullKey, coefficient]: linear_constraint.coef_per_var)
        {
            auto* variable = variableDictionary_(variableFullKey);
            ct->setCoefficient(variable, coefficient);
        }
    }

    void addTimeDependentConstraints(
      Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
      const std::vector<Optimization::LinearConstraint>& linear_constraints,
      const std::string& constraint_id) const
    {
        for (const auto& linear_constraint: linear_constraints)
        {
            auto* ct = pb.addConstraint(linear_constraint.lb,
                                        linear_constraint.ub,
                                        component_.Id() + "." + constraint_id + '_'
                                          + std::to_string(linear_constraint.timeStep));
            for (const auto& [variableFullKey, coefficient]: linear_constraint.coef_per_var)
            {
                auto* variable = variableDictionary_(variableFullKey);
                ct->setCoefficient(variable, coefficient);
            }
        }
    }

    bool IsThisConstraintTimeDependent(const Expressions::Nodes::Node* node)
    {
        Expressions::Visitors::TimeIndexVisitor timeIndexVisitor(component_);
        const auto ret = timeIndexVisitor.dispatch(node);
        return ret == Expressions::Visitors::TimeIndex::VARYING_IN_TIME_ONLY
               || ret == Expressions::Visitors::TimeIndex::VARYING_IN_TIME_AND_SCENARIO;
    }

    const ModelerStudy::SystemModel::Component& component_;
    Optimization::VariableDictionary<typename SolverTagType::VariableType>& variableDictionary_;
    const ScenarioGroupRepository& scenarioGroupRepository_;
};

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
class VariablesBulkAddition
{
public:
    VariablesBulkAddition(
      Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& linear_problem,
      Optimization::VariableDictionary<typename SolverTagType::VariableType>& variableDictionary):
        linear_problem_(linear_problem),
        variableDictionary(variableDictionary)
    {
    }

    void addVariable(double lb,
                     double ub,
                     bool integer,
                     const Optimization::Dimensions& dim,
                     const Optimization::PartialKey& key) const
    {
        variableDictionary.addVariable(
          dim,
          key,
          [this, lb, ub, integer](const Optimization::MCYearAndTime&, const std::string& name)
          { return linear_problem_.addVariable(lb, ub, integer, name); });
    }

    void addVariable(const std::vector<double>& lb,
                     double ub,
                     bool integer,
                     const Optimization::Dimensions& dim,
                     const Optimization::PartialKey& key) const
    {
        auto count = dim.getNumberOfTimesteps();
        if (lb.size() != count)
        {
            std::ostringstream errMessage;
            errMessage << "requested " << count << " variables but lb size = " << lb.size();
            throw VariablesBulkAddition<SolverTagType>::BoundsSizeMismatch(errMessage.str());
        }
        const auto offset = *dim.getTimesteps().begin();

        variableDictionary.addVariable(
          dim,
          key,
          [this, &lb, ub, integer, offset](const Optimization::MCYearAndTime& timeAndScenario,
                                           const std::string& name) {
              return linear_problem_.addVariable(lb[timeAndScenario.timestep - offset],
                                                 ub,
                                                 integer,
                                                 name);
          });
    }

    void addVariable(double lb,
                     const std::vector<double>& ub,
                     bool integer,
                     const Optimization::Dimensions& dim,
                     const Optimization::PartialKey& key) const
    {
        auto count = dim.getNumberOfTimesteps();
        if (ub.size() != count)
        {
            std::ostringstream errMessage;
            errMessage << "requested " << count << " variables but ub size = " << ub.size();
            throw VariablesBulkAddition<SolverTagType>::BoundsSizeMismatch(errMessage.str());
        }
        const auto offset = *dim.getTimesteps().begin();
        variableDictionary.addVariable(
          dim,
          key,
          [this, lb, &ub, integer, offset](const Optimization::MCYearAndTime& timeAndScenario,
                                           const std::string& name) {
              return linear_problem_.addVariable(lb,
                                                 ub[timeAndScenario.timestep - offset],
                                                 integer,
                                                 name);
          });
    }

    void addVariable(const std::vector<double>& lb,
                     const std::vector<double>& ub,
                     bool integer,
                     const Optimization::Dimensions& dim,
                     const Optimization::PartialKey& key) const
    {
        auto count = dim.getNumberOfTimesteps();
        if (lb.size() != ub.size() || lb.size() != count)
        {
            std::ostringstream errMessage;
            errMessage << "requested " << count << " variables but lb size = " << lb.size()
                       << " and ub size = " << ub.size();
            throw VariablesBulkAddition<SolverTagType>::BoundsSizeMismatch(errMessage.str());
        }
        const auto offset = *dim.getTimesteps().begin();

        variableDictionary.addVariable(
          dim,
          key,
          [this, &lb, &ub, integer, offset](const Optimization::MCYearAndTime& timeAndScenario,
                                            const std::string& name)
          {
              return linear_problem_.addVariable(lb[timeAndScenario.timestep - offset],
                                                 ub[timeAndScenario.timestep - offset],
                                                 integer,
                                                 name);
          });
    }

    class BoundsSizeMismatch: public std::invalid_argument
    {
        using std::invalid_argument::invalid_argument;
    };

private:
    Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& linear_problem_;
    Optimization::VariableDictionary<typename SolverTagType::VariableType>& variableDictionary;
};
} // namespace Antares::Optimisation
