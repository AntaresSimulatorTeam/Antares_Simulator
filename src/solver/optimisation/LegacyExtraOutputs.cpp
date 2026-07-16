// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacyExtraOutputs.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <vector>

#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/optimisation/variables/VariableManagerUtils.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using Antares::IO::Outputs::SimulationTable;
using Antares::Optimisation::LinearProblemApi::FillContext;

namespace Antares::Optimization
{

std::optional<unsigned> LegacyBlockTimeIndex(const FillContext& fillContext, unsigned timeIndex)
{
    if (timeIndex >= fillContext.getGlobalFirstTimeStep()
        && timeIndex <= fillContext.getGlobalLastTimeStep())
    {
        return timeIndex - fillContext.getGlobalFirstTimeStep();
    }
    return std::nullopt;
}

namespace
{
// Emission extra-output IDs, ordered to match Antares::Data::Pollutant::PollutantEnum
// so each pollutant's factor (read by ordinal from the cluster data) maps to its row.
constexpr std::array<const char*, Data::Pollutant::POLLUTANT_MAX> emissionOutputNames = {
  "co2_emissions",
  "nh3_emissions",
  "so2_emissions",
  "nox_emissions",
  "pm2_5_emissions",
  "pm5_emissions",
  "pm10_emissions",
  "nmvoc_emissions",
  "op1_emissions",
  "op2_emissions",
  "op3_emissions",
  "op4_emissions",
  "op5_emissions"};

class LegacyExtraOutputEmitter
{
public:
    LegacyExtraOutputEmitter(SimulationTable& simulationTable,
                             PROBLEME_HEBDO& problemeHebdo,
                             const FillContext& fillContext,
                             unsigned currentBlock):
        table_(simulationTable),
        problemeHebdo_(problemeHebdo),
        problem_(*problemeHebdo.ProblemeAResoudre),
        variableManager_(VariableManagerFromProblemHebdo(&problemeHebdo)),
        fillContext_(fillContext),
        block_(currentBlock)
    {
        // Component names are used for every time step of the week: build them
        // once instead of re-concatenating them on each hourly call.
        areaNames_.reserve(problemeHebdo.NombreDePays);
        hydroStorageNames_.reserve(problemeHebdo.NombreDePays);
        thermalNames_.reserve(problemeHebdo.NombreDePays);
        for (uint32_t pays = 0; pays < problemeHebdo.NombreDePays; ++pays)
        {
            const std::string area = problemeHebdo.NomsDesPays[pays];
            areaNames_.push_back(area + "_node");
            hydroStorageNames_.push_back(area + "_hydro_storage");

            const PALIERS_THERMIQUES& paliers = problemeHebdo.PaliersThermiquesDuPays[pays];
            auto& clusterNames = thermalNames_.emplace_back();
            clusterNames.reserve(paliers.NombreDePaliersThermiques);
            for (int index = 0; index < paliers.NombreDePaliersThermiques; ++index)
            {
                clusterNames.push_back(area + "_thermal_"
                                       + paliers.NomsDesPaliersThermiques[index]);
            }
        }

        linkNames_.reserve(problemeHebdo.NombreDInterconnexions);
        for (uint32_t interco = 0; interco < problemeHebdo.NombreDInterconnexions; ++interco)
        {
            const std::string o = problemeHebdo
                                    .NomsDesPays[problemeHebdo
                                                   .PaysOrigineDeLInterconnexion[interco]];
            const std::string d = problemeHebdo
                                    .NomsDesPays[problemeHebdo
                                                   .PaysExtremiteDeLInterconnexion[interco]];
            const auto& [a1, a2] = (o < d) ? std::tie(o, d) : std::tie(d, o);
            linkNames_.push_back(a1 + "_" + a2 + "_link");
        }
    }

    void areaOutputs(uint32_t pays, int pdt);
    void linkOutputs(uint32_t interco, int pdt);
    void thermalOutputs(uint32_t pays, int index, int pdt);
    void weeklyHydroOutputs(uint32_t pays) const;

private:
    void emit(const std::string& output, const std::string& component, int pdt, double value) const;

    [[nodiscard]] double x(int variableIndex) const
    {
        return problem_.X[static_cast<std::size_t>(variableIndex)];
    }

    [[nodiscard]] double cost(int variableIndex) const
    {
        return problem_.CoutLineaire[static_cast<std::size_t>(variableIndex)];
    }

    [[nodiscard]] double dual(int constraintIndex) const
    {
        return problem_.CoutsMarginauxDesContraintes[static_cast<std::size_t>(constraintIndex)];
    }

    [[nodiscard]] double areaPrice(uint32_t pays, int pdt) const
    {
        return -dual(problemeHebdo_.CorrespondanceCntNativesCntOptim[pdt]
                       .NumeroDeContrainteDesBilansPays[pays]);
    }

