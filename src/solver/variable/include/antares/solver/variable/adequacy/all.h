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
#ifndef __SOLVER_VARIABLE_ADEQUACY_ALL_H__
#define __SOLVER_VARIABLE_ADEQUACY_ALL_H__

#include "antares/solver/variable/adequacy/links.h"
#include "antares/solver/variable/adequacy/overallCost.h"
#include "antares/solver/variable/adequacy/spilledEnergy.h"
#include "antares/solver/variable/area.h"
#include "antares/solver/variable/bindConstraints.h"
#include "antares/solver/variable/commons/hydro.h"
#include "antares/solver/variable/commons/join.h"
#include "antares/solver/variable/commons/load.h"
#include "antares/solver/variable/commons/miscGenMinusRowPSP.h"
#include "antares/solver/variable/commons/psp.h"
#include "antares/solver/variable/commons/rowBalance.h"
#include "antares/solver/variable/commons/solar.h"
#include "antares/solver/variable/commons/spatial-aggregate.h"
#include "antares/solver/variable/commons/wind.h"
#include "antares/solver/variable/economy/STSbyGroup.h"
#include "antares/solver/variable/economy/STStorageInjectionByCluster.h"
#include "antares/solver/variable/economy/STStorageLevelsByCluster.h"
#include "antares/solver/variable/economy/STStorageWithdrawalByCluster.h"
#include "antares/solver/variable/economy/avail-dispatchable-generation.h"
#include "antares/solver/variable/economy/balance.h"
#include "antares/solver/variable/economy/dispatchable-generation-margin.h"
#include "antares/solver/variable/economy/dispatchableGeneration.h"
#include "antares/solver/variable/economy/hydroCost.h"
#include "antares/solver/variable/economy/hydrostorage.h"
#include "antares/solver/variable/economy/inflow.h"
#include "antares/solver/variable/economy/lold.h"
#include "antares/solver/variable/economy/lolp.h"
#include "antares/solver/variable/economy/max-mrg.h"
#include "antares/solver/variable/economy/operatingCost.h"
#include "antares/solver/variable/economy/overflow.h"
#include "antares/solver/variable/economy/price.h"
#include "antares/solver/variable/economy/productionByDispatchablePlant.h"
#include "antares/solver/variable/economy/productionByRenewablePlant.h"
#include "antares/solver/variable/economy/pumping.h"
#include "antares/solver/variable/economy/renewableGeneration.h"
#include "antares/solver/variable/economy/reservoirlevel.h"
#include "antares/solver/variable/economy/residual.h"
#include "antares/solver/variable/economy/thermalAirPollutantEmissions.h"
#include "antares/solver/variable/economy/unsupliedEnergy.h"
#include "antares/solver/variable/economy/waterValue.h"
#include "antares/solver/variable/setofareas.h"
#include "antares/solver/variable/variable.h"

// Output variables associated to binding constraints
#include "antares/solver/variable//economy/bindingConstraints/bindingConstraintsMarginalCost.h"

// By thermal plant
#include "antares/solver/variable/commons/compose-all.h"

#include "../economy/profitByPlant.h"

namespace Antares::Solver::Variable::Adequacy
{
/*!
** \brief All variables for a single area (economy)
*/
using VariablesPerArea = Common::ComposeAll<Variable::Adequacy::OverallCost,
                                            Variable::Economy::OperatingCost,
                                            Variable::Economy::Price,
                                            Variable::Economy::ThermalAirPollutantEmissions,
                                            Variable::Economy::ProductionByDispatchablePlant,
                                            Variable::Economy::ProductionByRenewablePlant,
                                            Variable::Economy::Balance,
                                            Variable::Economy::RowBalance,
                                            Variable::Economy::PSP,
                                            Variable::Economy::MiscGenMinusRowPSP,
                                            Variable::Economy::TimeSeriesValuesLoad,
                                            Variable::Economy::TimeSeriesValuesHydro,
                                            Variable::Economy::TimeSeriesValuesWind,
                                            Variable::Economy::TimeSeriesValuesSolar,
                                            Variable::Economy::DispatchableGeneration,
                                            Variable::Economy::RenewableGeneration,
                                            Variable::Economy::HydroStorage,
                                            Variable::Economy::Pumping,
                                            Variable::Economy::ReservoirLevel,
                                            Variable::Economy::Inflows,
                                            Variable::Economy::Overflows,
                                            Variable::Economy::WaterValue,
                                            Variable::Economy::HydroCost,
                                            Variable::Economy::STSbyGroup,
                                            Variable::Economy::STstorageInjectionByCluster,
                                            Variable::Economy::STstorageWithdrawalByCluster,
                                            Variable::Economy::STstorageLevelsByCluster,
                                            Variable::Economy::UnsupliedEnergy,
                                            Variable::Adequacy::SpilledEnergy,
                                            Variable::Economy::LOLD,
                                            Variable::Economy::LOLP,
                                            Variable::Economy::AvailableDispatchGen,
                                            Variable::Economy::DispatchableGenMargin,
                                            Variable::Economy::Marge,
                                            Variable::Economy::ProfitByPlant,
                                            Variable::Economy::ResidualLoad,
                                            Variable::Adequacy::Links>::type;

/*!
** \brief All variables for a single set of areas (economy)
*/
using VariablesPerSetOfAreas = Common::SpatialAggregateAll<
  Variable::Adequacy::OverallCost,
  Variable::Economy::OperatingCost,
  Variable::Economy::Price,
  Variable::Economy::ThermalAirPollutantEmissions,
  Variable::Economy::Balance,
  Variable::Economy::RowBalance,
  Variable::Economy::PSP,
  Variable::Economy::MiscGenMinusRowPSP,
  Variable::Economy::TimeSeriesValuesLoad,
  Variable::Economy::TimeSeriesValuesHydro,
  Variable::Economy::TimeSeriesValuesWind,
  Variable::Economy::TimeSeriesValuesSolar,
  Variable::Economy::HydroStorage,
  Variable::Economy::Pumping,
  Variable::Economy::ReservoirLevel,
  Variable::Economy::Inflows,
  Variable::Economy::Overflows,
  Variable::Economy::WaterValue,
  Variable::Economy::HydroCost,
  Variable::Economy::UnsupliedEnergy,
  Variable::Adequacy::SpilledEnergy,
  Variable::Economy::LOLD,
  Variable::Economy::LOLP,
  Variable::Economy::AvailableDispatchGen,
  Variable::Economy::DispatchableGenMargin,
  Variable::Economy::Marge>::type;

typedef Variable::Economy::BindingConstMarginCost< // Marginal cost for a binding constraint
  Container::EndOfList                             // End of variable list
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

} // namespace Antares::Solver::Variable::Adequacy

#endif // __SOLVER_VARIABLE_ADEQUACY_ALL_H__
