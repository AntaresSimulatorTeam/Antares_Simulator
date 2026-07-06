// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacyExtraOutputs.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <string>

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

// Emits the legacy extra outputs for one weekly solve, driven by the study
// structure: the entity methods below are called for every area / link /
// thermal cluster and hour of the optimization interval, fetch the solution
// values, objective coefficients and duals by index through the
// correspondence tables, and read study data straight from PROBLEME_HEBDO.
// Guards mirror the construction sites: variable indices use the -1 sentinel
// where construction writes one (HydroLevel); variables that exist only in a
// given mode are guarded by that mode's condition (hurdle-cost links, unit
// commitment, accurate water value), because their correspondence slots are
// not sentinel-initialized.
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
    }

    void areaOutputs(uint32_t pays, int pdt);
    void linkOutputs(uint32_t interco, int pdt);
    void thermalOutputs(uint32_t pays, int index, int pdt);
    // hydro_shadow_price is anchored on the weekly FinalStockExpression
    // constraint: one row per area, not one per hour.
    void weeklyHydroOutputs(uint32_t pays);

private:
    void emit(const std::string& output, const std::string& component, int pdt, double value);

    double x(int variableIndex) const
    {
        return problem_.X[static_cast<std::size_t>(variableIndex)];
    }

    double cost(int variableIndex) const
    {
        return problem_.CoutLineaire[static_cast<std::size_t>(variableIndex)];
    }

    double dual(int constraintIndex) const
    {
        return problem_.CoutsMarginauxDesContraintes[static_cast<std::size_t>(constraintIndex)];
    }

    // price = -dual(area balance constraint): the stored dual is the negative
    // of the marginal price (the legacy outputs print -CoutsMarginauxHoraires).
    double areaPrice(uint32_t pays, int pdt) const
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
};

void LegacyExtraOutputEmitter::emit(const std::string& output,
                                    const std::string& component,
                                    int pdt,
                                    double value)
{
    // Absolute hour of the year, matching the time index recorded on the raw
    // rows (opt_construction_variables_optimisees_lineaire.cpp).
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
    const std::string area = problemeHebdo_.NomsDesPays[pays];

    const int unsupplied = variableManager_.UnsuppliedEnergy(pays, pdt);
    const int spillage = variableManager_.Spillage(pays, pdt);

    // imbalance_cost = spillage_cost * spilled_energy
    //                  + unsupplied_energy_cost * unsupplied_energy
    // The costs are the objective coefficients on the two slack variables.
    emit("imbalance_cost", area, pdt, cost(spillage) * x(spillage) + cost(unsupplied) * x(unsupplied));

    // is_loss_of_load = 1 when the area has unsupplied energy (above the
    // 0.5 MW solver-noise threshold), 0 otherwise.
    constexpr double lossOfLoadThreshold = 0.5;
    emit("is_loss_of_load", area, pdt, x(unsupplied) > lossOfLoadThreshold ? 1. : 0.);

    // actual_load = raw load: ConsommationAbattueDuPays is the residual load
    // (load minus must-run generation, see sim_calcul_economique.cpp), so the
    // must-run part is added back.
    emit("actual_load",
         area,
         pdt,
         problemeHebdo_.ConsommationsAbattues[pdt].ConsommationAbattueDuPays[pays]
           + problemeHebdo_.AllMustRunGeneration[pdt].AllMustRunGenerationOfArea[pays]);

    const double price = areaPrice(pays, pdt);
    emit("price", area, pdt, price);

    // is_near_loss_of_load = 1 when the area price approaches the unsupplied
    // energy cost (within 5), 0 otherwise.
    constexpr double nearLossOfLoadCutoff = 5.;
    emit("is_near_loss_of_load", area, pdt, price > cost(unsupplied) - nearLossOfLoadCutoff ? 1. : 0.);

    // Hydro outputs, only for areas whose reservoir is managed (the HydroLevel
    // variable is -1 otherwise, see opt_construction_variables_optimisees_lineaire.cpp).
    const int hydroLevel = variableManager_.HydroLevel(pays, pdt);
    if (hydroLevel < 0)
    {
        return;
    }
    const auto& hydro = problemeHebdo_.CaracteristiquesHydrauliques[pays];
    if (hydro.TailleReservoir > 0.)
    {
        emit("level_percentage", area, pdt, x(hydroLevel) / hydro.TailleReservoir * 100.);
    }
    if (static_cast<std::size_t>(pdt) < hydro.ApportNaturelHoraire.size())
    {
        emit("actual_inflows", area, pdt, std::round(hydro.ApportNaturelHoraire[pdt]));
    }
}

