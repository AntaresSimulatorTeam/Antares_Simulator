/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once
#include "antares/solver/variable/commons/hydro.h"
#include "antares/solver/variable/commons/load.h"
#include "antares/solver/variable/commons/miscGenMinusRowPSP.h"
#include "antares/solver/variable/commons/psp.h"
#include "antares/solver/variable/commons/rowBalance.h"
#include "antares/solver/variable/commons/solar.h"
#include "antares/solver/variable/commons/spatial-aggregate.h"
#include "antares/solver/variable/commons/wind.h"
#include "antares/solver/variable/economy/avail-dispatchable-generation.h"
#include "antares/solver/variable/economy/balance.h"
#include "antares/solver/variable/economy/dispatchable-generation-margin.h"
#include "antares/solver/variable/economy/hydrostorage.h"
#include "antares/solver/variable/economy/inflow.h"
#include "antares/solver/variable/economy/lold.h"
#include "antares/solver/variable/economy/lolp.h"
#include "antares/solver/variable/economy/max-mrg.h"
#include "antares/solver/variable/economy/overflow.h"
#include "antares/solver/variable/economy/pumping.h"
#include "antares/solver/variable/economy/reservoirlevel.h"
#include "antares/solver/variable/economy/unsupliedEnergy.h"
#include "antares/solver/variable/economy/waterValue.h"

#include "STStorageCashFlowByCluster.h"
#include "domesticUnsuppliedEnergy.h"
#include "dtgMarginAfterCsr.h"
#include "hydroCost.h"
#include "links.h"
#include "localMatchingRuleViolations.h"
#include "loldCsr.h"
#include "lolpCsr.h"
#include "max-mrg-csr.h"
#include "minDispatchableGenByPlant.h"
#include "nbOfDispatchedUnits.h"
#include "nbOfDispatchedUnitsByPlant.h"
#include "nearPriceCap.h"
#include "nonProportionalCost.h"
#include "npCostByDispatchablePlant.h"
#include "operatingCost.h"
#include "overallCost.h"
#include "price.h"
#include "productionByDispatchablePlant.h"
#include "productionByRenewablePlant.h"
#include "profitByPlant.h"
#include "spilledEnergy.h"
#include "thermalAirPollutantEmissions.h"
#include "unsupliedEnergyCsr.h"

// Variadic template to recursively apply wrappers ending with Tail
// Usage: ApplyChain<Tail, Wrapper1, Wrapper2, ...>::type yields Wrapper1<Wrapper2<...<Tail>...>>
template<typename Tail, template<typename> class... Wrappers>
struct ApplyChain;

template<typename Tail>
struct ApplyChain<Tail>
{
    using type = Tail;
};

template<typename Tail, template<typename> class Head, template<typename> class... Rest>
struct ApplyChain<Tail, Head, Rest...>
{
    using type = Head<typename ApplyChain<Tail, Rest...>::type>;
};

// List the wrapper templates in the desired order, ending with Links as the tail
#define ECONOMY_SINGLE_AREA_VARIABLES_CHAIN                                                       \
    OverallCost, OverallCostCsr, OperatingCost, Price, PriceCSR, ThermalAirPollutantEmissions,    \
      ProductionByDispatchablePlant, MinDispatchableGenByPlant, ProductionByRenewablePlant,       \
      Balance, RowBalance, PSP, MiscGenMinusRowPSP, TimeSeriesValuesLoad, TimeSeriesValuesHydro,  \
      TimeSeriesValuesWind, TimeSeriesValuesSolar, DispatchableGeneration, RenewableGeneration,   \
      HydroStorage, Pumping, ReservoirLevel, Inflows, Overflows, WaterValue, HydroCost,           \
      STSbyGroup, STstorageInjectionByCluster, STstorageWithdrawalByCluster,                      \
      STstorageLevelsByCluster, STstorageCashFlowByCluster, UnsupliedEnergy, UnsupliedEnergyCSR,  \
      DomesticUnsuppliedEnergy, LMRViolations, SpilledEnergy, LOLD, LOLD_CSR, LOLP, NearPriceCap, \
      LOLP_CSR, AvailableDispatchGen, DispatchableGenMargin, DtgMarginCsr, Marge, MaxMrgCsr,      \
      NonProportionalCost, NonProportionalCostByDispatchablePlant, NbOfDispatchedUnits,           \
      NbOfDispatchedUnitsByPlant, ProfitByPlant