    SimulationTable& table_;
    PROBLEME_HEBDO& problemeHebdo_;
    const PROBLEME_ANTARES_A_RESOUDRE& problem_;
    VariableManagement::VariableManager variableManager_;
    const FillContext& fillContext_;
    unsigned block_;

    // Component names, precomputed once per week (see constructor).
    std::vector<std::string> areaNames_;
    std::vector<std::string> hydroStorageNames_;
    std::vector<std::vector<std::string>> thermalNames_;
    std::vector<std::string> linkNames_;
};

void LegacyExtraOutputEmitter::emit(const std::string& output,
                                    const std::string& component,
                                    int pdt,
                                    double value) const
{
    const unsigned hour = problemeHebdo_.weekInTheYear * 168 + static_cast<unsigned>(pdt);
    table_.addEntry({.block = block_,
                     .component = component,
                     .output = output,
                     .absolute_time_index = hour,
                     .block_time_index = LegacyBlockTimeIndex(fillContext_, hour),
                     .scenario_index = fillContext_.getYear(),
                     .value = value,
                     .status = std::nullopt});
}

void LegacyExtraOutputEmitter::areaOutputs(uint32_t pays, int pdt)
{
    const std::string& area = areaNames_[pays];

    const int unsupplied = variableManager_.UnsuppliedEnergy(pays, pdt);
    const int spillage = variableManager_.Spillage(pays, pdt);

    emit("imbalance_cost",
         area,
         pdt,
         cost(spillage) * x(spillage) + cost(unsupplied) * x(unsupplied));

    constexpr double lossOfLoadThreshold = 0.5;
    emit("is_loss_of_load", area, pdt, x(unsupplied) > lossOfLoadThreshold ? 1. : 0.);
    emit("actual_load",
         area,
         pdt,
         problemeHebdo_.ConsommationsAbattues[pdt].ConsommationAbattueDuPays[pays]
           + problemeHebdo_.AllMustRunGeneration[pdt].AllMustRunGenerationOfArea[pays]);

    const double price = areaPrice(pays, pdt);
    emit("price", area, pdt, price);

    constexpr double nearLossOfLoadCutoff = 5.;
    emit("is_near_loss_of_load",
         area,
         pdt,
         price > cost(unsupplied) - nearLossOfLoadCutoff ? 1. : 0.);

    const int hydroLevel = variableManager_.HydroLevel(pays, pdt);
    if (hydroLevel < 0)
    {
        return;
    }
    const auto& hydro = problemeHebdo_.CaracteristiquesHydrauliques[pays];
    const std::string& hydroComponent = hydroStorageNames_[pays];
    if (hydro.TailleReservoir > 0.)
    {
        emit("level_percentage", hydroComponent, pdt, x(hydroLevel) / hydro.TailleReservoir * 100.);
    }
    if (static_cast<std::size_t>(pdt) < hydro.ApportNaturelHoraire.size())
    {
        emit("actual_inflows", hydroComponent, pdt, std::round(hydro.ApportNaturelHoraire[pdt]));
    }
}

void LegacyExtraOutputEmitter::linkOutputs(uint32_t interco, int pdt)
{
    const uint32_t origin = problemeHebdo_.PaysOrigineDeLInterconnexion[interco];
    const uint32_t destination = problemeHebdo_.PaysExtremiteDeLInterconnexion[interco];
    const std::string& link = linkNames_[interco];

    const double flow = x(variableManager_.DirectFlow(interco, pdt));
    emit("abs_flow", link, pdt, std::abs(flow));
    emit("minus_flow", link, pdt, -flow);

    const auto& ntc = problemeHebdo_.ValeursDeNTC[pdt];
    emit("actual_loop_flow", link, pdt, ntc.ValeurDeLoopFlowOrigineVersExtremite[interco]);

    constexpr double saturationEpsilon = 1e-5;
    emit("is_directly_congested",
         link,
         pdt,
         flow >= ntc.ValeurDeNTCOrigineVersExtremite[interco] - saturationEpsilon ? 1. : 0.);
    emit("is_indirectly_congested",
         link,
         pdt,
         -flow >= ntc.ValeurDeNTCExtremiteVersOrigine[interco] - saturationEpsilon ? 1. : 0.);

    const double priceDelta = areaPrice(destination, pdt) - areaPrice(origin, pdt);
    emit("abs_congestion_fee", link, pdt, std::abs(flow) * std::abs(priceDelta));
    emit("alg_congestion_fee", link, pdt, flow * priceDelta);

    if (!problemeHebdo_.CoutDeTransport[interco].IntercoGereeAvecDesCouts)
    {
        return;
    }

    const int positiveDirect = variableManager_.PositiveDirectFlow(interco, pdt);
    const int positiveIndirect = variableManager_.PositiveIndirectFlow(interco, pdt);
    emit("prop_cost",
         link,
         pdt,
         cost(positiveDirect) * x(positiveDirect) + cost(positiveIndirect) * x(positiveIndirect));

    const int dissociation = problemeHebdo_.CorrespondanceCntNativesCntOptim[pdt]
                               .NumeroDeContrainteDeDissociationDeFlux[interco];
    emit("capacity_shadow_price", link, pdt, std::abs(dual(dissociation)));
}

void LegacyExtraOutputEmitter::thermalOutputs(uint32_t pays, int index, int pdt)
{
    const PALIERS_THERMIQUES& paliers = problemeHebdo_.PaliersThermiquesDuPays[pays];
    const std::string& cluster = thermalNames_[pays][static_cast<std::size_t>(index)];
    const int palier = paliers.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

    const int production = variableManager_.DispatchableProduction(palier, pdt);
    const double generation = x(production);

    emit("prop_cost", cluster, pdt, cost(production) * generation);

    for (std::size_t pollutant = 0; pollutant < emissionOutputNames.size(); ++pollutant)
    {
        emit(emissionOutputNames[pollutant],
             cluster,
             pdt,
             generation * paliers.emissionFactors[index][pollutant]);
    }

    const auto& disp = paliers.PuissanceDisponibleEtCout[index];
    const auto& availabilitySeries = disp.PuissanceDisponibleDuPalierThermique;
    const auto& minGenSeries = disp.PuissanceMinDuPalierThermique;
    const double available = availabilitySeries[pdt];
    const double minGen = minGenSeries[pdt];
    const double unitSize = paliers.TailleUnitaireDUnGroupeDuPalierThermique[index];
    const double minStablePower = paliers.PminDuPalierThermiquePendantUneHeure[index];

    double unitFloor = 0.;
    if (unitSize > 0.)
    {
        unitFloor = minStablePower * std::ceil(available / unitSize);
    }
    const double clusterAvailability = std::max(available, unitFloor);

    emit("cluster_availability", cluster, pdt, clusterAvailability);
    emit("up_margin", cluster, pdt, clusterAvailability - generation);
    emit("min_gen_power", cluster, pdt, std::min(generation, minGen));
    emit("down_margin", cluster, pdt, generation - std::min(clusterAvailability, minGen));

    const double profit = (areaPrice(pays, pdt) - cost(production))
                          * std::max(generation - minGen, 0.);
    emit("profit", cluster, pdt, profit);

    if (!problemeHebdo_.OptimisationNotFastMode)
    {
        return;
    }

    const int nodu = variableManager_.NumberOfDispatchableUnits(palier, pdt);
    const double unitsOn = std::ceil(x(nodu));
    emit("actual_num_units_on", cluster, pdt, unitsOn);

    const double startupCost = cost(variableManager_.NumberStartingDispatchableUnits(palier, pdt));
    double startedUnits = 0.;
    if (pdt > 0)
    {
        const double previousUnitsOn = std::ceil(
          x(variableManager_.NumberOfDispatchableUnits(palier, pdt - 1)));
        startedUnits = std::max(0., unitsOn - previousUnitsOn);
    }
    emit("non_prop_cost", cluster, pdt, startupCost * startedUnits + cost(nodu) * unitsOn);
}

void LegacyExtraOutputEmitter::weeklyHydroOutputs(uint32_t pays) const
{
    if (!problemeHebdo_.CaracteristiquesHydrauliques[pays].AccurateWaterValue)
    {
        return;
    }
    const int pdt = problemeHebdo_.NombreDePasDeTempsPourUneOptimisation - 1;
    emit("hydro_shadow_price",
         hydroStorageNames_[pays],
         pdt,
         dual(problemeHebdo_.NumeroDeContrainteExpressionStockFinal[pays]));
}

} // namespace

void AddLegacyExtraOutputs(SimulationTable& simulationTable,
                           PROBLEME_HEBDO& problemeHebdo,
                           const FillContext& fillContext,
                           unsigned currentBlock)
{
    LegacyExtraOutputEmitter emitter(simulationTable, problemeHebdo, fillContext, currentBlock);

    for (int pdt = 0; pdt < problemeHebdo.NombreDePasDeTempsPourUneOptimisation; ++pdt)
    {
        for (uint32_t pays = 0; pays < problemeHebdo.NombreDePays; ++pays)
        {
            emitter.areaOutputs(pays, pdt);

            const PALIERS_THERMIQUES& paliers = problemeHebdo.PaliersThermiquesDuPays[pays];
            for (int index = 0; index < paliers.NombreDePaliersThermiques; ++index)
            {
                emitter.thermalOutputs(pays, index, pdt);
            }
        }

        for (uint32_t interco = 0; interco < problemeHebdo.NombreDInterconnexions; ++interco)
        {
            emitter.linkOutputs(interco, pdt);
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo.NombreDePays; ++pays)
    {
        emitter.weeklyHydroOutputs(pays);
    }
}

} // namespace Antares::Optimization
