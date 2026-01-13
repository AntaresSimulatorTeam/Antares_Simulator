/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include <antares/solver/variable/commons/spatial-aggregate.h>
#include "antares/solver/variable/economy/STStorageInjectionByCluster.h"
#include "antares/solver/variable/variable.h"

#include "../area.h"
#include "../bindConstraints.h"
#include "../commons/compose-all.h"
#include "../commons/hydro.h"
#include "../commons/join.h"
#include "../commons/load.h"
#include "../commons/miscGenMinusRowPSP.h"
#include "../commons/psp.h"
#include "../commons/rowBalance.h"
#include "../commons/solar.h"
#include "../commons/wind.h"
#include "../setofareas.h"
#include "STSbyGroup.h"
#include "STStorageCashFlowByCluster.h"
#include "STStorageLevelsByCluster.h"
#include "STStorageWithdrawalByCluster.h"
#include "avail-dispatchable-generation.h"
#include "balance.h"
#include "bindingConstraints/bindingConstraintsMarginalCost.h"
#include "dispatchableGeneration.h"
#include "domesticUnsuppliedEnergy.h"
#include "dtgMarginAfterCsr.h"
#include "hydroCost.h"
#include "hydrostorage.h"
#include "inflow.h"
#include "links.h"
#include "localMatchingRuleViolations.h"
#include "lold.h"
#include "loldCsr.h"
#include "lolp.h"
#include "lolpCsr.h"
#include "max-mrg-csr.h"
#include "max-mrg.h"
#include "minDispatchableGenByPlant.h"
#include "nbOfDispatchedUnits.h"
#include "nbOfDispatchedUnitsByPlant.h"
#include "nearPriceCap.h"
#include "nonProportionalCost.h"
#include "npCostByDispatchablePlant.h"
#include "operatingCost.h"
#include "overallCost.h"
#include "overallCostCsr.h"
#include "overflow.h"
#include "price.h"
#include "priceCSR.h"
#include "productionByDispatchablePlant.h"
#include "productionByRenewablePlant.h"
#include "profitByPlant.h"
#include "pumping.h"
#include "renewableGeneration.h"
#include "reservoirlevel.h"
#include "spilledEnergy.h"
#include "thermalAirPollutantEmissions.h"
#include "unsupliedEnergy.h"
#include "waterValue.h"

namespace Antares::Solver::Variable::Economy
{

/*!
** \brief All variables for a single area (economy)
*/
using VariablesPerArea = Common::ComposeAll<OverallCost,
                                            OverallCostCsr,
                                            OperatingCost,
                                            Price,
                                            PriceCSR,
                                            ThermalAirPollutantEmissions,
                                            ProductionByDispatchablePlant,
                                            MinDispatchableGenByPlant,
                                            ProductionByRenewablePlant,
                                            Balance,
                                            RowBalance,
                                            PSP,
                                            MiscGenMinusRowPSP,
                                            TimeSeriesValuesLoad,
                                            TimeSeriesValuesHydro,
                                            TimeSeriesValuesWind,
                                            TimeSeriesValuesSolar,
                                            DispatchableGeneration,
                                            RenewableGeneration,
                                            HydroStorage,
                                            Pumping,
                                            ReservoirLevel,
                                            Inflows,
                                            Overflows,
                                            WaterValue,
                                            HydroCost,
                                            STSbyGroup,
                                            STstorageInjectionByCluster,
                                            STstorageWithdrawalByCluster,
                                            STstorageLevelsByCluster,
                                            STstorageCashFlowByCluster,
                                            UnsupliedEnergy,
                                            UnsupliedEnergyCSR,
                                            DomesticUnsuppliedEnergy,
                                            LMRViolations,
                                            SpilledEnergy,
                                            LOLD,
                                            LOLD_CSR,
                                            LOLP,
                                            NearPriceCap,
                                            LOLP_CSR,
                                            AvailableDispatchGen,
                                            DispatchableGenMargin,
                                            DtgMarginCsr,
                                            Marge,
                                            MaxMrgCsr,
                                            NonProportionalCost,
                                            NonProportionalCostByDispatchablePlant,
                                            NbOfDispatchedUnits,
                                            NbOfDispatchedUnitsByPlant,
                                            ProfitByPlant,
                                            Links>::type;

/*!\n** \brief All variables for a single set of areas (economy)
 */
using VariablesPerSetOfAreas = Common::SpatialAggregateAll<OverallCost,
                                                           OperatingCost,
                                                           Price,
                                                           ThermalAirPollutantEmissions,
                                                           Balance,
                                                           RowBalance,
                                                           PSP,
                                                           MiscGenMinusRowPSP,
                                                           TimeSeriesValuesLoad,
                                                           TimeSeriesValuesHydro,
                                                           TimeSeriesValuesWind,
                                                           TimeSeriesValuesSolar,
                                                           HydroStorage,
                                                           Pumping,
                                                           ReservoirLevel,
                                                           Inflows,
                                                           Overflows,
                                                           WaterValue,
                                                           HydroCost,
                                                           UnsupliedEnergy,
                                                           DomesticUnsuppliedEnergy,
                                                           LMRViolations,
                                                           SpilledEnergy,
                                                           LOLD,
                                                           LOLP,
                                                           NearPriceCap,
                                                           AvailableDispatchGen,
                                                           DispatchableGenMargin,
                                                           DtgMarginCsr,
                                                           Marge,
                                                           NonProportionalCost,
                                                           NbOfDispatchedUnits>::type;

typedef BindingConstMarginCost< // Marginal cost for a binding constraint
  Container::EndOfList          // End of variable list
  >

  VariablesPerBindingConstraints;

typedef Variable::Join<
  // Variables for each area / links attached to the areas
  Variable::Areas<VariablesPerArea>,
  // Variables for each set of areas
  Variable::Join<Variable::SetsOfAreas<VariablesPerSetOfAreas>,
                 // Variables for each binding constraint
                 Variable::BindingConstraints<VariablesPerBindingConstraints>>>
  ItemList;

/*!
** \brief All variables for a simulation (economy)
*/
typedef Container::List<ItemList> AllVariables;

} // namespace Antares::Solver::Variable::Economy