void LegacyExtraOutputEmitter::linkOutputs(uint32_t interco, int pdt)
{
    const uint32_t origin = problemeHebdo_.PaysOrigineDeLInterconnexion[interco];
    const uint32_t destination = problemeHebdo_.PaysExtremiteDeLInterconnexion[interco];
    const std::string link = std::string(problemeHebdo_.NomsDesPays[origin]) + "$$"
                             + problemeHebdo_.NomsDesPays[destination];

    // The DirectFlow variable is signed: negative when the link is used from
    // destination to origin.
    const double flow = x(variableManager_.DirectFlow(interco, pdt));
    emit("abs_flow", link, pdt, std::abs(flow));
    // minus_flow = -flow: the signed convention of the GEMS view.
    emit("minus_flow", link, pdt, -flow);

    const auto& ntc = problemeHebdo_.ValeursDeNTC[pdt];
    emit("actual_loop_flow", link, pdt, ntc.ValeurDeLoopFlowOrigineVersExtremite[interco]);

    // is_directly/indirectly_congested = 1 when the link is at (or near) its
    // transmission capacity in that direction. The capacities are the bounds
    // of the DirectFlow variable, not objective coefficients.
    constexpr double saturationEpsilon = 1e-5;
    emit("is_directly_congested",
         link,
         pdt,
         flow >= ntc.ValeurDeNTCOrigineVersExtremite[interco] - saturationEpsilon ? 1. : 0.);
    emit("is_indirectly_congested",
         link,
         pdt,
         -flow >= ntc.ValeurDeNTCExtremiteVersOrigine[interco] - saturationEpsilon ? 1. : 0.);

    // abs_congestion_fee = |flow| * |price_out - price_in|
    // alg_congestion_fee = flow  * (price_out - price_in)
    // price_in / price_out are the balance duals of the link's endpoints.
    const double priceDelta = areaPrice(destination, pdt) - areaPrice(origin, pdt);
    emit("abs_congestion_fee", link, pdt, std::abs(flow) * std::abs(priceDelta));
    emit("alg_congestion_fee", link, pdt, flow * priceDelta);

    // The flow decomposition variables and the FlowDissociation constraint
    // only exist for links managed with hurdle costs (same guard as their
    // construction sites).
    if (!problemeHebdo_.CoutDeTransport[interco].IntercoGereeAvecDesCouts)
    {
        return;
    }

    // prop_cost (link) = direct_hurdle_cost * positive_direct_flow
    //                    + indirect_hurdle_cost * positive_indirect_flow
    // The hurdle costs are the objective coefficients on the decomposition
    // variables (opt_gestion_des_couts_cas_lineaire.cpp).
    const int positiveDirect = variableManager_.PositiveDirectFlow(interco, pdt);
    const int positiveIndirect = variableManager_.PositiveIndirectFlow(interco, pdt);
    emit("prop_cost",
         link,
         pdt,
         cost(positiveDirect) * x(positiveDirect) + cost(positiveIndirect) * x(positiveIndirect));

    // capacity_shadow_price = |dual(flow dissociation constraint)|.
    const int dissociation = problemeHebdo_.CorrespondanceCntNativesCntOptim[pdt]
                               .NumeroDeContrainteDeDissociationDeFlux[interco];
    emit("capacity_shadow_price", link, pdt, std::abs(dual(dissociation)));
}

