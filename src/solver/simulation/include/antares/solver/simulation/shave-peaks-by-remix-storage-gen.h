
#pragma once

#include <memory>

#include "hydro-for-remix.h"
#include "storage-for-remix.h" // gp : will be removed (needed by common-hydro-remix.cpp)

namespace Antares::Solver::Simulation
{
void shavePeaksByRemixingStorageGen(std::vector<double>& UnsupE,
                                    const std::vector<double>& DispatchGen,
                                    const std::vector<double>& Spillage,
                                    const std::vector<double>& DTG_MRG,
                                    std::shared_ptr<StorageForRemix> storage);

} // namespace Antares::Solver::Simulation
