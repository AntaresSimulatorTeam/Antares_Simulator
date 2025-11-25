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
#include <antares/study/system-model/variable.h>
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/modeler-optimisation-container/scenarioGroupRepo.h"
#include "antares/solver/optim-model-filler/Dimensions.h"

#include "ReadLinearConstraintVisitor.h"

namespace Antares::Optimisation
{

// Represents a variable shared by master and subproblems
struct ConnectionVariable
{
    std::string name;
    unsigned indexInProblem;
};

class BendersDecomposition
{
public:
    BendersDecomposition() = default;
    void setCurrentProblemId(std::string id);
    void collectConnectionVariables(std::vector<std::string>&& varnames, unsigned varsCountInPb);

    const std::map<std::string, std::vector<ConnectionVariable>>& connections() const
    {
        return connectionVars_;
    }

private:
    std::map<std::string, std::vector<ConnectionVariable>> connectionVars_;
    std::string currentProblemId_ = "master";
};

class BendersDecompositionWriter
{
public:
    BendersDecompositionWriter(const BendersDecomposition& bd);
    void write(std::ostream& os) const;

private:
    const BendersDecomposition& bd_;
};

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

    explicit ComponentFiller(const ModelerStudy::SystemModel::Component& component,
                             OptimEntityContainer& optimEntityContainer,
                             const ScenarioGroupRepository& scenarioGroupRepository,
                             Modeler::Config::Location targetLocation,
                             BendersDecomposition* bendersDecomposition = nullptr);

    void addVariables(const LinearProblemApi::FillContext& ctx) override;

    void addConstraints(const LinearProblemApi::FillContext& ctx) override;
    void addObjectives(const LinearProblemApi::FillContext& ctx) override;

private:
    void addStaticConstraint(const LinearConstraint& linear_constraint,
                             const std::string& constraint_id);

    void addTimeDependentConstraints(const LinearConstraint& linear_constraints,
                                     const std::string& constraint_id,
                                     const LinearProblemApi::FillContext& ctx);

    void addStaticObjective(const Optimization::TimeDependentLinearExpression& expression) const;

    TimeIndex getConstraintTimeIndex(const Nodes::Node* node,
                                     const ModelerStudy::SystemModel::Component& component) const;

    const ModelerStudy::SystemModel::Component& component_;
    OptimEntityContainer& optimEntityContainer_;
    const ScenarioGroupRepository& scenarioGroupRepository_;
    const Modeler::Config::Location targetLocation_;
    BendersDecomposition* bendersDecomposition_ = nullptr;

    // Filter to keep only items compatible with the target location
    auto locationFilter()
    {
        return std::views::filter(
          [this](const auto& item)
          { return AreLocationsCompatible(item.location(), targetLocation_); });
    }
};
} // namespace Antares::Optimisation