void LegacyExtraOutputEmitter::thermalOutputs(uint32_t pays, int index, int pdt)
{
    const PALIERS_THERMIQUES& paliers = problemeHebdo_.PaliersThermiquesDuPays[pays];
    const std::string& cluster = paliers.NomsDesPaliersThermiques[index];
    const int palier = paliers.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

    const int production = variableManager_.DispatchableProduction(palier, pdt);
    const double generation = x(production);

    // prop_cost = generation_cost * generation_power: both factors belong to
    // the generation variable itself.
    emit("prop_cost", cluster, pdt, cost(production) * generation);

    // <pollutant>_emissions = generation_power * emission_rate, one row per pollutant.
    for (std::size_t pollutant = 0; pollutant < emissionOutputNames.size(); ++pollutant)
    {
        emit(emissionOutputNames[pollutant],
             cluster,
             pdt,
             generation * paliers.emissionFactors[index][pollutant]);
    }

    // Margin outputs:
    //   cluster_availability = max(availability, minStablePower * ceil(availability /
    //                              unitSize))
    //   up_margin            = cluster_availability - generation_power
    //   min_gen_power        = min(generation_power, minGenPower)
    //   down_margin          = generation_power - min(cluster_availability, minGenPower)
    // availability, unitSize and minStablePower are the spinning-adjusted
    // weekly-problem quantities; the spec's raw cluster_max_generation /
    // max_power_per_unit differ by the same (1 - spinning/100) factor, which
    // cancels in the formulas above.
    const auto& disp = paliers.PuissanceDisponibleEtCout[index];
    const auto& availabilitySeries = disp.PuissanceDisponibleDuPalierThermique;
    const auto& minGenSeries = disp.PuissanceMinDuPalierThermique;
    if (static_cast<std::size_t>(pdt) < availabilitySeries.size()
        && static_cast<std::size_t>(pdt) < minGenSeries.size())
    {
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

        // profit = (area_price - generation_cost)
        //          * max(generation_power - min_gen_power, 0)
        const double profit = (areaPrice(pays, pdt) - cost(production))
                              * std::max(generation - minGen, 0.);
        emit("profit", cluster, pdt, profit);
    }

    // The unit-commitment variables only exist in "not fast" mode (their
    // construction in opt_construction_variables_couts_demarrages.cpp is
    // guarded by the same condition).
    if (!problemeHebdo_.OptimisationNotFastMode)
    {
        return;
    }

    // actual_num_units_on = ceil(num_units_on): the NODU variable may be
    // fractional when the unit-commitment problem is relaxed.
    const int nodu = variableManager_.NumberOfDispatchableUnits(palier, pdt);
    const double unitsOn = std::ceil(x(nodu));
    emit("actual_num_units_on", cluster, pdt, unitsOn);

    // non_prop_cost = startup_cost * started_units + fixed_cost * units_on.
    // fixed_cost is the objective coefficient on NODU itself; startup_cost the
    // one on NumberStartingDispatchableUnits. The started-units count compares
    // ceil(NODU) against the previous hour; at the first hour of the interval
    // the previous NODU is not part of this solution, so the start-up term is
    // dropped.
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

void LegacyExtraOutputEmitter::weeklyHydroOutputs(uint32_t pays)
{
    // hydro_shadow_price = dual(final stock expression constraint); the
    // constraint exists only in accurate water value mode and is anchored on
    // the last hour of the interval (see FinalStockExpression::add).
    if (!problemeHebdo_.CaracteristiquesHydrauliques[pays].AccurateWaterValue)
    {
        return;
    }
    const int pdt = problemeHebdo_.NombreDePasDeTempsPourUneOptimisation - 1;
    emit("hydro_shadow_price",
         problemeHebdo_.NomsDesPays[pays],
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
