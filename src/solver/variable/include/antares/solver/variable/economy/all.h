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

#include "bindingConstraints/bindingConstraintsMarginalCost.h"
#include "variable_list.h"

namespace Antares::Solver::Variable::Economy
{
/*
// Toutes les variables économiques pour une zone, factorisées via un template générique et une
liste centralisée using VariablesPerArea = typename
MakeSpatialAggregate<ECONOMY_VARIABLES>::type;

*/
using VariablesPerArea = ApplyChain<Variable::Economy::Links,
                                    ECONOMY_SINGLE_AREA_VARIABLES_CHAIN>::type;
using VariablesPerSetOfAreas = MakeSpatialAggregate<ECONOMY_SET_OF_AREAS_VARIABLES>::type;

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
