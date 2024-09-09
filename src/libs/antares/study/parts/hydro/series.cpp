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

#include <yuni/yuni.h>
#include <yuni/io/file.h>
#include "antares/study/parts/hydro/series.h"
#include <antares/study/area/capacityReservation.h>
#include <antares/inifile/inifile.h>
#include <antares/logs/logs.h>
#include <antares/exception/LoadingError.hpp>
#include "antares/study/study.h"



using namespace Yuni;

#define SEP IO::Separator

namespace Antares::Data
{

static void resizeTSNoDataLoss(TimeSeries& TSToResize, uint width)
{
    auto& ts = TSToResize.timeSeries;
    ts.resizeWithoutDataLost(width, ts.height);
    for (uint x = 1; x < width; ++x)
        ts.pasteToColumn(x, ts[0]);
}

static uint EqualizeTSsize(TimeSeries& TScollection1,
                           TimeSeries& TScollection2,
                           const std::string& fatalErrorMsg,
                           Area& area,
                           unsigned int height1 = HOURS_PER_YEAR,
                           unsigned int height2 = HOURS_PER_YEAR)
{
    const auto ts1Width = TScollection1.timeSeries.width;
    const auto ts2Width = TScollection2.timeSeries.width;
    const auto maxWidth = std::max(ts1Width, ts2Width);

    if (ts1Width == 0 && ts2Width == 0)
    {
        TScollection1.reset(1, height1);
        TScollection2.reset(1, height2);
        return 1;
    }

    if (ts1Width == ts2Width)
        return maxWidth;

    if (ts1Width > 1 && ts2Width > 1)
    {
        logs.fatal() << fatalErrorMsg;
        return 0;
    }

    // At this point, one TS collection size is > 1 and the other is of size 1.

    // This following instruction to force reloading all area's TS when saving the study (GUI)
    area.invalidateJIT = true;

    if (ts1Width == 1)
        resizeTSNoDataLoss(TScollection1, maxWidth);
    if (ts2Width == 1)
        resizeTSNoDataLoss(TScollection2, maxWidth);

    return maxWidth;
}

static bool loadTSfromFile(Matrix<double>& ts,
                           const AreaName& areaID,
                           const AnyString& folder,
                           const std::string& filename,
                           unsigned int height)
{
    YString filePath;
    Matrix<>::BufferType fileContent;
    filePath.clear() << folder << SEP << areaID << SEP << filename;
    return ts.loadFromCSVFile(filePath, 1, height, &fileContent);
}

static void ConvertDailyTSintoHourlyTS(const Matrix<double>::ColumnType& dailyColumn,
                                       Matrix<double>::ColumnType& hourlyColumn)
{
    uint hour = 0;
    uint day = 0;

    while (hour < HOURS_PER_YEAR && day < DAYS_PER_YEAR)
    {
        for (uint i = 0; i < HOURS_PER_DAY; ++i)
        {
            hourlyColumn[hour] = dailyColumn[day];
            ++hour;
        }
        ++day;
    }
}

DataSeriesHydro::DataSeriesHydro() :
 ror(timeseriesNumbers),
 storage(timeseriesNumbers),
 mingen(timeseriesNumbers),
 maxHourlyGenPower(timeseriesNumbersHydroMaxPower),
 maxHourlyPumpPower(timeseriesNumbersHydroMaxPower)
{
    // Pmin was introduced in v8.6
    // The previous behavior was Pmin=0
    // For compatibility reasons with existing studies, mingen, maxHourlyGenPower and
    // maxHourlyPumpPower are set to one column of zeros by default
    mingen.reset();
    maxHourlyGenPower.reset();
    maxHourlyPumpPower.reset();
}

void DataSeriesHydro::copyGenerationTS(const DataSeriesHydro& source)
{
    ror.timeSeries = source.ror.timeSeries;
    storage.timeSeries = source.storage.timeSeries;
    mingen.timeSeries = source.mingen.timeSeries;

    generationTScount_ = source.generationTScount_;

    source.ror.unloadFromMemory();
    source.storage.unloadFromMemory();
    source.mingen.unloadFromMemory();
}

void DataSeriesHydro::copyMaxPowerTS(const DataSeriesHydro& source)
{
    maxHourlyGenPower.timeSeries = source.maxHourlyGenPower.timeSeries;
    maxHourlyPumpPower.timeSeries = source.maxHourlyPumpPower.timeSeries;

    maxPowerTScount_ = source.maxPowerTScount_;

    source.maxHourlyGenPower.unloadFromMemory();
    source.maxHourlyPumpPower.unloadFromMemory();
}

void DataSeriesHydro::reset()
{
    resizeGenerationTS(1);
    resizeMaxPowerTS(1);
}

void DataSeriesHydro::resizeGenerationTS(uint nbSeries)
{
    storage.reset(nbSeries, DAYS_PER_YEAR);
    ror.reset(nbSeries, HOURS_PER_YEAR);
    mingen.reset(nbSeries, HOURS_PER_YEAR);

    generationTScount_ = nbSeries;
}

void DataSeriesHydro::resizeMaxPowerTS(uint nbSeries)
{
    maxHourlyGenPower.reset(nbSeries, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(nbSeries, HOURS_PER_YEAR);

    maxPowerTScount_ = nbSeries;
}

bool DataSeriesHydro::forceReload(bool reload) const
{
    bool ret = true;
    ret = ror.forceReload(reload) && ret;
    ret = storage.forceReload(reload) && ret;
    ret = mingen.forceReload(reload) && ret;
    ret = maxHourlyGenPower.forceReload(reload) && ret;
    ret = maxHourlyPumpPower.forceReload(reload) && ret;
    return ret;
}

void DataSeriesHydro::markAsModified() const
{
    ror.markAsModified();
    storage.markAsModified();
    mingen.markAsModified();
    maxHourlyGenPower.markAsModified();
    maxHourlyPumpPower.markAsModified();
}

void DataSeriesHydro::EqualizeGenerationTSsizes(Area& area, bool usedByTheSolver)
{
    if (!usedByTheSolver) // From GUI, no need to equalize TS collections sizes
        return;

    // Equalize ROR and INFLOWS time series sizes
    // ------------------------------------------
    std::string fatalErrorMsg = "Hydro : area `" + area.id.to<std::string>() + "` : ";
    fatalErrorMsg += "ROR and INFLOWS must have the same number of time series.";

    generationTScount_
      = EqualizeTSsize(ror, storage, fatalErrorMsg, area, HOURS_PER_YEAR, DAYS_PER_YEAR);

    logs.info() << "  '" << area.id
                << "': ROR and INFLOWS time series were both set to : " << generationTScount_;

    // Equalize ROR and MINGEN time series sizes
    // -----------------------------------------
    fatalErrorMsg = "Hydro : area `" + area.id.to<std::string>() + "` : ";
    fatalErrorMsg += "ROR and MINGEN must have the same number of time series.";

    generationTScount_ = EqualizeTSsize(ror, mingen, fatalErrorMsg, area);

    logs.info() << "  '" << area.id
                << "': ROR and MINGEN time series were both set to : " << generationTScount_;
}

bool DataSeriesHydro::loadGenerationTS(const AreaName& areaID,
                                       const AnyString& folder,
                                       StudyVersion studyVersion)
{
    timeseriesNumbers.clear();

    bool ret = loadTSfromFile(ror.timeSeries, areaID, folder, "ror.txt", HOURS_PER_YEAR);
    ret = loadTSfromFile(storage.timeSeries, areaID, folder, "mod.txt", DAYS_PER_YEAR) && ret;
    if (studyVersion >= StudyVersion(8, 6))
        ret
          = loadTSfromFile(mingen.timeSeries, areaID, folder, "mingen.txt", HOURS_PER_YEAR) && ret;

    return ret;
}

bool DataSeriesHydro::LoadMaxPower(const AreaName& areaID, const AnyString& folder)
{
    bool ret = true;
    YString filepath;
    Matrix<>::BufferType fileContent;

    filepath.clear() << folder << SEP << areaID << SEP << "maxHourlyGenPower.txt";
    ret = maxHourlyGenPower.timeSeries.loadFromCSVFile(filepath, 1, HOURS_PER_YEAR, &fileContent)
          && ret;

    filepath.clear() << folder << SEP << areaID << SEP << "maxHourlyPumpPower.txt";
    ret = maxHourlyPumpPower.timeSeries.loadFromCSVFile(filepath, 1, HOURS_PER_YEAR, &fileContent)
          && ret;

    timeseriesNumbersHydroMaxPower.clear();

    return ret;
}

void DataSeriesHydro::buildHourlyMaxPowerFromDailyTS(
  const Matrix<double>::ColumnType& DailyMaxGenPower,
  const Matrix<double>::ColumnType& DailyMaxPumpPower)
{
    maxPowerTScount_ = 1;

    maxHourlyGenPower.reset(maxPowerTScount_, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(maxPowerTScount_, HOURS_PER_YEAR);

    ConvertDailyTSintoHourlyTS(DailyMaxGenPower, maxHourlyGenPower.timeSeries[0]);
    ConvertDailyTSintoHourlyTS(DailyMaxPumpPower, maxHourlyPumpPower.timeSeries[0]);
}

bool DataSeriesHydro::saveToFolder(const AreaName& areaID, const AnyString& folder) const
{
    String buffer;
    buffer.clear() << folder << SEP << areaID;
    /* Make sure the folder is created */
    if (IO::Directory::Create(buffer))
    {
        bool ret = true;

        // Saving data
        buffer.clear() << folder << SEP << areaID << SEP << "ror.txt";
        ret = ror.timeSeries.saveToCSVFile(buffer, 0) && ret;
        buffer.clear() << folder << SEP << areaID << SEP << "mod.txt";
        ret = storage.timeSeries.saveToCSVFile(buffer, 0) && ret;
        buffer.clear() << folder << SEP << areaID << SEP << "mingen.txt";
        ret = mingen.timeSeries.saveToCSVFile(buffer, 0) && ret;
        buffer.clear() << folder << SEP << areaID << SEP << "maxHourlyGenPower.txt";
        ret = maxHourlyGenPower.timeSeries.saveToCSVFile(buffer, 0) && ret;
        buffer.clear() << folder << SEP << areaID << SEP << "maxHourlyPumpPower.txt";
        ret = maxHourlyPumpPower.timeSeries.saveToCSVFile(buffer, 0) && ret;

        return ret;
    }
    return false;
}

uint64_t DataSeriesHydro::memoryUsage() const
{
    return sizeof(double) + ror.memoryUsage() + storage.memoryUsage() + mingen.memoryUsage()
           + maxHourlyGenPower.memoryUsage() + maxHourlyPumpPower.memoryUsage();
}

void DataSeriesHydro::EqualizeMaxPowerTSsizes(Area& area)
{
    std::string fatalErrorMsg = "Hydro Max Power: " + area.id.to<std::string>() + " : ";
    fatalErrorMsg += "generation and pumping must have the same number of TS.";

    maxPowerTScount_ = EqualizeTSsize(maxHourlyGenPower, maxHourlyPumpPower, fatalErrorMsg, area);

    logs.info() << "  '" << area.id << "': The number of hydro max power (generation and pumping) "
                << "TS were both set to : " << maxPowerTScount_;
}

void DataSeriesHydro::setHydroModulability(Area& area) const
{
    if (MatrixTestForAtLeastOnePositiveValue(maxHourlyGenPower.timeSeries))
    {
        area.hydro.hydroModulable = true;
    }
}

uint DataSeriesHydro::TScount() const
{
    return generationTScount_;
}

uint DataSeriesHydro::maxPowerTScount() const
{
    return maxPowerTScount_;
}

void DataSeriesHydro::resizeTSinDeratedMode(bool derated,
                                            StudyVersion studyVersion,
                                            bool usedBySolver)
{
    if (!(derated && usedBySolver))
        return;

    ror.averageTimeseries();
    storage.averageTimeseries();
    if (studyVersion >= StudyVersion(8, 6))
        mingen.averageTimeseries();
    generationTScount_ = 1;

    if (studyVersion >= StudyVersion(9, 1))
    {
        maxHourlyGenPower.averageTimeseries();
        maxHourlyPumpPower.averageTimeseries();
        maxPowerTScount_ = 1;
    }
}

} // namespace Antares::Data
