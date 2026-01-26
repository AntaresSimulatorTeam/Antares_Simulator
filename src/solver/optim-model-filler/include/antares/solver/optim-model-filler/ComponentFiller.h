// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
                             Solver::Config::Location targetLocation,
                             BendersDecomposition* bendersDecomposition = nullptr);

    void addVariables(const LinearProblemApi::FillContext& ctx) override;

    void addConstraints(const LinearProblemApi::FillContext& ctx) override;
    void addObjectives(const LinearProblemApi::FillContext& ctx) override;

private:
    void addStaticConstraint(const LinearConstraint& linear_constraint,
                             const std::string& constraint_id) const;

    void addTimeDependentConstraints(const LinearConstraint& linear_constraints,
                                     const std::string& constraint_id,
                                     const LinearProblemApi::FillContext& ctx) const;

    void addStaticObjective(const Optimization::LinearExpression& expression) const;

    VariabilityType getVariability(const Nodes::Node* node,
                                   const ModelerStudy::SystemModel::Component& component) const;

    const ModelerStudy::SystemModel::Component& component_;
    OptimEntityContainer& optimEntityContainer_;
    const ScenarioGroupRepository& scenarioGroupRepository_;
    const Solver::Config::Location targetLocation_;
    BendersDecomposition* bendersDecomposition_ = nullptr;

    // Filter to keep only items compatible with the target location
    auto locationFilter()
    {
        return std::views::filter(
          [this](const auto& item)
          { return AreLocationsCompatibleForFillers(item.location(), targetLocation_); });
    }
};
} // namespace Antares::Optimisation
