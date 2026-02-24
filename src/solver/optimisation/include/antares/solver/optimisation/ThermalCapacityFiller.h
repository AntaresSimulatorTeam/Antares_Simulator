// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"
#include "antares/solver/optim-model-filler/TimeDependentLinearExpression.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/study/system-model/system.h"

#include "variables/VariableManagement.h"

namespace Antares::Optimisation
{
namespace LinearProblemApi
{
class ILinearProblem;
class IMipConstraint;
} // namespace LinearProblemApi
class OptimEntityContainer;
} // namespace Antares::Optimisation

namespace Antares::Optimization
{

/**
 * \brief Fills the linear problem with constraints and variables related to
 * thermal capacity connections.
 * This class is responsible for adding constraints to the linear problem
 * based on the connections between components and (areas, thermal clusters) in the Antares study.
 */
class ThermalCapacityFiller final: public Optimisation::LinearProblemApi::LinearProblemFiller
{
public:
    explicit ThermalCapacityFiller(PROBLEME_HEBDO* problemeHebdo,
                                   Optimisation::OptimEntityContainer& variableContainer);
    void addVariables(const Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addConstraints(const Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addObjectives(const Optimisation::LinearProblemApi::FillContext& ctx) override;

private:
    const PROBLEME_HEBDO* problemeHebdo_;
    const ModelerStudy::SystemModel::System* modelerSystem_;
    Optimisation::OptimEntityContainer& optimEntityContainer_;
    void processThermalCapacityField(
      const TimeDependentLinearExpression& linearExpression,
      const ModelerStudy::SystemModel::ThermalComponent& thermalCapacityConnection,
      const Optimisation::LinearProblemApi::FillContext& ctx);

    void addComponentPortContributionToThermalCapacity(
      const Optimisation::LinearProblemApi::FillContext& ctx,
      const ModelerStudy::SystemModel::Component& component,
      const std::string& portId,
      const ModelerStudy::SystemModel::ThermalComponent& thermalCapacityConnection);
    Optimisation::LinearProblemApi::IMipVariable* getDispatchableProductionVariable(
      int thermalClusterIndex,
      unsigned pdt);
    void addCapacityFieldConstraint(const TimeDependentLinearExpression& linearExpression,
                                    const Optimisation::LinearProblemApi::FillContext& ctx,
                                    int clusterIndex,
                                    const std::string& namePrefix);

    int getClusterIndex(const std::string& areaId, const std::string& clusterId);

    VariableManagement::VariableManager variableManager_;
};

} // namespace Antares::Optimization
