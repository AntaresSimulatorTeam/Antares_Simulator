// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/ThermalCapacityFiller.h"

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include "antares/exception/RuntimeError.hpp"
#include "antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h"
#include "antares/solver/optimisation/variables/VariableManagerUtils.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemApi;
using namespace Antares::ModelerStudy::SystemModel;

namespace Antares::Optimization
{
ThermalCapacityFiller::ThermalCapacityFiller(PROBLEME_HEBDO* problemeHebdo,
                                             OptimEntityContainer& optimEntityContainer,
                                             const ILinearProblemData* data,
                                             const ScenarioGroupRepository& scenarioGroupRepo):
    problemeHebdo_(problemeHebdo),
    modelerSystem_(problemeHebdo->modelerData->system.get()),
    optimEntityContainer_(optimEntityContainer),
    pb_(optimEntityContainer_.Problem()),
    data_(data),
    scenarioGroupRepo_(scenarioGroupRepo),
    variableManager_(VariableManagerFromProblemHebdo(problemeHebdo))
{
    unsigned int i = 0;
    for (auto name: problemeHebdo_->NomsDesPays)
    {
        unsigned int j = 0;
        const auto& palierThermiques = problemeHebdo_->PaliersThermiquesDuPays[i];
        std::unordered_map<std::string, unsigned int> clusters;
        for (const auto& nomPalier: palierThermiques.NomsDesPaliersThermiques)
        {
            clusters[nomPalier] = j;
            ++j;
        }
        if (!clusters.empty())
        {
            areasAndClusters_.try_emplace(name, AreaAndClusters{i, clusters});
        }
        ++i;
    }
}

void ThermalCapacityFiller::addVariables(const FillContext&)
{
    // nothing to do
}

static std::string getThermalCapacityField(const Component& component, const std::string& portId)
{
    auto field = component.getModel()->Ports().at(portId).Type().ThermalCapacityConnectionFieldId();
    if (!field.has_value())
    {
        throw Error::RuntimeError(fmt::format(
          "Component '{}' is connected to a thermal cluster using a port type that has no "
          "thermal-capacity-connection field defined.",
          component.Id()));
    }
    return field.value();
}

void ThermalCapacityFiller::addConstraints(const FillContext& ctx)
{
    for (const auto& component: modelerSystem_->Components())
    {
        for (const auto& [portId, thermalCapacityConnection]:
             component.portToThermalCapacityConnections())
        {
            addComponentPortContributionToThermalCapacity(ctx,
                                                          component,
                                                          portId,
                                                          thermalCapacityConnection);
        }
    }
}

void ThermalCapacityFiller::addObjectives(const FillContext&)
{
    // nothing to do
}

IMipVariable* ThermalCapacityFiller::getDispatchableProductionVariable(int palier, unsigned pdt)
{
    auto varIndex = variableManager_.DispatchableProduction(palier, pdt);
    return pb_.getVariable(varIndex);
}

void ThermalCapacityFiller::addCapacityFieldConstraint(
  const TimeDependentLinearExpression& linearExpression,
  const FillContext& ctx,
  const int clusterIndex,
  const std::string& namePrefix)
{
    const auto& solverVariables = pb_.getVariables();
    for (auto localIndex(ctx.getLocalFirstTimeStep()); localIndex <= ctx.getLocalLastTimeStep();
         ++localIndex)
    {
        auto pdt = localIndex % problemeHebdo_->NombreDePasDeTempsPourUneOptimisation;
        IMipVariable* dispatchableProduction = getDispatchableProductionVariable(clusterIndex, pdt);
        double infinity = pb_.infinity();
        dispatchableProduction->setUb(infinity);

        auto* ct = pb_.addConstraint(-infinity,
                                     linearExpression[localIndex].constant(),
                                     namePrefix
                                       + fmt::format("::hour<{}>",
                                                     pdt + problemeHebdo_->weekInTheYear * 168));
        ct->setCoefficient(dispatchableProduction, 1.0);

        for (const auto& [varIndex, coef]: linearExpression[localIndex])
        {
            ct->setCoefficient(solverVariables[varIndex].get(), -coef);
        }
    }
}

ThermalCapacityFiller::AreaAndClusters* ThermalCapacityFiller::areaClusters(
  const std::string& areaId)
{
    const auto it = areasAndClusters_.find(areaId);
    if (it == areasAndClusters_.end())
    {
        throw Error::RuntimeError(
          fmt::format("unknown area '{}', is found in thermal-connection-capacity ", areaId));
    }
    return &it->second;
}

unsigned int getClusterLocalIndex(const std::string& areaId,
                                  const std::string& clusterId,
                                  const std::unordered_map<std::string, unsigned>& clusters)
{
    if (clusters.empty())
    {
        throw Error::RuntimeError(fmt::format(" area '{}' has not thermal clusters ", areaId));
    }
    const auto itv = clusters.find(clusterId);
    if (itv == clusters.end())
    {
        throw Error::RuntimeError(
          fmt::format(" area '{}' has not thermal cluster by the name '{}' ", areaId, clusterId));
    }
    return itv->second;
}

int ThermalCapacityFiller::getClusterIndex(const std::string& areaId, const std::string& clusterId)
{
    //--- area
    const auto& [pays, clusters] = *areaClusters(areaId);

    //--- cluster
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo_
                                                          ->PaliersThermiquesDuPays[pays];
    const auto clusterLocalIndex = getClusterLocalIndex(areaId, clusterId, clusters);
    return PaliersThermiquesDuPays
      .NumeroDuPalierDansLEnsembleDesPaliersThermiques[clusterLocalIndex];
}

// set up DispatchableProduction up to max and add
// DispatchableProduction[t] <= component.port.capacity-field (qui vaut
// my_thermal_invest.capacity_port.
// capacity = availability_factor * invested_capacity + already_installed_availability_factor *
// already_installed_capacity

void ThermalCapacityFiller::processThermalCapacityField(
  const TimeDependentLinearExpression& linearExpression,
  const ThermalComponent& thermalCapacityConnection,
  const FillContext& ctx)
{
    const int clusterIndex = getClusterIndex(thermalCapacityConnection.areaId,
                                             thermalCapacityConnection.clusterId);
    addCapacityFieldConstraint(
      linearExpression,
      ctx,
      clusterIndex,
      fmt::format("MaxGenerationFromCapacity::area<{}>::ThermalCluster<{}>",
                  thermalCapacityConnection.areaId,
                  thermalCapacityConnection.clusterId));
}

void ThermalCapacityFiller::addComponentPortContributionToThermalCapacity(
  const FillContext& ctx,
  const Component& component,
  const std::string& portId,
  const ThermalComponent& thermalCapacityConnection)
{
    std::string thermalCapacityField = getThermalCapacityField(component, portId);
    ReadLinearExpressionVisitor visitor(optimEntityContainer_,
                                        ctx,
                                        component,
                                        data_,
                                        scenarioGroupRepo_);

    const auto linearExpression = visitor.visitMergeDuplicates(
      component.nodeAtPortField(portId, thermalCapacityField));
    processThermalCapacityField(linearExpression, thermalCapacityConnection, ctx);
}
} // namespace Antares::Optimization
