// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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

// Make Links a template alias to be used in Common::ComposeAll
template<class>
using Links = Antares::Solver::Variable::Links<VariablePerLink>;

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_LINK_H__
