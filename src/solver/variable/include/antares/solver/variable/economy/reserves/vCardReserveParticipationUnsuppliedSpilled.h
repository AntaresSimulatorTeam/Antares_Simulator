// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "reserveParticipationBase.h"

namespace Antares::Solver::Variable::Economy::Reserves
{

using VCardReserveParticipationUnsuppliedSpilled = VCardReserveParticipationBase<
  UnsuppliedSpilledTraits>;

constexpr std::string_view unsuppliedSpilledToString(Data::UnsuppliedSpilled type)
{
    using enum Data::UnsuppliedSpilled;

    switch (type)
    {
    case Unsupplied:
        return "UNSP.";
    case Spilled:
        return "SPIL.";
    case Count:
        break;
    }

    throw std::invalid_argument("Invalid UnsuppliedSpilled value: "
                                + std::to_string(static_cast<int>(type))
                                + " should be 0 (UNSP.) or 1 (SPIL.)");
}

} // namespace Antares::Solver::Variable::Economy::Reserves
