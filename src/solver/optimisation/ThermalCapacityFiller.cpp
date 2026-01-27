// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/ThermalCapacityFiller.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include "antares/exception/RuntimeError.hpp"
#include "antares/solver/optim-model-filler/ReadLinearExpressionVisitor.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemApi;

namespace Antares::Optimization
{
ThermalCapacityFiller::ThermalCapacityFiller(
  PROBLEME_HEBDO* problemeHebdo,
  OptimEntityContainer& optimEntityContainer,
  const ILinearProblemData& linearProblemData,
  const Optimisation::ScenarioGroupRepository& scenarioGroupRepository):
    problemeHebdo_(problemeHebdo),
    modelerSystem_(problemeHebdo->modelerData->system.get()),
    optimEntityContainer_(optimEntityContainer),
    variableManager_(problemeHebdo->CorrespondanceVarNativesVarOptim,
                     problemeHebdo->NumeroDeVariableStockFinal,
                     problemeHebdo->NumeroDeVariableDeTrancheDeStock,
                     problemeHebdo->NombreDePasDeTempsPourUneOptimisation)
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

static std::string getConnectionFieldId(const ModelerStudy::SystemModel::Component& component,
                                        const std::string& portId)
{
    auto field = component.getModel()->Ports().at(portId).Type().AreaConnectionFieldId();
    if (!field.has_value())
    {
        throw Error::RuntimeError("Component \"" + component.Id()
                                  + "\" is connected to an area using a port type that has no "
                                    "area-connection field defined.");
    }
    return field.value();
}

void ThermalCapacityFiller::addConstraints(const FillContext& ctx)
{
    //
}

void ThermalCapacityFiller::addObjectives(const FillContext&)
{
    // nothing to do
}

void ThermalCapacityFiller::setDispatchableProductionUpperBoundToInf(const std::string& areaId,
                                                                     const std::string& clusterId,
                                                                     const FillContext& ctx)
{
    auto pays = areaIndices_[areaId];
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo_
                                                          ->PaliersThermiquesDuPays[pays];
    auto index = clusters_[areaId][clusterId];
    const int palier = PaliersThermiquesDuPays
                         .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
    for (auto localIndex(ctx.getLocalFirstTimeStep()); localIndex <= ctx.getLocalLastTimeStep();
         ++localIndex)
    {
        auto pdt = localIndex % problemeHebdo_->NombreDePasDeTempsPourUneOptimisation;
        auto index = variableManager_.DispatchableProduction(palier, pdt);
        auto& linearProblem = optimEntityContainer_.Problem();
        auto* dispatchableProduction = linearProblem.getVariable(index);
        dispatchableProduction->setUb(linearProblem.infinity());
        linearProblem.
    }
}
} // namespace Antares::Optimization
