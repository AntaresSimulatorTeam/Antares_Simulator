// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacyExtraOutputs.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <string_view>
#include <utility>

#include "antares/solver/optimisation/LegacyExtraOutputsContext.h"
#include "antares/solver/optimisation/LegacySolutionView.h"

using Antares::IO::Outputs::SimulationTable;
using Antares::Optimisation::LinearProblemApi::FillContext;

namespace Antares::Optimization
{

namespace
{
std::optional<unsigned> BlockTimeIndex(const FillContext& fillContext, unsigned timeIndex)
{
    if (timeIndex >= fillContext.getGlobalFirstTimeStep()
        && timeIndex <= fillContext.getGlobalLastTimeStep())
    {
        return timeIndex - fillContext.getGlobalFirstTimeStep();
    }
    return std::nullopt;
}

// Reads a per-hour study series carried in the context (load, inflows, loop
// flow): finds the component's vector, then indexes it by hour-in-week. Returns
// nullopt when the component is unknown to the context or the hour is out of
// range, so the caller can skip the row.
std::optional<double> ContextValueAtHour(
  const std::unordered_map<std::string, std::vector<double>>& byKey,
  const std::string& key,
  unsigned timeIndex,
  unsigned weekFirstTimeStep)
{
    const auto it = byKey.find(key);
    if (it == byKey.end())
    {
        return std::nullopt;
    }
    const unsigned pdt = timeIndex - weekFirstTimeStep;
    if (pdt >= it->second.size())
    {
        return std::nullopt;
    }
    return it->second[pdt];
}

// area -> price (= -dual(AreaBalance)), built up front from the recorded
// AreaBalance constraints so the variable-anchored consumers (thermal profit,
// link congestion fees, ...) can look up their area's price before the variable
// dispatch loop. Zero on MIP weeks, where the duals are not extracted (same
// limitation as the price output).
std::unordered_map<std::string, double> BuildPriceByArea(
  const std::vector<std::optional<LegacyVariableInfo>>& constraintsInfo,
  const std::vector<double>& constraintDuals)
{
    std::unordered_map<std::string, double> priceByArea;
    for (std::size_t index = 0; index < constraintsInfo.size(); ++index)
    {
        const auto& info = constraintsInfo[index];
        if (info && info->name == "AreaBalance")
        {
            priceByArea.emplace(info->component, -constraintDuals[index]);
        }
    }
    return priceByArea;
}

// Splits a link component "origin$$destination" (see AREA_SEP in
// opt_rename_problem.cpp) into its origin and extremity area names. Returns
// nullopt when the separator is absent.
std::optional<std::pair<std::string, std::string>> SplitLinkEndpoints(const std::string& component)
{
    constexpr std::string_view sep = "$$";
    const auto pos = component.find(sep);
    if (pos == std::string::npos)
    {
        return std::nullopt;
    }
    return std::make_pair(component.substr(0, pos), component.substr(pos + sep.size()));
}

// Emission extra-output IDs, ordered to match Antares::Data::Pollutant::PollutantEnum
// so each pollutant's factor (read by ordinal from the context) maps to its row.
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

// Emits the legacy extra outputs for one weekly solve. The constructor captures
// everything that is invariant across the dispatch loop (the table, the solution
// view, the study-data context, the derived area-price map, the raw value / cost
// / dual arrays, the fill context and the block); each emitter method then takes
// only the two things that vary per row: the anchor's LegacyVariableInfo and its
// index. Variable-anchored methods index the value / cost arrays; constraint-
// anchored methods index the dual array.
class LegacyExtraOutputEmitter
{
public:
    LegacyExtraOutputEmitter(SimulationTable& simulationTable,
                             const LegacySolutionView& solution,
                             const LegacyExtraOutputsContext& context,
                             const std::unordered_map<std::string, double>& priceByArea,
                             const std::vector<double>& solutionValues,
                             const std::vector<double>& linearCosts,
                             const std::vector<double>& constraintDuals,
                             const FillContext& fillContext,
                             unsigned currentBlock):
        table_(simulationTable),
        solution_(solution),
        context_(context),
        priceByArea_(priceByArea),
        values_(solutionValues),
        costs_(linearCosts),
        duals_(constraintDuals),
        fillContext_(fillContext),
        block_(currentBlock)
    {
    }

