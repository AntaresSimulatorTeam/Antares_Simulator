// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/load-options.h"

#include <antares/exception/LoadingError.hpp>
#include <antares/logs/logs.h>

namespace Antares::Data
{
StudyLoadOptions::StudyLoadOptions():
    nbYears(0),
    forceYearByYear(false),
    forceDerated(false),
    noTimeseriesImportIntoInput(false),
    simplexOptimizationRange(sorUnknown),
    mpsToExport(false),
    ignoreConstraints(false),
    forceMode(SimulationMode::Unknown),
    enableParallel(false),
    forceParallel(false),
    maxNbYearsInParallel(0)
{
}

void StudyLoadOptions::checkForceSimulationMode()
{
    const unsigned int number_of_enabled_force_options = static_cast<unsigned int>(forceExpansion)
                                                 + static_cast<unsigned int>(forceEconomy)
                                                 + static_cast<unsigned int>(forceAdequacy);

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