#define ECONOMY_SET_OF_AREAS_VARIABLES                                                         \
    OverallCost, OperatingCost, Price, ThermalAirPollutantEmissions, Balance, RowBalance, PSP, \
      MiscGenMinusRowPSP, TimeSeriesValuesLoad, TimeSeriesValuesHydro, TimeSeriesValuesWind,   \
      TimeSeriesValuesSolar, HydroStorage, Pumping, ReservoirLevel, Inflows, Overflows,        \
      WaterValue, HydroCost, UnsupliedEnergy, DomesticUnsuppliedEnergy, LMRViolations,         \
      SpilledEnergy, LOLD, LOLP, NearPriceCap, AvailableDispatchGen, DispatchableGenMargin,    \
      DtgMarginCsr, Marge, NonProportionalCost, NbOfDispatchedUnits

// Adaptateurs pour utiliser Common::SpatialAggregate (qui attend un template<class> VarT et un
// NextT) avec ApplyChain (qui empile des wrappers unaires template<typename> class W).
namespace Antares::Solver::Variable::Economy
{
// Générateur générique : transforme un template variable VarT<Next> en wrapper unaire compatible
// ApplyChain en le re-mappant vers Common::SpatialAggregate<VarT, Next>.

template<template<class> class VarT>
struct ToSpatialAggregate
{
    template<typename Next>
    using Wrapper = Antares::Solver::Variable::Common::SpatialAggregate<VarT, Next>;
};

#undef SA_WRAP
#define SA_WRAP(VAR) Antares::Solver::Variable::Economy::ToSpatialAggregate<VAR>::template Wrapper

// Base (innermost) : SpatialAggregate<NbOfDispatchedUnits>
using SetOfAreasTail = Antares::Solver::Variable::Common::SpatialAggregate<NbOfDispatchedUnits>;

// Chaîne demandée (ordre extérieur -> intérieur) : OverallCost -> OperatingCost -> Price ->
// ThermalAirPollutantEmissions -> Balance -> RowBalance -> PSP -> MiscGenMinusRowPSP ->
// TimeSeriesValuesLoad -> TimeSeriesValuesHydro -> TimeSeriesValuesWind -> TimeSeriesValuesSolar ->
// HydroStorage -> Pumping -> ReservoirLevel -> Inflows -> Overflows -> WaterValue -> HydroCost ->
// UnsupliedEnergy -> DomesticUnsuppliedEnergy -> LMRViolations -> SpilledEnergy -> LOLD -> LOLP ->
// NearPriceCap -> AvailableDispatchGen -> DispatchableGenMargin -> DtgMarginCsr -> Marge ->
// NonProportionalCost -> NbOfDispatchedUnits
using VariablesPerSetOfAreasChain = ApplyChain<
  SetOfAreasTail, // base : SpatialAggregate<NbOfDispatchedUnits>
  // Wrappers listés de l'extérieur vers l'intérieur
  SA_WRAP(OverallCost),
  SA_WRAP(OperatingCost),
  SA_WRAP(Price),
  SA_WRAP(ThermalAirPollutantEmissions),
  SA_WRAP(Balance),
  SA_WRAP(RowBalance),
  SA_WRAP(PSP),
  SA_WRAP(MiscGenMinusRowPSP),
  SA_WRAP(TimeSeriesValuesLoad),
  SA_WRAP(TimeSeriesValuesHydro),
  SA_WRAP(TimeSeriesValuesWind),
  SA_WRAP(TimeSeriesValuesSolar),
  SA_WRAP(HydroStorage),
  SA_WRAP(Pumping),
  SA_WRAP(ReservoirLevel),
  SA_WRAP(Inflows),
  SA_WRAP(Overflows),
  SA_WRAP(WaterValue),
  SA_WRAP(HydroCost),
  SA_WRAP(UnsupliedEnergy),
  SA_WRAP(DomesticUnsuppliedEnergy),
  SA_WRAP(LMRViolations),
  SA_WRAP(SpilledEnergy),
  SA_WRAP(LOLD),
  SA_WRAP(LOLP),
  SA_WRAP(NearPriceCap),
  SA_WRAP(AvailableDispatchGen),
  SA_WRAP(DispatchableGenMargin),
  SA_WRAP(DtgMarginCsr),
  SA_WRAP(Marge),
  SA_WRAP(NonProportionalCost)>::type;

} // namespace Antares::Solver::Variable::Economy
