// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/modeler-optimisation-container/scenarioGroupRepo.h"
#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"
#include "antares/solver/optim-model-filler/TimeDependentLinearExpression.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/study/system-model/system.h"

#include "variables/VariableManagement.h"

namespace Antares::LinearProblem
{
namespace Api
{
class ILinearProblem;
class IMipConstraint;
class IMipVariable;
} // namespace Api
class OptimEntityContainer;
} // namespace Antares::LinearProblem

namespace Antares::Optimization
{

class LinkCapacityFiller final: public LinearProblem::Api::LinearProblemFiller
{
public:
    explicit LinkCapacityFiller(PROBLEME_HEBDO* problemeHebdo,
                                LinearProblem::OptimEntityContainer& variableContainer,
                                const LinearProblem::Api::ILinearProblemData* data,
                                const LinearProblem::ScenarioGroupRepository& scenarioGroupRepo);

    void addVariables(const LinearProblem::Api::FillContext& ctx) override;
    void addConstraints(const LinearProblem::Api::FillContext& ctx) override;
    void addObjectives(const LinearProblem::Api::FillContext& ctx) override;

private:
    PROBLEME_HEBDO* problemeHebdo_;
    const ModelerStudy::SystemModel::System* modelerSystem_;
    LinearProblem::OptimEntityContainer& optimEntityContainer_;
    LinearProblem::Api::ILinearProblem& pb_;
    const LinearProblem::Api::ILinearProblemData* data_;
    const LinearProblem::ScenarioGroupRepository& scenarioGroupRepo_;
    VariableManagement::VariableManager variableManager_;

    int getIntercoIndex(const std::string& from, const std::string& to) const;

    void addComponentPortContributionToLinkCapacity(
      const LinearProblem::Api::FillContext& ctx,
      const ModelerStudy::SystemModel::Component& component,
      const std::string& portId,
      const ModelerStudy::SystemModel::LinkComponent& linkConnection);
};

/**
 * @brief Report the solved link capacity constraints on the legacy link structures.
 *
 * LinkCapacityFiller leaves the flow variable unbounded, so its reduced cost and the legacy NTC
 * no longer describe the link. Writes the effective capacities into ValeursDeNTC and the duals of
 * the capacity constraints into the flow variable's reduced cost, from where applyResults copies
 * them to VariablesDualesDesContraintesDeNTC.
 */
void applyLinkCapacityResults(PROBLEME_HEBDO& problemeHebdo,
                              const LinearProblem::Api::ILinearProblem& pb,
                              int NumIntervalle);

} // namespace Antares::Optimization
