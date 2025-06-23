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

#include <algorithm>

#include <yuni/yuni.h>
#include <yuni/io/file.h>

#include <antares/exception/LoadingError.hpp>
#include <antares/inifile/inifile.h>
#include <antares/logs/logs.h>
#include <antares/study/parts/hydro/hydrorulecurves.h>
#include "antares/study/study.h"

namespace fs = std::filesystem;

namespace Antares::Data
{
RuleCurves::RuleCurves(TimeSeriesNumbers& timeseriesNumbers):
    timeseriesNumbers(timeseriesNumbers),
    max(timeseriesNumbers),
    min(timeseriesNumbers),
    avg(timeseriesNumbers)
{
    timeseriesNumbers.registerSeries(&max, "max-reservoir-level");
    timeseriesNumbers.registerSeries(&min, "min-reservoir-level");
    timeseriesNumbers.registerSeries(&avg, "avg-reservoir-level");

    max.reset(1U, DAYS_PER_YEAR);
    max.fill(1.0);
    avg.reset(1U, DAYS_PER_YEAR);
    avg.fill(0.5);
    min.reset(1U, DAYS_PER_YEAR);
    standardRuleCurvesGUI.reset(3L, DAYS_PER_YEAR, true);
    standardRuleCurvesGUI.fillColumn(RuleCurves::maximum, 1.);
    standardRuleCurvesGUI.fillColumn(RuleCurves::average, 0.5);
}

bool RuleCurves::forceReload(bool reload) const
{
    bool ret = true;
    ret = max.forceReload(reload) && ret;
    ret = min.forceReload(reload) && ret;
    ret = avg.forceReload(reload) && ret;
    ret = standardRuleCurvesGUI.forceReload(reload) && ret;

    return ret;
}

void RuleCurves::markAsModified() const
{
    max.markAsModified();
    min.markAsModified();
    avg.markAsModified();
    standardRuleCurvesGUI.markAsModified();
}

bool RuleCurves::saveToFolder(const std::string& areaID, const std::string& folder) const
{
    bool ret = true;
    std::string buffer;
    buffer = folder + "/" + "common" + "/" + "capacity" + "/" + "reservoir_" + areaID + ".txt";

    ret = standardRuleCurvesGUI.saveToCSVFile(buffer, /*decimal*/ 3) && ret;

    return ret;
}

void RuleCurves::averageTimeSeries()
{
    max.averageTimeseries();
    min.averageTimeseries();
    avg.averageTimeseries();
}

bool ScenarizedRuleCurvesLoader::load()
{
    const std::filesystem::path path = baseFolder_ / "series" / areaID_;
    Matrix<>::BufferType fileContent;

    bool ret = true;

    fs::path filePath = path / "maxDailyReservoirLevels.txt";
    ret = max_.timeSeries.loadFromCSVFile(filePath.string(), 1, DAYS_PER_YEAR, &fileContent) && ret;
    filePath = path / "minDailyReservoirLevels.txt";
    ret = min_.timeSeries.loadFromCSVFile(filePath.string(), 1, DAYS_PER_YEAR, &fileContent) && ret;
    filePath = path / "avgDailyReservoirLevels.txt";
    ret = avg_.timeSeries.loadFromCSVFile(filePath.string(), 1, DAYS_PER_YEAR, &fileContent) && ret;

    return ret;
}

bool StandardRuleCurvesLoader::load()
{
    const std::filesystem::path filePath = baseFolder_ / "common" / "capacity"
                                           / std::string("reservoir_" + areaID_ + ".txt");
    Matrix<>::BufferType fileContent;
    bool ret = true;

    ret = standardRuleCurvesMatrixGUI_.loadFromCSVFile(filePath.string(),
                                                       3,
                                                       DAYS_PER_YEAR,
                                                       Matrix<>::optFixedSize,
                                                       &fileContent)
          && ret;
    copyRuleCurvesFromBuffer();

    return ret;
}

void StandardRuleCurvesLoader::copyRuleCurvesFromBuffer()
{
    min_.timeSeries.reset(1U, DAYS_PER_YEAR, true);
    min_.timeSeries.pasteToColumn(0, standardRuleCurvesMatrixGUI_[RuleCurves::minimum]);
    avg_.timeSeries.reset(1U, DAYS_PER_YEAR, true);
    avg_.timeSeries.pasteToColumn(0, standardRuleCurvesMatrixGUI_[RuleCurves::average]);
    max_.timeSeries.reset(1U, DAYS_PER_YEAR, true);
    max_.timeSeries.pasteToColumn(0, standardRuleCurvesMatrixGUI_[RuleCurves::maximum]);
}

std::unique_ptr<RuleCurvesLoader> RuleCurvesLoaderService::createRuleCurvesLoader(
  Parameters::Compatibility::HydroRuleCurves hydroRuleCurves,
  const std::filesystem::path& filePath,
  const std::string& areaID,
  Matrix<double>& standardRuleCurvesGUI,
  TimeSeries& max,
  TimeSeries& avg,
  TimeSeries& min)
{
    switch (hydroRuleCurves)
    {
    case Parameters::Compatibility::HydroRuleCurves::Single:
    {
        return std::make_unique<StandardRuleCurvesLoader>(filePath,
                                                          areaID,
                                                          standardRuleCurvesGUI,
                                                          max,
                                                          avg,
                                                          min);
    }
    case Parameters::Compatibility::HydroRuleCurves::Scenarized:
    {
        return std::make_unique<ScenarizedRuleCurvesLoader>(filePath, areaID, max, avg, min);
    }
    default:
        throw std::invalid_argument("Value not supported for hydro rule curves compatibility");
    }
}

bool RuleCurvesLoaderService::LoadFromFolder(
  const std::string& areaID,
  const std::filesystem::path& folder,
  bool usedBySolver,
  Parameters::Compatibility::HydroRuleCurves hydroRuleCurves)
{
    bool ret = true;

    if (!usedBySolver)
    {
        Matrix<>::BufferType fileContent;
        fs::path filePath = folder / "common" / "capacity"
                            / std::string("reservoir_" + areaID + ".txt");
        ruleCurves_.standardRuleCurvesGUI.reset(3, DAYS_PER_YEAR, true);
        bool enabledModeIsChanged = false;

        if (JIT::enabled)
        {
            JIT::enabled = false;
            enabledModeIsChanged = true;
        }

        ret = ruleCurves_.standardRuleCurvesGUI.loadFromCSVFile(filePath.string(),
                                                                3,
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
        auto loader = createRuleCurvesLoader(hydroRuleCurves,
                                             folder,
                                             areaID,
                                             ruleCurves_.standardRuleCurvesGUI,
                                             ruleCurves_.max,
                                             ruleCurves_.avg,
                                             ruleCurves_.min);
        ret = loader->load() && ret;
    }
    return ret;
}

} // namespace Antares::Data
