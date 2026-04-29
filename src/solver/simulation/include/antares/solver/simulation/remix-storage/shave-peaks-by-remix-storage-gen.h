#pragma once

#include <memory>

#include "storage-for-remix-interface.h"

namespace Antares::Solver::Simulation
{
void shavePeaksByRemixingStorageGen(const std::vector<double>& Load,
                                    std::vector<double>& UnsupE,
                                    const std::vector<double>& Spillage,
                                    const std::vector<double>& DTG_MRG,
                                    ListStorageForRemix storagesForRemix);

void checkInput(const std::vector<double>& Load,
                const std::vector<double>& UnsupE,
                const std::vector<double>& Spillage,
                const std::vector<double>& DTG_MRG,
                const ListStorageForRemix& storagesForRemix);

void collectRemixDebugInfo(const ListStorageForRemix& storagesForRemix, std::stringstream& stream);

} // namespace Antares::Solver::Simulation
