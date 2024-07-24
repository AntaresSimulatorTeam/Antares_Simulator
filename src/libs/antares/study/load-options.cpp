/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

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
