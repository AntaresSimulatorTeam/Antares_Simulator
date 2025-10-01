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
