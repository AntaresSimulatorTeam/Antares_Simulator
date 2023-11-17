/*
** Copyright 2007-2023 RTE
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

#include "load-options.h"
#include <antares/logs/logs.h>

#include <antares/exception/LoadingError.hpp>

namespace Antares
{
namespace Data
{
StudyLoadOptions::StudyLoadOptions() :
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
 usedByTheSolver(false),
 ortoolsUsed(false)
{
}

void StudyLoadOptions::pushProgressLogs() const
{
    if (loadOnlyNeeded && progressTicks)
    {
        uint percent = progressTicks * 100 / progressTickCount;
        if (percent < 100)
            logs.info() << logMessage << "  " << percent << '%';
    }
}

void StudyLoadOptions::checkForceSimulationMode()
{
    const uint number_of_enabled_force_options
      = static_cast<uint>(forceExpansion) + static_cast<uint>(forceEconomy)
        + static_cast<uint>(forceAdequacy);

    if (number_of_enabled_force_options > 1)
    {
        throw Error::InvalidSimulationMode();
    }
    if (forceExpansion)
        forceMode = SimulationMode::Expansion;
    else if (forceEconomy)
        forceMode = SimulationMode::Economy;
    else if (forceAdequacy)
        forceMode = SimulationMode::Adequacy;
}
} // namespace Data
} // namespace Antares
