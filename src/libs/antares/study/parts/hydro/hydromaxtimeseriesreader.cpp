// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/hydro/hydromaxtimeseriesreader.h"

#include <antares/inifile/inifile.h>
#include "antares/study/study.h"

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
    hydro_.dailyMaxPumpAndGen.reset(4U, DAYS_PER_YEAR, true);
}

static bool checkPower(const Matrix<>& dailyMaxPumpAndGen, const std::string& areaName)
{
    for (uint i = 0; i < 4U; ++i)
    {
        auto& col = dailyMaxPumpAndGen[i];
        for (uint day = 0; day < DAYS_PER_YEAR; ++day)
        {
            if (col[day] < 0. || (i % 2U /*column hours*/ && col[day] > 24.))
            {
                logs.error() << areaName << ": invalid power or energy value";
                return false;
            }
        }
    }

    return true;
}

bool HydroMaxTimeSeriesReader::loadDailyMaxPowersAndEnergies(const AnyString& folder)
{
    YString filePath;
    Matrix<>::BufferType fileContent;
    bool ret = true;

    filePath.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "maxpower_"
                     << areaID_ << ".txt";

    //  It is necessary to load maxpower_ txt file.

    ret = hydro_.dailyMaxPumpAndGen.loadFromCSVFile(filePath,
                                                    4U,
                                                    DAYS_PER_YEAR,
                                                    Matrix<>::optFixedSize,
                                                    &fileContent)
          && ret;
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
    const auto& dailyMaxGenE = hydro_.dailyMaxPumpAndGen[genMaxE];

    dailyNbHoursAtGenPmax.reset(1U, DAYS_PER_YEAR, true);

    dailyNbHoursAtGenPmax.pasteToColumn(0, dailyMaxGenE);
}

void HydroMaxTimeSeriesReader::copyDailyMaxPumpingEnergy() const
{
    auto& dailyNbHoursAtPumpPmax = hydro_.dailyNbHoursAtPumpPmax;
    const auto& dailyMaxPumpE = hydro_.dailyMaxPumpAndGen[pumpMaxE];

    dailyNbHoursAtPumpPmax.reset(1U, DAYS_PER_YEAR, true);

    dailyNbHoursAtPumpPmax.pasteToColumn(0, dailyMaxPumpE);
}

bool HydroMaxTimeSeriesReader::read(const AnyString& folder)
{
    bool ret = loadDailyMaxPowersAndEnergies(folder);
    ret = checkPower(hydro_.dailyMaxPumpAndGen, areaName_) && ret;
    copyDailyMaxEnergy();
    hydro_.series->buildHourlyMaxPowerFromDailyTS(hydro_.dailyMaxPumpAndGen[genMaxP],
                                                  hydro_.dailyMaxPumpAndGen[pumpMaxP]);

    return ret;
}

} // namespace Antares::Data
