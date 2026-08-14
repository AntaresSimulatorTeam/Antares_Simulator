// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

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
} // namespace Api
class OptimEntityContainer;
} // namespace Antares::LinearProblem

namespace Antares::Optimization
{

/**
 * \brief Fills the linear problem with constraints and variables related to
 * thermal capacity connections.
 * This class is responsible for adding constraints to the linear problem
 * based on the connections between components and (areas, thermal clusters) in the Antares study.
 */
class ThermalCapacityFiller final: public LinearProblem::Api::LinearProblemFiller
{
public:
    explicit ThermalCapacityFiller(PROBLEME_HEBDO* problemeHebdo,
                                   LinearProblem::OptimEntityContainer& variableContainer,
                                   const LinearProblem::Api::ILinearProblemData* data,
                                   const LinearProblem::ScenarioGroupRepository& scenarioGroupRepo);

    void addVariables(const LinearProblem::Api::FillContext& ctx) override;
    void addConstraints(const LinearProblem::Api::FillContext& ctx) override;
    void addObjectives(const LinearProblem::Api::FillContext& ctx) override;

private:
    const PROBLEME_HEBDO* problemeHebdo_;
    const ModelerStudy::SystemModel::System* modelerSystem_;
    LinearProblem::OptimEntityContainer& optimEntityContainer_;
    LinearProblem::Api::ILinearProblem& pb_;
    const LinearProblem::Api::ILinearProblemData* data_;
    const LinearProblem::ScenarioGroupRepository& scenarioGroupRepo_;

    void processThermalCapacityField(
      const TimeDependentLinearExpression& linearExpression,
      const ModelerStudy::SystemModel::ThermalComponent& thermalCapacityConnection,
      const LinearProblem::Api::FillContext& ctx);

    void addComponentPortContributionToThermalCapacity(
      const LinearProblem::Api::FillContext& ctx,
      const ModelerStudy::SystemModel::Component& component,
      const std::string& portId,
      const ModelerStudy::SystemModel::ThermalComponent& thermalCapacityConnection);
    LinearProblem::Api::IMipVariable* getDispatchableProductionVariable(int thermalClusterIndex,
                                                                        unsigned pdt);
    void addCapacityFieldConstraint(const TimeDependentLinearExpression& linearExpression,
                                    const LinearProblem::Api::FillContext& ctx,
                                    int clusterIndex,
                                    const std::string& namePrefix);

    int getClusterIndex(const std::string& areaId, const std::string& clusterId);

    VariableManagement::VariableManager variableManager_;
};

} // namespace Antares::Optimization
