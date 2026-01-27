// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/ThermalCapacityFiller.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include "antares/exception/RuntimeError.hpp"
#include "antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h"
#include "antares/solver/optimisation/variables/VariableManagerUtils.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemApi;

namespace Antares::Optimization
{
ThermalCapacityFiller::ThermalCapacityFiller(PROBLEME_HEBDO* problemeHebdo,
                                             OptimEntityContainer& optimEntityContainer):
    problemeHebdo_(problemeHebdo),
    modelerSystem_(problemeHebdo->modelerData->system.get()),
    optimEntityContainer_(optimEntityContainer),
    variableManager_(VariableManagerFromProblemHebdo(problemeHebdo))
{
    int i = 0;
    for (auto name: problemeHebdo_->NomsDesPays)
    {
        int j = 0;
        const auto& palierThermiques = problemeHebdo_->PaliersThermiquesDuPays[i];
        for (const auto& nomPalier: palierThermiques.NomsDesPaliersThermiques)
        {
            clusters_[name].try_emplace(nomPalier, j);
            j++;
        }

        areaIndices_[name] = i++;
    }
}

void ThermalCapacityFiller::addVariables(const FillContext&)
{
    // nothing to do
}

static std::string getThermalCapacityField(const ModelerStudy::SystemModel::Component& component,
                                           const std::string& portId)
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
        for (const auto& [portId, thermalConnection]: component.portToThermalConnections())
        {
            addComponentPortContributionToThermalCapacity(ctx,
                                                          component,
                                                          portId,
                                                          thermalConnection);
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
    return optimEntityContainer_.Problem().getVariable(varIndex);
}

// set up DispatchableProduction up to max and add
// DispatchableProduction[t] <= component.port.capacity-field (qui vaut
// my_thermal_invest.capacity_port.
// capacity = availability_factor * invested_capacity + already_installed_availability_factor *
// already_installed_capacity
void ThermalCapacityFiller::processThermalCapacityField(
  const TimeDependentLinearExpression& linearExpression,
  const ModelerStudy::SystemModel::Component::ThermalConnection& thermalConnection,
  const FillContext& ctx)
{
    //--- area
    auto areaId = thermalConnection.areaId;
    boost::algorithm::to_lower(areaId);
    auto pays = areaIndices_[areaId];
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo_
                                                          ->PaliersThermiquesDuPays[pays];
    //--- cluster
    auto clusterId = thermalConnection.clusterId;
    boost::algorithm::to_lower(clusterId);
    auto index = clusters_[areaId][clusterId];

    const int palier = PaliersThermiquesDuPays
                         .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
    auto& linearProblem = optimEntityContainer_.Problem();
    const auto& solverVariables = optimEntityContainer_.getVariables();

    // TODO check size == 1 or == ctx.getLocalLastTimeStep() - ctx.getLocalFirstTimeStep() + 1
    for (auto localIndex(ctx.getLocalFirstTimeStep()); localIndex <= ctx.getLocalLastTimeStep();
         ++localIndex)
    {
        auto pdt = localIndex % problemeHebdo_->NombreDePasDeTempsPourUneOptimisation;
        IMipVariable* dispatchableProduction = getDispatchableProductionVariable(palier, pdt);
        double infinity = linearProblem.infinity();
        dispatchableProduction->setUb(infinity);
        // TODO the name
        auto* ct = linearProblem.addConstraint(-infinity,
                                               linearExpression[localIndex].constant(),
                                               fmt::format("ThermalCapacity_{}", pdt));
        ct->setCoefficient(dispatchableProduction, 1.0);

        for (const auto& [varIndex, coef]: linearExpression[localIndex])
        {
            ct->setCoefficient(solverVariables[varIndex].get(), -coef);
        }
    }
}

void ThermalCapacityFiller::addComponentPortContributionToThermalCapacity(
  const Optimisation::LinearProblemApi::FillContext& ctx,
  const ModelerStudy::SystemModel::Component& component,
  const std::string& portId,
  const ModelerStudy::SystemModel::Component::ThermalConnection& thermalConnection)
{
    std::string thermalCapacityField = getThermalCapacityField(component, portId);
    ReadLinearExpressionVisitor visitor(optimEntityContainer_, ctx, component);
    const auto linearExpression = visitor.visitMergeDuplicates(
      component.nodeAtPortField(portId, thermalCapacityField));
    processThermalCapacityField(linearExpression, thermalConnection, ctx);
}
} // namespace Antares::Optimization
