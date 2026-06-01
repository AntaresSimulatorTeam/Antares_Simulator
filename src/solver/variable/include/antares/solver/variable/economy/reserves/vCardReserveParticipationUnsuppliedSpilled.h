/*
** Copyright 2007-2025 RTE
** Copyright 2007-2025 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
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
