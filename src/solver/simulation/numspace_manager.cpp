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

#include <stdexcept>

#include <antares/solver/simulation/numspace_manager.h>

namespace Antares::Solver::Simulation
{
NumSpaceManager::NumSpaceManager(unsigned N):
    available(N, true)
{
}

unsigned NumSpaceManager::getAvailableNumSpace()
{
    // std::find not available for std::vector<bool>
    std::unique_lock lk(mut);
    for (unsigned idx = 0; idx < available.size(); ++idx)
    {
        if (available[idx])
        {
            available[idx] = false;
            return static_cast<unsigned>(idx);
        }
    }
    throw std::runtime_error("No numspaces could be allocated, aborting");
}

void NumSpaceManager::freeNumSpace(unsigned numSpace)
{
    std::unique_lock lk(mut);
    available[numSpace] = true;
}

} // namespace Antares::Solver::Simulation
