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

#include "antares/study/parts/hydro/hydromaxtimeseriesreader.h"

#include "antares/study/study.h"
#include <antares/inifile/inifile.h>

using namespace Yuni;

#define SEP IO::Separator

namespace Antares::Data
{

HydroMaxTimeSeriesReader::HydroMaxTimeSeriesReader(PartHydro& hydro,
                                                   std::string areaID,
                                                   std::string areaName):
    hydro_(hydro),
    areaID_(areaID),
    areaName_(areaName)
{
    dailyMaxPumpAndGen.reset(4U, DAYS_PER_YEAR, true);
}

bool HydroMaxTimeSeriesReader::loadDailyMaxPowersAndEnergies(const AnyString& folder,
                                                             bool usedBySolver)
{
    YString filePath;
    Matrix<>::BufferType fileContent;
    bool ret = true;

    filePath.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "maxpower_"
                     << areaID_ << ".txt";

    //  It is necessary to load maxpower_ txt file, whether loading is called from old GUI
    //  or from solver.

    if (!usedBySolver)
    {
        bool enabledModeIsChanged = false;

        if (JIT::enabled)
        {
            JIT::enabled = false; // Allowing to read the area's daily max power and energy
            enabledModeIsChanged = true;
        }

        ret = dailyMaxPumpAndGen.loadFromCSVFile(filePath,
                                                 4U,
                                                 DAYS_PER_YEAR,
                                                 Matrix<>::optFixedSize,
                                                 &fileContent)
              && ret;

        if (enabledModeIsChanged)
        {
            JIT::enabled = true; // Back to the previous loading mode.
        }
    }
    else
    {
        ret = dailyMaxPumpAndGen.loadFromCSVFile(filePath,
                                                 4U,
                                                 DAYS_PER_YEAR,
                                                 Matrix<>::optFixedSize,
                                                 &fileContent)
              && ret;

        bool errorPowers = false;
        for (uint i = 0; i < 4U; ++i)
        {
            auto& col = dailyMaxPumpAndGen[i];
            for (uint day = 0; day < DAYS_PER_YEAR; ++day)
            {
                if (!errorPowers && (col[day] < 0. || (i % 2U /*column hours*/ && col[day] > 24.)))
                {
                    logs.error() << areaName_ << ": invalid power or energy value";
                    errorPowers = true;
                    ret = false;
                }
            }
        }
    }
    return ret;
}

void HydroMaxTimeSeriesReader::copyDailyMaxEnergy() const
{
    copyDailyMaxGenerationEnergy();
    copyDailyMaxPumpingEnergy();
}

void HydroMaxTimeSeriesReader::copyDailyMaxGenerationEnergy() const
{
    auto& dailyNbHoursAtGenPmax = hydro_.dailyNbHoursAtGenPmax;
    const auto& dailyMaxGenE = dailyMaxPumpAndGen[genMaxE];

    dailyNbHoursAtGenPmax.reset(1U, DAYS_PER_YEAR, true);

    dailyNbHoursAtGenPmax.pasteToColumn(0, dailyMaxGenE);
}

void HydroMaxTimeSeriesReader::copyDailyMaxPumpingEnergy() const
{
    auto& dailyNbHoursAtPumpPmax = hydro_.dailyNbHoursAtPumpPmax;
    const auto& dailyMaxPumpE = dailyMaxPumpAndGen[pumpMaxE];

    dailyNbHoursAtPumpPmax.reset(1U, DAYS_PER_YEAR, true);

    dailyNbHoursAtPumpPmax.pasteToColumn(0, dailyMaxPumpE);
}

bool HydroMaxTimeSeriesReader::read(const AnyString& folder, bool usedBySolver)
{
    bool ret = loadDailyMaxPowersAndEnergies(folder, usedBySolver);
    copyDailyMaxEnergy();
    hydro_.series->buildHourlyMaxPowerFromDailyTS(dailyMaxPumpAndGen[genMaxP],
                                                  dailyMaxPumpAndGen[pumpMaxP]);

    return ret;
}

} // namespace Antares::Data