    // One method per anchor: the recorded variable / constraint name that
    // AddLegacyExtraOutputs dispatches on. Each emits every output derived from
    // that anchor. Variable anchors index the value / cost arrays; constraint
    // anchors index the dual array.
    void dispatchableProduction(const LegacyVariableInfo& info, std::size_t index);
    void unsuppliedEnergy(const LegacyVariableInfo& info, std::size_t index);
    void nodu(const LegacyVariableInfo& info, std::size_t index);
    void directFlow(const LegacyVariableInfo& info, std::size_t index);
    void positiveDirectFlow(const LegacyVariableInfo& info, std::size_t index);
    void hydroLevel(const LegacyVariableInfo& info, std::size_t index);
    void areaBalance(const LegacyVariableInfo& info, std::size_t index);
    void flowDissociation(const LegacyVariableInfo& info, std::size_t index);
    void finalStockExpression(const LegacyVariableInfo& info, std::size_t index);

private:
    // Per-output emitters, grouped by the anchor that drives them.
    void thermalPropCost(const LegacyVariableInfo& info, std::size_t index);
    void thermalEmissions(const LegacyVariableInfo& info, std::size_t index);
    void thermalMargins(const LegacyVariableInfo& info, std::size_t index);
    void thermalProfit(const LegacyVariableInfo& info, std::size_t index);
    void areaImbalanceCost(const LegacyVariableInfo& info, std::size_t index);
    void areaIsLossOfLoad(const LegacyVariableInfo& info, std::size_t index);
    void areaActualLoad(const LegacyVariableInfo& info, std::size_t index);
    void thermalActualNumUnitsOn(const LegacyVariableInfo& info, std::size_t index);
    void thermalNonPropCost(const LegacyVariableInfo& info, std::size_t index);
    void linkAbsFlow(const LegacyVariableInfo& info, std::size_t index);
    void linkMinusFlow(const LegacyVariableInfo& info, std::size_t index);
    void linkActualLoopFlow(const LegacyVariableInfo& info, std::size_t index);
    void linkIsDirectlyCongested(const LegacyVariableInfo& info, std::size_t index);
    void linkIsIndirectlyCongested(const LegacyVariableInfo& info, std::size_t index);
    void linkCongestionFees(const LegacyVariableInfo& info, std::size_t index);
    void linkPropCost(const LegacyVariableInfo& info, std::size_t index);
    void areaLevelPercentage(const LegacyVariableInfo& info, std::size_t index);
    void hydroActualInflows(const LegacyVariableInfo& info, std::size_t index);
    void areaPrice(const LegacyVariableInfo& info, std::size_t index);
    void areaIsNearLossOfLoad(const LegacyVariableInfo& info, std::size_t index);
    void linkCapacityShadowPrice(const LegacyVariableInfo& info, std::size_t index);
    void hydroShadowPrice(const LegacyVariableInfo& info, std::size_t index);

    // Pushes one row into the simulation table for the given anchor and value.
    void emit(const std::string& output, const LegacyVariableInfo& info, double value);

    // Context key for per-cluster study data: the thermal anchor's component is
    // the cluster name alone, which is not unique across areas, so the area
    // qualifier is folded in (see AREA_SEP in opt_rename_problem.cpp).
    std::string clusterKey(const LegacyVariableInfo& info) const
    {
        return info.area + "$$" + info.component;
    }

    // Hour-in-week of the anchor, used to index the context's per-hour vectors.
    unsigned hourInWeek(const LegacyVariableInfo& info) const
    {
        return info.timeIndex - context_.weekFirstTimeStep;
    }

