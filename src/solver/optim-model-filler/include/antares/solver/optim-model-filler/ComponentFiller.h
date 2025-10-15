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
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/solver/optim-model-filler/Dimensions.h"

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
                             OptimEntityContainer& optimEntityContainer,
                             const ScenarioGroupRepository& scenarioGroupRepository);

    void addVariables(const Optimisation::LinearProblemApi::FillContext& ctx) override;

    void addConstraints(const Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addObjectives(const Optimisation::LinearProblemApi::FillContext& ctx) override;

private:
    void addStaticConstraint(const Optimisation::LinearConstraint& linear_constraint,
                             const std::string& constraint_id);

    void addTimeDependentConstraints(const Optimisation::LinearConstraint& linear_constraints,
                                     const std::string& constraint_id,
                                     const Optimisation::LinearProblemApi::FillContext& ctx);

    TimeIndex getConstraintTimeIndex(const Expressions::Nodes::Node* node,
                                     const ModelerStudy::SystemModel::Component& component) const;

    const ModelerStudy::SystemModel::Component& component_;
    OptimEntityContainer& optimEntityContainer_;
    const ScenarioGroupRepository& scenarioGroupRepository_;
};

class VariablesBulkAddition
{
public:
    VariablesBulkAddition(Optimisation::LinearProblemApi::ILinearProblem& linear_problem,
                          OptimEntityContainer& optimEntityContainer);

    void addVariable(const std::string& compoId,
                     const std::string& variableId,
                     double lb,
                     double ub,
                     bool integer,
                     const Optimisation::Dimensions& dim) const;

    void addVariable(const std::string& compoId,
                     const std::string& variableId,
                     const std::vector<double>& lb,
                     double ub,
                     bool integer,
                     const Optimisation::Dimensions& dim) const;

    void addVariable(const std::string& compoId,
                     const std::string& variableId,
                     double lb,
                     const std::vector<double>& ub,
                     bool integer,
                     const Optimisation::Dimensions& dim) const;

    void addVariable(const std::string& compoId,
                     const std::string& variableId,
                     const std::vector<double>& lb,
                     const std::vector<double>& ub,
                     bool integer,
                     const Optimisation::Dimensions& dim) const;

    class BoundsSizeMismatch: public std::invalid_argument
    {
        using std::invalid_argument::invalid_argument;
    };

private:
    Optimisation::LinearProblemApi::ILinearProblem& linear_problem_;
    OptimEntityContainer& optimEntityContainer_;
};
} // namespace Antares::Optimisation
