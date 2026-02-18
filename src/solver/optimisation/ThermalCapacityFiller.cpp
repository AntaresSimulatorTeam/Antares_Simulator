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
                                             OptimEntityContainer& optimEntityContainer):
    problemeHebdo_(problemeHebdo),
    modelerSystem_(problemeHebdo->modelerData->system.get()),
    optimEntityContainer_(optimEntityContainer),
    variableManager_(VariableManagerFromProblemHebdo(problemeHebdo))
{
    unsigned int areaIndex = 0;
    for (const auto* areaName: problemeHebdo_->NomsDesPays)
    {
        unsigned int thermalClusterLocalIndex = 0;
        const auto& thermalClusters = problemeHebdo_->PaliersThermiquesDuPays[areaIndex];
        std::unordered_map<std::string, unsigned int> clusters;
        for (const auto& clusterName: thermalClusters.NomsDesPaliersThermiques)
        {
            clusters[clusterName] = thermalClusters.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                                      [thermalClusterLocalIndex];
            ++thermalClusterLocalIndex;
        }
        if (!clusters.empty())
        {
            areasAndClusters_.try_emplace(areaName, AreaAndClusters{areaIndex, clusters});
        }
        ++areaIndex;
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

IMipVariable* ThermalCapacityFiller::getDispatchableProductionVariable(int thermalClusterIndex,
                                                                       unsigned pdt)
{
    auto varIndex = variableManager_.DispatchableProduction(thermalClusterIndex, pdt);
    return optimEntityContainer_.Problem().getVariable(varIndex);
}

void ThermalCapacityFiller::addCapacityFieldConstraint(
  const TimeDependentLinearExpression& linearExpression,
  const FillContext& ctx,
  const int clusterIndex,
  const std::string& namePrefix)
{
    auto& linearProblem = optimEntityContainer_.Problem();
    const auto& solverVariables = optimEntityContainer_.getVariables();
    for (auto localIndex(ctx.getLocalFirstTimeStep()); localIndex <= ctx.getLocalLastTimeStep();
         ++localIndex)
    {
        auto pdt = localIndex % problemeHebdo_->NombreDePasDeTempsPourUneOptimisation;
        IMipVariable* dispatchableProduction = getDispatchableProductionVariable(clusterIndex, pdt);
        double infinity = linearProblem.infinity();
        dispatchableProduction->setUb(infinity);

        auto* ct = linearProblem.addConstraint(
          -infinity,
          linearExpression[localIndex].constant(),
          namePrefix + fmt::format("::hour<{}>", pdt + problemeHebdo_->weekInTheYear * 168));
        ct->setCoefficient(dispatchableProduction, 1.0);

        for (const auto& [varIndex, coef]: linearExpression[localIndex])
        {
            ct->setCoefficient(solverVariables[varIndex].get(), -coef);
        }
    }
}

ThermalCapacityFiller::AreaAndClusters& ThermalCapacityFiller::areaClusters(
  const std::string& areaId)
{
    const auto it = areasAndClusters_.find(areaId);
    if (it == areasAndClusters_.end())
    {
        throw Error::RuntimeError(
          fmt::format("unknown area '{}', is found in thermal-connection-capacity ", areaId));
    }
    return it->second;
}

unsigned int clusterAt(const std::string& areaId,
                       const std::string& clusterId,
                       const std::unordered_map<std::string, unsigned>& clusters)
{
    if (clusters.empty())
    {
        throw Error::RuntimeError(fmt::format(" area '{}' has not thermal clusters ", areaId));
    }
    const auto it = clusters.find(clusterId);
    if (it == clusters.end())
    {
        throw Error::RuntimeError(
          fmt::format(" area '{}' has not thermal cluster by the name '{}' ", areaId, clusterId));
    }
    return it->second;
}

int ThermalCapacityFiller::getClusterIndex(const std::string& areaId, const std::string& clusterId)
{
    const auto& [_, clusters] = areaClusters(areaId);
    return clusterAt(areaId, clusterId, clusters);
}

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
    ReadLinearExpressionVisitor visitor(optimEntityContainer_, ctx, component);
    const auto linearExpression = visitor.visitMergeDuplicates(
      component.nodeAtPortField(portId, thermalCapacityField));
    processThermalCapacityField(linearExpression, thermalCapacityConnection, ctx);
}
} // namespace Antares::Optimization
