// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/hydro/series.h"

#include <algorithm>

#include <antares/array/matrix-io.h>
#include <antares/inifile/inifile.h>
#include <antares/logs/logs.h>
#include <antares/study/parts/hydro/series.h>

namespace fs = std::filesystem;

namespace Antares::Data
{

static bool loadTSfromFile(Matrix<double>& ts,
                           const std::string& areaID,
                           const fs::path& folder,
                           const std::string& filename,
                           unsigned int height)
{
    fs::path filePath = folder / areaID / filename;
    return MatrixIO::load(ts, filePath.string(), 1, height, 0);
}

static void ConvertDailyTSintoHourlyTS(const Matrix<double>::ColumnType& dailyColumn,
                                       Matrix<double>::ColumnType& hourlyColumn)
{
    unsigned int hour = 0;
    unsigned int day = 0;

    while (hour < HOURS_PER_YEAR && day < DAYS_PER_YEAR)
    {
        for (unsigned int i = 0; i < HOURS_PER_DAY; ++i)
        {
            hourlyColumn[hour] = dailyColumn[day];
            ++hour;
        }
        ++day;
    }
}

static void ConvertHourlyTSintoDailyTS(const Matrix<double>::ColumnType& hourlyColumn,
                                       Matrix<double>::ColumnType& dailyColumn)
{
    for (unsigned hour = 0; hour < HOURS_PER_YEAR; ++hour)
    {
        unsigned day = hour / HOURS_PER_DAY;
        if (hour % HOURS_PER_DAY == 0)
        {
            dailyColumn[day] = 0.0; // start a new day sum
        }

        dailyColumn[day] += hourlyColumn[hour] / HOURS_PER_DAY;
    }
}

DataSeriesHydro::DataSeriesHydro():
    ror(timeseriesNumbers),
    storage(timeseriesNumbers),
    mingen(timeseriesNumbers),
    maxHourlyGenPower(timeseriesNumbers),
    maxHourlyPumpPower(timeseriesNumbers),
    ruleCurves(timeseriesNumbers)
{
    timeseriesNumbers.registerSeries(&ror, "ror");
    timeseriesNumbers.registerSeries(&storage, "storage");
    timeseriesNumbers.registerSeries(&mingen, "mingen");
    timeseriesNumbers.registerSeries(&maxHourlyGenPower, "max-geneneration-power");
    timeseriesNumbers.registerSeries(&maxHourlyPumpPower, "max-pumping-power");

    // Pmin was introduced in v8.6
    // The previous behavior was Pmin=0
    // For compatibility reasons with existing studies, mingen, maxHourlyGenPower and
    // maxHourlyPumpPower are set to one column of zeros by default
    mingen.reset();
    maxHourlyGenPower.reset();
    maxHourlyPumpPower.reset();
}

void DataSeriesHydro::reset()
{
    resizeTS(1);
}

void DataSeriesHydro::resizeTS(unsigned int nbSeries)
{
    storage.reset(nbSeries, DAYS_PER_YEAR);
    ror.reset(nbSeries, HOURS_PER_YEAR);
}

bool DataSeriesHydro::loadGenerationTS(const AreaName& areaID,
                                       const fs::path& folder,
                                       StudyVersion studyVersion)
{
    timeseriesNumbers.clear();

    bool ret = loadTSfromFile(ror.timeSeries, areaID, folder, "ror.txt", HOURS_PER_YEAR);
    ret = loadTSfromFile(storage.timeSeries, areaID, folder, "mod.txt", DAYS_PER_YEAR) && ret;
    if (studyVersion >= StudyVersion(8, 6))
    {
        ret = loadTSfromFile(mingen.timeSeries, areaID, folder, "mingen.txt", HOURS_PER_YEAR)
              && ret;
    }
    return ret;
}

bool DataSeriesHydro::LoadMaxPower(const std::string& areaID, const fs::path& folder)
{
    bool ret = true;
    fs::path filePath = folder / areaID / "maxHourlyGenPower.txt";
    ret = MatrixIO::load(maxHourlyGenPower.timeSeries, filePath.string(), 1, HOURS_PER_YEAR, 0)
          && ret;

    filePath = folder / areaID / "maxHourlyPumpPower.txt";
    ret = MatrixIO::load(maxHourlyPumpPower.timeSeries, filePath.string(), 1, HOURS_PER_YEAR, 0)
          && ret;

    return ret;
}

void DataSeriesHydro::buildHourlyMaxPowerFromDailyTS(
  const Matrix<double>::ColumnType& DailyMaxGenPower,
  const Matrix<double>::ColumnType& DailyMaxPumpPower)
{
    const unsigned int count = 1;

    maxHourlyGenPower.reset(count, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(count, HOURS_PER_YEAR);

    ConvertDailyTSintoHourlyTS(DailyMaxGenPower, maxHourlyGenPower.timeSeries[0]);
    ConvertDailyTSintoHourlyTS(DailyMaxPumpPower, maxHourlyPumpPower.timeSeries[0]);
}

Matrix<> DataSeriesHydro::getDailyMaxGenPowerFromHourlyTS()
{
    Matrix<> dailyTs(1, DAYS_PER_YEAR);
    ConvertHourlyTSintoDailyTS(maxHourlyGenPower.timeSeries[0], dailyTs[0]);
    return dailyTs;
}

Matrix<> DataSeriesHydro::getDailyMaxPumpPowerFromHourlyTS()
{
    Matrix<> dailyTs(1, DAYS_PER_YEAR);
    ConvertHourlyTSintoDailyTS(maxHourlyPumpPower.timeSeries[0], dailyTs[0]);
    return dailyTs;
}

bool DataSeriesHydro::saveToFolder(const AreaName& areaID,
                                   const std::string& folder,
                                   Parameters::Compatibility::HydroPmax hydroPmax) const
{
    const auto buffer = fs::path(folder) / areaID;
    /* Make sure the folder is created */
    std::error_code ec;
    const bool created = std::filesystem::create_directories(buffer, ec);
    std::error_code directoryEc;
    const bool isDirectory = std::filesystem::is_directory(buffer, directoryEc);
    if (created || isDirectory)
    {
        bool ret = true;

        // Saving data
        ret = MatrixIO::save(ror.timeSeries, (buffer / "ror.txt").string(), 0) && ret;
        ret = MatrixIO::save(storage.timeSeries, (buffer / "mod.txt").string(), 0) && ret;
        ret = MatrixIO::save(mingen.timeSeries, (buffer / "mingen.txt").string(), 0) && ret;

        if (hydroPmax == Parameters::Compatibility::HydroPmax::Hourly)
        {
            ret = MatrixIO::save(maxHourlyGenPower.timeSeries,
                                 (buffer / "maxHourlyGenPower.txt").string(),
                                 0)
                  && ret;
            ret = MatrixIO::save(maxHourlyPumpPower.timeSeries,
                                 (buffer / "maxHourlyPumpPower.txt").string(),
                                 0)
                  && ret;
        }

        return ret;
    }
    return false;
}

unsigned int DataSeriesHydro::TScount() const
{
    const std::vector<uint32_t> nbColumns({storage.numberOfColumns(),
                                           ror.numberOfColumns(),
                                           mingen.numberOfColumns(),
                                           maxHourlyGenPower.numberOfColumns(),
                                           maxHourlyPumpPower.numberOfColumns(),
                                           ruleCurves.max.numberOfColumns(),
                                           ruleCurves.min.numberOfColumns(),
                                           ruleCurves.avg.numberOfColumns()});

    return *std::max_element(nbColumns.begin(), nbColumns.end());
}

void DataSeriesHydro::resizeTSinDeratedMode(bool derated,
                                            StudyVersion studyVersion,
                                            Parameters::Compatibility::HydroPmax hydroPmax)
{
    if (!derated)
    {
        return;
    }

    ror.averageTimeseries();
    storage.averageTimeseries();
    if (studyVersion >= StudyVersion(8, 6))
    {
        mingen.averageTimeseries();

        if (hydroPmax == Parameters::Compatibility::HydroPmax::Hourly)
        {
            maxHourlyGenPower.averageTimeseries();
            maxHourlyPumpPower.averageTimeseries();
        }
    }

    ruleCurves.averageTimeSeries();
}
} // namespace Antares::Data
