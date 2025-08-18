
#pragma once

#include <memory>

#include "hydro-for-remix.h"
#include "storage-for-remix.h" // gp : will be removed (needed by common-hydro-remix.cpp)

namespace Antares::Solver::Simulation
{
void shavePeaksByRemixingStorageGen(const std::vector<double>& Load,
                                    std::vector<double>& UnsupE,
                                    const std::vector<double>& Spillage,
                                    const std::vector<double>& DTG_MRG,
                                    std::shared_ptr<StorageForRemix> storage);

void checkInput(const std::vector<double>& Load,
                const std::vector<double>& UnsupE,
                const std::vector<double>& Spillage,
                const std::vector<double>& DTG_MRG,
                const std::vector<double>& storageGen);

} // namespace Antares::Solver::Simulation