    SimulationTable& table_;
    const LegacySolutionView& solution_;
    const LegacyExtraOutputsContext& context_;
    const std::unordered_map<std::string, double>& priceByArea_;
    const std::vector<double>& values_;
    const std::vector<double>& costs_;
    const std::vector<double>& duals_;
    const FillContext& fillContext_;
    unsigned block_;
};

void LegacyExtraOutputEmitter::emit(const std::string& output,
                                    const LegacyVariableInfo& info,
                                    double value)
{
    table_.addEntry({.block = block_ + 1,
                     .component = info.component,
                     .output = output,
                     .absolute_time_index = info.timeIndex + 1,
                     .block_time_index = BlockTimeIndex(fillContext_, info.timeIndex),
                     .scenario_index = fillContext_.getYear(),
                     .value = value,
                     .status = std::nullopt});
}

// prop_cost = generation_cost * generation_power: both factors belong to the
// generation variable itself, so they are read by index, which also keeps
// identically-named clusters of different areas apart.
void LegacyExtraOutputEmitter::thermalPropCost(const LegacyVariableInfo& info, std::size_t index)
{
    emit("prop_cost", info, costs_[index] * values_[index]);
}

// <pollutant>_emissions = generation_power * emission_rate, one row per pollutant.
// generation_power is the DispatchableProduction value (by index, area-safe); the
// rates come from the context keyed by "area$$cluster". Skipped for clusters
// absent from the context.
void LegacyExtraOutputEmitter::thermalEmissions(const LegacyVariableInfo& info, std::size_t index)
{
    const auto it = context_.emissionFactorsByCluster.find(clusterKey(info));
    if (it == context_.emissionFactorsByCluster.end())
    {
        return;
    }
    const double generation = values_[index];
    for (std::size_t pollutant = 0; pollutant < emissionOutputNames.size(); ++pollutant)
    {
        emit(emissionOutputNames[pollutant], info, generation * it->second[pollutant]);
    }
}

// Thermal margin outputs, all anchored on the cluster's DispatchableProduction
// variable and computed from the per-cluster data carried in the context
// (keyed "area$$cluster"):
//   cluster_availability = max(availability, minStablePower * ceil(availability /
//                              unitSize))
//   up_margin            = cluster_availability - generation_power
//   min_gen_power        = min(generation_power, minGenPower)
//   down_margin          = generation_power - min(cluster_availability, minGenPower)
// availability, unitSize and minStablePower are the spinning-adjusted weekly-
// problem quantities; the spec's raw cluster_max_generation / max_power_per_unit
// differ by the same (1 - spinning/100) factor, which cancels in the formulas
// above. Skipped when the cluster is absent from the context or its per-hour
// vectors do not cover the anchor's hour.
void LegacyExtraOutputEmitter::thermalMargins(const LegacyVariableInfo& info, std::size_t index)
{
    const auto it = context_.thermalMarginByCluster.find(clusterKey(info));
    if (it == context_.thermalMarginByCluster.end())
    {
        return;
    }
    const auto& data = it->second;
    const unsigned pdt = hourInWeek(info);
    if (pdt >= data.availability.size() || pdt >= data.minGenPower.size())
    {
        return;
    }

    const double generation = values_[index];
    const double available = data.availability[pdt];
    const double minGen = data.minGenPower[pdt];

    double unitFloor = 0.;
    if (data.unitSize > 0.)
    {
        unitFloor = data.minStablePower * std::ceil(available / data.unitSize);
    }
    const double clusterAvailability = std::max(available, unitFloor);

    emit("cluster_availability", info, clusterAvailability);
    emit("up_margin", info, clusterAvailability - generation);
    emit("min_gen_power", info, std::min(generation, minGen));
    emit("down_margin", info, generation - std::min(clusterAvailability, minGen));
}

// profit = (area_price - generation_cost)
//          * max(generation_power - min_gen_power, 0)
// area_price is the cluster area's balance dual, looked up in the area-price map;
// generation_cost and generation_power belong to the DispatchableProduction
// variable, read by index (area-safe); the min_gen_power floor is the same
// quantity the context carries for the margin outputs. Skipped when the area has
// no recorded price, the cluster is absent from the context, or the per-hour
// floor does not cover the anchor's hour.
void LegacyExtraOutputEmitter::thermalProfit(const LegacyVariableInfo& info, std::size_t index)
{
    const auto priceIt = priceByArea_.find(info.area);
    if (priceIt == priceByArea_.end())
    {
        return;
    }
    const auto marginIt = context_.thermalMarginByCluster.find(clusterKey(info));
    if (marginIt == context_.thermalMarginByCluster.end())
    {
        return;
    }
    const unsigned pdt = hourInWeek(info);
    if (pdt >= marginIt->second.minGenPower.size())
    {
        return;
    }

    const double generation = values_[index];
    const double floor = marginIt->second.minGenPower[pdt];
    const double profit = (priceIt->second - costs_[index]) * std::max(generation - floor, 0.);
    emit("profit", info, profit);
}

// imbalance_cost = spillage_cost * spilled_energy
//                  + unsupplied_energy_cost * unsupplied_energy
// Driven by the area's UnsuppliedEnergy variable; the Spillage variable of the
// same area and timestep is found through the solution view.
void LegacyExtraOutputEmitter::areaImbalanceCost(const LegacyVariableInfo& info, std::size_t index)
{
    const auto spilled = solution_.value("Spillage", info.component, info.timeIndex);
    const auto spillageCost = solution_.linearCost("Spillage", info.component, info.timeIndex);
    if (!spilled || !spillageCost)
    {
        return;
    }

    const double value = spillageCost.value() * spilled.value() + costs_[index] * values_[index];
    emit("imbalance_cost", info, value);
}

// is_loss_of_load = 1 when the area has unsupplied energy (above the 0.5 MW
// solver-noise threshold), 0 otherwise.
void LegacyExtraOutputEmitter::areaIsLossOfLoad(const LegacyVariableInfo& info, std::size_t index)
{
    constexpr double lossOfLoadthreshold = 0.5;
    emit("is_loss_of_load", info, values_[index] > lossOfLoadthreshold ? 1. : 0.);
}

// actual_load = load: the area's input residual-load series, carried per-hour in
// the context. Skipped when the area is unknown to the context.
void LegacyExtraOutputEmitter::areaActualLoad(const LegacyVariableInfo& info, std::size_t)
{
    const auto load = ContextValueAtHour(context_.loadByArea,
                                         info.component,
                                         info.timeIndex,
                                         context_.weekFirstTimeStep);
    if (!load)
    {
        return;
    }
    emit("actual_load", info, load.value());
}

// actual_num_units_on = ceil(num_units_on): the NODU variable may be
// fractional when the unit-commitment problem is relaxed.
void LegacyExtraOutputEmitter::thermalActualNumUnitsOn(const LegacyVariableInfo& info,
                                                       std::size_t index)
{
    emit("actual_num_units_on", info, std::ceil(values_[index]));
}

// non_prop_cost = startup_cost * started_units + fixed_cost * units_on, with
// units rounded up (the NODU variable may be fractional when relaxed).
// fixed_cost is the objective coefficient on NODU itself; startup_cost is the
// coefficient on the same cluster's NumberStartingDispatchableUnits variable,
// read through the solution view (0 when that variable is not recorded). The
// started-units count compares ceil(NODU) against the previous hour; at the
// first hour of the week the previous NODU is not part of this weekly solution,
// so the start-up term is dropped.
void LegacyExtraOutputEmitter::thermalNonPropCost(const LegacyVariableInfo& info, std::size_t index)
{
    const double unitsOn = std::ceil(values_[index]);
    const double fixedCost = costs_[index];

    double startupCost = 0.;
    if (const auto cost = solution_.linearCost("NumberStartingDispatchableUnits",
                                               info.component,
                                               info.timeIndex))
    {
        startupCost = cost.value();
    }

    double startedUnits = 0.;
    if (info.timeIndex > 0)
    {
        if (const auto previous = solution_.value("NODU", info.component, info.timeIndex - 1))
        {
            startedUnits = std::max(0., unitsOn - std::ceil(previous.value()));
        }
    }

    emit("non_prop_cost", info, startupCost * startedUnits + fixedCost * unitsOn);
}

// abs_flow = |flow|: the DirectFlow variable is signed (negative when the
// link is used from destination to origin).
void LegacyExtraOutputEmitter::linkAbsFlow(const LegacyVariableInfo& info, std::size_t index)
{
    emit("abs_flow", info, std::abs(values_[index]));
}

// minus_flow = -flow: the signed convention of the GEMS view, anchored on the
// link's DirectFlow variable.
void LegacyExtraOutputEmitter::linkMinusFlow(const LegacyVariableInfo& info, std::size_t index)
{
    emit("minus_flow", info, -values_[index]);
}

// actual_loop_flow = loop_flow: the link's input loop-flow parameter, carried
// per-hour in the context. Skipped when the link is unknown to the context.
void LegacyExtraOutputEmitter::linkActualLoopFlow(const LegacyVariableInfo& info, std::size_t)
{
    const auto loopFlow = ContextValueAtHour(context_.loopFlowByLink,
                                             info.component,
                                             info.timeIndex,
                                             context_.weekFirstTimeStep);
    if (!loopFlow)
    {
        return;
    }
    emit("actual_loop_flow", info, loopFlow.value());
}

// is_directly_congested = 1 when the link is at (or near) its
// origin->extremity transmission capacity in the direct direction; 0
// otherwise. The capacity is not an objective coefficient — it is the
// upper bound of the DirectFlow variable — and is carried per-pdt in
// LegacyExtraOutputsContext. Skipped when the link's capacity is not
// known to the context.
void LegacyExtraOutputEmitter::linkIsDirectlyCongested(const LegacyVariableInfo& info,
                                                       std::size_t index)
{
    constexpr double saturationEpsilon = 1e-5;
    const auto capacity = ContextValueAtHour(context_.directCapacityByLink,
                                             info.component,
                                             info.timeIndex,
                                             context_.weekFirstTimeStep);
    if (!capacity)
    {
        return;
    }
    const double saturated = values_[index] >= capacity.value() - saturationEpsilon ? 1. : 0.;
    emit("is_directly_congested", info, saturated);
}

// is_indirectly_congested = 1 when the link is at (or near) its
// extremity->origin transmission capacity. DirectFlow is signed: negative
// values mean the link is used in the indirect direction, so the indicator
// compares -X against the indirect capacity.
void LegacyExtraOutputEmitter::linkIsIndirectlyCongested(const LegacyVariableInfo& info,
                                                         std::size_t index)
{
    constexpr double saturationEpsilon = 1e-5;
    const auto capacity = ContextValueAtHour(context_.indirectCapacityByLink,
                                             info.component,
                                             info.timeIndex,
                                             context_.weekFirstTimeStep);
    if (!capacity)
    {
        return;
    }
    const double saturated = -values_[index] >= capacity.value() - saturationEpsilon ? 1. : 0.;
    emit("is_indirectly_congested", info, saturated);
}

// abs_congestion_fee = |flow| * |price_out - price_in|
// alg_congestion_fee = flow  * (price_out - price_in)
// price_in / price_out are the balance duals (area-price map) of the link's
// origin / extremity areas, parsed from the "origin$$destination" component.
// Anchored on the link's DirectFlow variable; skipped when either endpoint area
// has no recorded price.
void LegacyExtraOutputEmitter::linkCongestionFees(const LegacyVariableInfo& info, std::size_t index)
{
    const auto endpoints = SplitLinkEndpoints(info.component);
    if (!endpoints)
    {
        return;
    }
    const auto priceIn = priceByArea_.find(endpoints->first);
    const auto priceOut = priceByArea_.find(endpoints->second);
    if (priceIn == priceByArea_.end() || priceOut == priceByArea_.end())
    {
        return;
    }

    const double flow = values_[index];
    const double delta = priceOut->second - priceIn->second;
    emit("abs_congestion_fee", info, std::abs(flow) * std::abs(delta));
    emit("alg_congestion_fee", info, flow * delta);
}

// prop_cost (link) = direct_hurdle_cost * positive_direct_flow
//                    + indirect_hurdle_cost * positive_indirect_flow
// The hurdle costs are the linear objective coefficients on the flow
// decomposition variables (opt_gestion_des_couts_cas_lineaire.cpp), which
// only exist for links managed with hurdle costs. Driven by the link's
// PositiveDirectFlow variable; the PositiveIndirectFlow variable of the same
// link and timestep is found through the solution view.
void LegacyExtraOutputEmitter::linkPropCost(const LegacyVariableInfo& info, std::size_t index)
{
    const auto indirect = solution_.value("PositiveIndirectFlow", info.component, info.timeIndex);
    const auto indirectCost = solution_.linearCost("PositiveIndirectFlow",
                                                   info.component,
                                                   info.timeIndex);
    if (!indirect || !indirectCost)
    {
        return;
    }

    const double value = costs_[index] * values_[index] + indirectCost.value() * indirect.value();
    emit("prop_cost", info, value);
}

// level_percentage = HydroLevel / reservoir_capacity * 100; the reservoir
// capacity is not an objective coefficient on any recorded variable, so it is
// carried in LegacyExtraOutputsContext. Skipped when the area has no reservoir
// (capacity unknown or non-positive).
void LegacyExtraOutputEmitter::areaLevelPercentage(const LegacyVariableInfo& info,
                                                   std::size_t index)
{
    const auto it = context_.reservoirCapacityByArea.find(info.component);
    if (it == context_.reservoirCapacityByArea.end() || it->second <= 0.)
    {
        return;
    }
    emit("level_percentage", info, values_[index] / it->second * 100.);
}

// actual_inflows = round(inflows): the area's input natural-inflow series,
// carried per-hour in the context (only areas with a reservoir have one).
// Skipped when the area is unknown to the context.
void LegacyExtraOutputEmitter::hydroActualInflows(const LegacyVariableInfo& info, std::size_t)
{
    const auto inflows = ContextValueAtHour(context_.inflowsByArea,
                                            info.component,
                                            info.timeIndex,
                                            context_.weekFirstTimeStep);
    if (!inflows)
    {
        return;
    }
    emit("actual_inflows", info, std::round(inflows.value()));
}

// price = -dual(area balance constraint): the stored dual is the negative of
// the marginal price (the legacy outputs print -CoutsMarginauxHoraires).
void LegacyExtraOutputEmitter::areaPrice(const LegacyVariableInfo& info, std::size_t index)
{
    emit("price", info, -duals_[index]);
}

// is_near_loss_of_load = 1 when the area price approaches the unsupplied
// energy cost (within 5), 0 otherwise. The unsupplied energy cost is the
// linear objective coefficient on the area's UnsuppliedEnergy variable, found
// through the solution view; skipped if that variable is not recorded.
void LegacyExtraOutputEmitter::areaIsNearLossOfLoad(const LegacyVariableInfo& info,
                                                    std::size_t index)
{
    const auto unsuppliedCost = solution_.linearCost("UnsuppliedEnergy",
                                                     info.component,
                                                     info.timeIndex);
    if (!unsuppliedCost)
    {
        return;
    }

    const double price = -duals_[index];
    constexpr unsigned cutoff = 5;
    emit("is_near_loss_of_load", info, price > unsuppliedCost.value() - cutoff ? 1. : 0.);
}

// capacity_shadow_price = |dual(flow dissociation constraint)|. The
// constraint only exists (and is only recorded) for links managed with
// hurdle costs.
void LegacyExtraOutputEmitter::linkCapacityShadowPrice(const LegacyVariableInfo& info,
                                                       std::size_t index)
{
    emit("capacity_shadow_price", info, std::abs(duals_[index]));
}

// hydro_shadow_price = dual(final stock expression constraint); one per area
// and week, recorded only when accurate water value mode is on.
void LegacyExtraOutputEmitter::hydroShadowPrice(const LegacyVariableInfo& info, std::size_t index)
{
    emit("hydro_shadow_price", info, duals_[index]);
}

// Per-anchor entry points: each gathers the outputs derived from one recorded
// variable / constraint.
void LegacyExtraOutputEmitter::dispatchableProduction(const LegacyVariableInfo& info,
                                                      std::size_t index)
{
    thermalPropCost(info, index);
    thermalEmissions(info, index);
    thermalMargins(info, index);
    thermalProfit(info, index);
}

void LegacyExtraOutputEmitter::unsuppliedEnergy(const LegacyVariableInfo& info, std::size_t index)
{
    areaImbalanceCost(info, index);
    areaIsLossOfLoad(info, index);
    areaActualLoad(info, index);
}

void LegacyExtraOutputEmitter::nodu(const LegacyVariableInfo& info, std::size_t index)
{
    thermalActualNumUnitsOn(info, index);
    thermalNonPropCost(info, index);
}

void LegacyExtraOutputEmitter::directFlow(const LegacyVariableInfo& info, std::size_t index)
{
    linkAbsFlow(info, index);
    linkMinusFlow(info, index);
    linkActualLoopFlow(info, index);
    linkIsDirectlyCongested(info, index);
    linkIsIndirectlyCongested(info, index);
    linkCongestionFees(info, index);
}

void LegacyExtraOutputEmitter::positiveDirectFlow(const LegacyVariableInfo& info, std::size_t index)
{
    linkPropCost(info, index);
}

void LegacyExtraOutputEmitter::hydroLevel(const LegacyVariableInfo& info, std::size_t index)
{
    areaLevelPercentage(info, index);
    hydroActualInflows(info, index);
}

void LegacyExtraOutputEmitter::areaBalance(const LegacyVariableInfo& info, std::size_t index)
{
    areaPrice(info, index);
    areaIsNearLossOfLoad(info, index);
}

void LegacyExtraOutputEmitter::flowDissociation(const LegacyVariableInfo& info, std::size_t index)
{
    linkCapacityShadowPrice(info, index);
}

void LegacyExtraOutputEmitter::finalStockExpression(const LegacyVariableInfo& info,
                                                    std::size_t index)
{
    hydroShadowPrice(info, index);
}

} // namespace

void AddLegacyExtraOutputs(SimulationTable& simulationTable,
                           const std::vector<std::optional<LegacyVariableInfo>>& variablesInfo,
                           const std::vector<double>& solutionValues,
                           const std::vector<double>& linearCosts,
                           const std::vector<std::optional<LegacyVariableInfo>>& constraintsInfo,
                           const std::vector<double>& constraintDuals,
                           const LegacyExtraOutputsContext& context,
                           const FillContext& fillContext,
                           unsigned currentBlock)
{
    const LegacySolutionView solution(variablesInfo, solutionValues, linearCosts);

    // The area price is a constraint dual; the consumers below (thermal profit,
    // link congestion fees) are variable-anchored, so the lookup is built up
    // front, before the variable dispatch loop.
    const auto priceByArea = BuildPriceByArea(constraintsInfo, constraintDuals);

    LegacyExtraOutputEmitter emitter(simulationTable,
                                     solution,
                                     context,
                                     priceByArea,
                                     solutionValues,
                                     linearCosts,
                                     constraintDuals,
                                     fillContext,
                                     currentBlock);

    for (std::size_t index = 0; index < variablesInfo.size(); ++index)
    {
        const auto& info = variablesInfo[index];
        if (!info)
        {
            continue;
        }

        if (info->name == "DispatchableProduction")
        {
            emitter.dispatchableProduction(*info, index);
        }
        else if (info->name == "UnsuppliedEnergy")
        {
            emitter.unsuppliedEnergy(*info, index);
        }
        else if (info->name == "NODU")
        {
            emitter.nodu(*info, index);
        }
        else if (info->name == "DirectFlow")
        {
            emitter.directFlow(*info, index);
        }
        else if (info->name == "PositiveDirectFlow")
        {
            emitter.positiveDirectFlow(*info, index);
        }
        else if (info->name == "HydroLevel")
        {
            emitter.hydroLevel(*info, index);
        }
    }

    for (std::size_t index = 0; index < constraintsInfo.size(); ++index)
    {
        const auto& info = constraintsInfo[index];
        if (!info)
        {
            continue;
        }

        if (info->name == "AreaBalance")
        {
            emitter.areaBalance(*info, index);
        }
        else if (info->name == "FlowDissociation")
        {
            emitter.flowDissociation(*info, index);
        }
        else if (info->name == "FinalStockExpression")
        {
            emitter.finalStockExpression(*info, index);
        }
    }
}

} // namespace Antares::Optimization
