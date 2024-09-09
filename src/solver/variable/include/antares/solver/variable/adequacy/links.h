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
#ifndef __SOLVER_VARIABLE_ADEQUACY_LINK_H__
#define __SOLVER_VARIABLE_ADEQUACY_LINK_H__

#include <yuni/yuni.h>

// TODO SL: should be moved to common
#include "antares/solver/variable//commons/links/links.h"
#include "antares/solver/variable/economy/links/congestionFee.h"
#include "antares/solver/variable/economy/links/congestionFeeAbs.h"
#include "antares/solver/variable/economy/links/congestionProbability.h"
#include "antares/solver/variable/economy/links/flowLinear.h"
#include "antares/solver/variable/economy/links/flowLinearAbs.h"
#include "antares/solver/variable/economy/links/flowQuad.h"
#include "antares/solver/variable/economy/links/hurdleCosts.h"
#include "antares/solver/variable/economy/links/marginalCost.h"

namespace Antares::Solver::Variable::Adequacy
{
/*!
** \brief All variables for a single link.
*
* #
*/
using VariablePerLink = Economy::FlowLinear // Flow linear
  <Economy::FlowLinearAbs                   // Flow linear Abs
   <Economy::FlowQuad                       // Flow Quad
    <Economy::CongestionFee                 // Congestion Fee
     <Economy::CongestionFeeAbs             // Congestion Fee (Abs)
      <Economy::MarginalCost                // Marginal Cost
       <Economy::CongestionProbability      // Congestion Probability (+/-)
        <Economy::HurdleCosts               // Hurdle costs
         <>>>>>>>>;

using Links = Antares::Solver::Variable::Links<VariablePerLink>;

} // namespace Antares::Solver::Variable::Adequacy

#endif // __SOLVER_VARIABLE_ADEQUACY_LINK_H__
