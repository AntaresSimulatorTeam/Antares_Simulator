// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "reserveParticipationBase.h"

namespace Antares::Solver::Variable::Economy::Reserves
{

using VCardReserveParticipationMarginalCost = VCardReserveParticipationBase<MarginalCostTraits>;

inline std::string marginalCostToString()
{
    return "MRG.COST";
}

} // namespace Antares::Solver::Variable::Economy::Reserves
