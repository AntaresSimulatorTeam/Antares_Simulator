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
    ret &= max_.timeSeries.loadFromCSVFile(filePath.string(), 1, DAYS_PER_YEAR, &fileContent);
    filePath = path / "minDailyReservoirLevels.txt";
    ret &= min_.timeSeries.loadFromCSVFile(filePath.string(), 1, DAYS_PER_YEAR, &fileContent);
    filePath = path / "avgDailyReservoirLevels.txt";
    ret &= avg_.timeSeries.loadFromCSVFile(filePath.string(), 1, DAYS_PER_YEAR, &fileContent);

    return ret;
}

bool StandardRuleCurvesLoader::load()
{
    const std::filesystem::path filePath = baseFolder_ / "common" / "capacity"
                                           / std::string("reservoir_" + areaID_ + ".txt");

    Matrix<double> standardRuleCurves;
    standardRuleCurves.reset(3L, DAYS_PER_YEAR, true);

    Matrix<>::BufferType fileContent;
    bool ret = true;

    ret = standardRuleCurves.loadFromCSVFile(filePath.string(),
                                             3,
                                             DAYS_PER_YEAR,
                                             Matrix<>::optFixedSize,
                                             &fileContent);

    min_.timeSeries.reset(1U, DAYS_PER_YEAR, true);
    min_.timeSeries.pasteToColumn(0, standardRuleCurves[RuleCurves::minimum]);
    avg_.timeSeries.reset(1U, DAYS_PER_YEAR, true);
    avg_.timeSeries.pasteToColumn(0, standardRuleCurves[RuleCurves::average]);
    max_.timeSeries.reset(1U, DAYS_PER_YEAR, true);
    max_.timeSeries.pasteToColumn(0, standardRuleCurves[RuleCurves::maximum]);

    return ret;
}

std::unique_ptr<RuleCurvesLoader> RuleCurvesLoaderService::createRuleCurvesLoader(
  Parameters::Compatibility::HydroRuleCurves hydroRuleCurves,
  const std::filesystem::path& filePath,
  const std::string& areaID)
{
    switch (hydroRuleCurves)
    {
    case Parameters::Compatibility::HydroRuleCurves::Single:
    {
        return std::make_unique<StandardRuleCurvesLoader>(filePath,
                                                          areaID,
                                                          ruleCurves_.max,
                                                          ruleCurves_.avg,
                                                          ruleCurves_.min);
    }
    case Parameters::Compatibility::HydroRuleCurves::Scenarized:
    {
        return std::make_unique<ScenarizedRuleCurvesLoader>(filePath,
                                                            areaID,
                                                            ruleCurves_.max,
                                                            ruleCurves_.avg,
                                                            ruleCurves_.min);
    }
    default:
        throw std::invalid_argument("Value not supported for hydro rule curves compatibility");
    }
}

bool RuleCurvesLoaderService::LoadFromFolder(
  const std::string& areaID,
  const std::filesystem::path& folder,
  Parameters::Compatibility::HydroRuleCurves hydroRuleCurves)
{
    bool ret = true;

    auto loader = createRuleCurvesLoader(hydroRuleCurves, folder, areaID);
    ret = loader->load();

    return ret;
}

} // namespace Antares::Data
