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

#include <antares/optimisation/linear-problem-api/linearProblemFiller.h>
#include <antares/study/system-model/component.h>
#include "antares/expressions/visitors/EvaluationContext.h"
#include "antares/solver/optim-model-filler/VariableDictionary.h"

#include "EvaluationContextProvider.h"
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
class ComponentFiller: public LinearProblemApi::LinearProblemFiller
{
public:
    ComponentFiller() = delete;

    ComponentFiller(ComponentFiller& other) = delete;

    /// Create a ComponentFiller for a Component
    explicit ComponentFiller(const ModelerStudy::SystemModel::Component& component,
                             Optimization::VariableDictionary& variableDictionary,
                             const LinearProblemApi::ILinearProblemData& data,
                             const ScenarioGroupRepository& scenarioGroupRepository);

    void addVariables(Optimisation::LinearProblemApi::ILinearProblem& pb,
                      const Optimisation::LinearProblemApi::FillContext& ctx) override;

    void addConstraints(Optimisation::LinearProblemApi::ILinearProblem& pb,
                        const Optimisation::LinearProblemApi::FillContext& ctx) override;

    void addObjective(Optimisation::LinearProblemApi::ILinearProblem& pb,
                      const Optimisation::LinearProblemApi::FillContext& ctx) override;

private:
    void addStaticConstraint(Optimisation::LinearProblemApi::ILinearProblem& pb,
                             const Optimization::LinearConstraint& linear_constraint,
                             const std::string& constraint_id) const;

    void addTimeDependentConstraints(
      Optimisation::LinearProblemApi::ILinearProblem& pb,
      const std::vector<Optimization::LinearConstraint>& linear_constraints,
      const std::string& constraint_id) const;

    bool IsThisConstraintTimeDependent(const Expressions::Nodes::Node* node) const;

    const ModelerStudy::SystemModel::Component& component_;
    Optimization::VariableDictionary& variableDictionary_;
    const EvaluationContextProvider evaluationContextProvider_;
};

class VariablesBulkAddition
{
public:
    VariablesBulkAddition(Optimisation::LinearProblemApi::ILinearProblem& linear_problem,
                          Optimization::VariableDictionary& variableDictionary);

    void addVariable(double lb,
                     double ub,
                     bool integer,
                     const Optimization::Dimensions& dim,
                     const Optimization::PartialKey&) const;

    void addVariable(const std::vector<double>& lb,
                     double ub,
                     bool integer,
                     const Optimization::Dimensions& dim,
                     const Optimization::PartialKey&) const;

    void addVariable(double lb,
                     const std::vector<double>& ub,
                     bool integer,
                     const Optimization::Dimensions& dim,
                     const Optimization::PartialKey&) const;

    void addVariable(const std::vector<double>& lb,
                     const std::vector<double>& ub,
                     bool integer,
                     const Optimization::Dimensions& dim,
                     const Optimization::PartialKey&) const;

    class BoundsSizeMismatch: public std::invalid_argument
    {
        using std::invalid_argument::invalid_argument;
    };

private:
    Optimisation::LinearProblemApi::ILinearProblem& linear_problem_;
    Optimization::VariableDictionary& variableDictionary;
};
} // namespace Antares::Optimisation
