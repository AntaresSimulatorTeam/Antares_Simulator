// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <mutex>
#include <vector>

namespace Antares::Solver::Simulation
{
class NumSpaceManager final
{
public:
    explicit NumSpaceManager(unsigned N);
    unsigned getAvailableNumSpace();
    void freeNumSpace(unsigned numSpace);

private:
    std::mutex mut;
    std::vector<bool> available;
};
} // namespace Antares::Solver::Simulation
