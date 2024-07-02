/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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
#ifndef __SOLVER_VARIABLE_ECONOMY_LINK_H__
#define __SOLVER_VARIABLE_ECONOMY_LINK_H__

#include <yuni/yuni.h>

#include "antares/solver/variable/economy/links/congestionFee.h"
#include "antares/solver/variable/economy/links/congestionFeeAbs.h"
#include "antares/solver/variable/economy/links/congestionProbability.h"
#include "antares/solver/variable/economy/links/flowLinear.h"
#include "antares/solver/variable/economy/links/flowLinearAbs.h"
#include "antares/solver/variable/economy/links/flowQuad.h"
#include "antares/solver/variable/economy/links/hurdleCosts.h"
#include "antares/solver/variable/economy/links/loopFlow.h"
#include "antares/solver/variable/economy/links/marginalCost.h"

#include "../commons/links/links.h"

namespace Antares::Solver::Variable::Economy
{
/*!
** \brief All variables for a single link (economy)
*/
typedef FlowLinear             // Flow linear
  <FlowLinearAbs               // Flow linear Abs
   <LoopFlow                   // Loop flow
    <FlowQuad                  // Flow Quad
     <CongestionFee            // Congestion Fee
      <CongestionFeeAbs        // Congestion Fee (Abs)
       <MarginalCost           // Marginal Cost
        <CongestionProbability // Congestion Probability (+/-)
         <HurdleCosts          // Hurdle costs
          <>>>>>>>>>
    VariablePerLink;

using Links = Antares::Solver::Variable::Links<VariablePerLink>;

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_LINK_H__
