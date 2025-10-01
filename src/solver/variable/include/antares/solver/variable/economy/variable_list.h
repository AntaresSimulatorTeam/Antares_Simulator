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
#include "links.h"
#include "localMatchingRuleViolations.h"
#include "loldCsr.h"
#include "minDispatchableGenByPlant.h"
#include "nbOfDispatchedUnits.h"
#include "nbOfDispatchedUnitsByPlant.h"
#include "operatingCost.h"
#include "overallCost.h"
#include "price.h"
#include "productionByDispatchablePlant.h"
#include "productionByRenewablePlant.h"
#include "profitByPlant.h"
#include "spilledEnergy.h"
#include "thermalAirPollutantEmissions.h"
#include "unsupliedEnergyCsr.h"

// Add here all economic variables to aggregate

#define ECONOMY_SINGLE_AREA_VARIABLES                                                            \
    VCardOverallCost, VCardOverallCostCsr, VCardOperatingCost, VCardPrice, VCardPriceCSR,        \
      VCardThermalAirPollutantEmissions, VCardProductionByDispatchablePlant,                     \
      VCardMinDispatchableGenByPlant, VCardProductionByRenewablePlant, VCardBalance,             \
      VCardRowBalance, VCardPSP, VCardMiscGenMinusRowPSP, VCardTimeSeriesValuesLoad,             \
      VCardTimeSeriesValuesHydro, VCardTimeSeriesValuesWind, VCardTimeSeriesValuesSolar,         \
      VCardDispatchableGeneration, VCardRenewableGeneration, VCardHydroStorage, VCardPumping,    \
      VCardReservoirLevel, VCardInflows, VCardOverflow, VCardWaterValue, VCardHydroCost,         \
      VCardSTSbyGroup, VCardSTstorageInjectionByCluster, VCardSTstorageWithdrawalByCluster,      \
      VCardSTstorageLevelsByCluster, VCardSTstorageCashFlowByCluster, VCardUnsupliedEnergy,      \
      VCardUnsupliedEnergyCSR, VCardDomesticUnsuppliedEnergy, VCardLMRViolations,                \
      VCardSpilledEnergy, VCardLOLD, VCardLOLD_CSR, VCardLOLP, VCardNearPriceCap, VCardLOLP_CSR, \
      VCardAvailableDispatchGen, VCardDispatchableGenMargin, VCardDtgMarginCsr, VCardMarge,      \
      VCardMaxMrgCsr, VCardNonProportionalCost, VCardNonProportionalCostByDispatchablePlant,     \
      VCardNbOfDispatchedUnits, VCardNbOfDispatchedUnitsByPlant, VCardProfitByPlant,             \
      Variable::Economy::Links

#define ECONOMY_SET_OF_AREAS_VARIABLES                                                     \
    VCardMaxMrg, VCardPriceCSR, VCardNbOfDispatchedUnits, VCardNbOfDispatchedUnitsByPlant, \
      VCardOperatingCost, VCardOverallCost, VCardThermalAirPollutantEmissions,             \
      VCardProductionByDispatchablePlant, VCardProductionByRenewablePlant, VCardProfitByPlant
