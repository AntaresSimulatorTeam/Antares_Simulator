// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ADEQUACY_LINK_H__
#define __SOLVER_VARIABLE_ADEQUACY_LINK_H__

#include <yuni/yuni.h>

// TODO SL: should be moved to common
#include "antares/solver/variable//commons/links/links.h"
#include "antares/solver/variable/commons/compose-all.h"
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
using VariablePerLink = Common::ComposeAll<Economy::FlowLinear,
                                           Economy::FlowLinearAbs,
                                           Economy::FlowQuad,
                                           Economy::CongestionFee,
                                           Economy::CongestionFeeAbs,
                                           Economy::MarginalCost,
                                           Economy::CongestionProbability,
                                           Economy::HurdleCosts>::type;

template<class>
using Links = Antares::Solver::Variable::Links<VariablePerLink>;

} // namespace Antares::Solver::Variable::Adequacy

#endif // __SOLVER_VARIABLE_ADEQUACY_LINK_H__
