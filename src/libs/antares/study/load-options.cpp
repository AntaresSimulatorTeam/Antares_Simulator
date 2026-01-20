// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/load-options.h"

#include <antares/exception/LoadingError.hpp>
#include <antares/logs/logs.h>

namespace Antares::Data
{
StudyLoadOptions::StudyLoadOptions():
    nbYears(0),
    prepareOutput(false),
    loadOnlyNeeded(false),
    forceYearByYear(false),
    forceDerated(false),
    noTimeseriesImportIntoInput(false),
    simplexOptimizationRange(sorUnknown),
    mpsToExport(false),
    ignoreConstraints(false),
    forceMode(SimulationMode::Unknown),
    enableParallel(false),
    forceParallel(false),
    maxNbYearsInParallel(0),
    usedByTheSolver(false)
{
}

void StudyLoadOptions::checkForceSimulationMode()
{
    const uint number_of_enabled_force_options = static_cast<uint>(forceExpansion)
                                                 + static_cast<uint>(forceEconomy)
                                                 + static_cast<uint>(forceAdequacy);

    if (number_of_enabled_force_options > 1)
    {
        throw Error::InvalidSimulationMode();
    }
    if (forceExpansion)
    {
        forceMode = SimulationMode::Expansion;
    }
    else if (forceEconomy)
    {
        forceMode = SimulationMode::Economy;
    }
    else if (forceAdequacy)
    {
        forceMode = SimulationMode::Adequacy;
    }
}
} // namespace Antares::Data
