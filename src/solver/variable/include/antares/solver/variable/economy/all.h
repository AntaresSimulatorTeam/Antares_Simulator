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
#ifndef __SOLVER_VARIABLE_ECONOMY_ALL_H__
#define __SOLVER_VARIABLE_ECONOMY_ALL_H__

#include "antares/solver/variable/commons/variable_aggregate.h"

#include "../bindConstraints.h"
#include "../commons/hydro.h"
#include "../commons/join.h"
#include "../commons/load.h"
#include "../commons/miscGenMinusRowPSP.h"
#include "../commons/spatial-aggregate.h"
#include "../setofareas.h"
#include "price.h"

// For General values
#include "max-mrg.h"
#include "nbOfDispatchedUnits.h"
#include "operatingCost.h"
#include "overallCost.h"
#include "overallCostCsr.h"
#include "priceCSR.h"
#include "thermalAirPollutantEmissions.h"

// By thermal plant
#include "nbOfDispatchedUnitsByPlant.h"
#include "productionByDispatchablePlant.h"
#include "profitByPlant.h"

// By RES plant
#include "productionByRenewablePlant.h"

// Output variables associated to links
#include "links.h"

// Output variables associated to binding constraints
#include "antares/solver/variable/adequacy/all.h"
#include "antares/solver/variable/area.h"
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
#include "bindingConstraints/bindingConstraintsMarginalCost.h"
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
#include "variable_list.h"

namespace Antares::Solver::Variable::Economy
{
/*
// Toutes les variables économiques pour une zone, factorisées via un template générique et une
liste centralisée using VariablesPerArea = typename
MakeSpatialAggregate<ECONOMY_VARIABLES>::type;

*/

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

template<template<typename> class... Wrappers>
struct ApplyChainSpatialAgregate;

template<template<typename> class Last>
struct ApplyChainSpatialAgregate<Last>
{
    using type = Antares::Solver::Variable::Common::SpatialAggregate<Last>;
};

template<template<typename> class Head, template<typename> class... Rest>
struct ApplyChainSpatialAgregate<Head, Rest...>
{
    using type = Antares::Solver::Variable::Common::
      SpatialAggregate<Head, typename ApplyChainSpatialAgregate<Rest...>::type>;
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

using VariablesPerArea = ApplyChain<Variable::Economy::Links,
                                    ECONOMY_SINGLE_AREA_VARIABLES_CHAIN>::type;
using VariablesPerSetOfAreas = ApplyChainSpatialAgregate<ECONOMY_SET_OF_AREAS_VARIABLES>::type;

typedef BindingConstMarginCost< // Marginal cost for a binding constraint
  >
  VariablesPerBindingConstraints;

typedef Join<Join<
               // Variables for each area / links attached to the areas
               Areas<Economy::VariablesPerArea>,
               // Variables for each set of areas
               Join<SetsOfAreas<Economy::VariablesPerSetOfAreas>,
                    // Variables for each binding constraint
                    BindingConstraints<VariablesPerBindingConstraints>>>,
             Container::EndOfList>
  ItemList;

/*!
** \brief All variables for a simulation (economy)
*/
typedef Container::List<ItemList> AllVariables;

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_ALL_H__
