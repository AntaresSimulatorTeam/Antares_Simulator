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

#pragma once

#include <antares/optimisation/linear-problem-api/linearProblemFiller.h>
#include <antares/study/system-model/component.h>
#include "antares/expressions/visitors/EvaluationContext.h"

#include "ReadLinearConstraintVisitor.h"

namespace Antares::Study::SystemModel
{
class Component;
class Variable;
} // namespace Antares::Study::SystemModel

namespace Antares::Expressions::Visitors
{
class EvalVisitor;
}

namespace Antares::Optimization
{
/**
 * Component filler
 * Implements LinearProblemFiller interface.
 * Fills a LinearProblem with variables, constraints, and objective coefficients of a Component
 */
class ComponentFiller: public Optimisation::LinearProblemApi::LinearProblemFiller
{
public:
    ComponentFiller() = delete;
    ComponentFiller(ComponentFiller& other) = delete;
    /// Create a ComponentFiller for a Component
    explicit ComponentFiller(const Study::SystemModel::Component& component);

    void addVariables(Optimisation::LinearProblemApi::ILinearProblem& pb,
                      Optimisation::LinearProblemApi::ILinearProblemData& data,
                      Optimisation::LinearProblemApi::FillContext& ctx) override;

    void addStaticConstraint(Optimisation::LinearProblemApi::ILinearProblem& pb,
                             const LinearConstraint& linear_constraint,
                             const std::string& constraint_id) const;

    void addTimeDependentConstraints(Optimisation::LinearProblemApi::ILinearProblem& pb,
                                     const LinearConstraint& linear_constraint,
                                     const std::string& constraint_id,
                                     unsigned int nb_cstr) const;

    void addConstraints(Optimisation::LinearProblemApi::ILinearProblem& pb,
                        Optimisation::LinearProblemApi::ILinearProblemData& data,
                        Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addObjective(Optimisation::LinearProblemApi::ILinearProblem& pb,
                      Optimisation::LinearProblemApi::ILinearProblemData& data,
                      Optimisation::LinearProblemApi::FillContext& ctx) override;

private:
    static bool IsThisConstraintTimeDependent(const Expressions::Nodes::Node* node);

    bool IsThisVariableTimeDependent(const std::string& var_id) const;

    const Study::SystemModel::Component& component_;
    Expressions::Visitors::EvaluationContext evaluationContext_;
    const std::map<std::string, Study::SystemModel::Variable>& modelVariable_;
};
} // namespace Antares::Optimization
