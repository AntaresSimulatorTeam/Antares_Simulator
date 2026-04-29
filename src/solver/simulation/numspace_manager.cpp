// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
