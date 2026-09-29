// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/study/parts/hydro/hydrorulecurves.h>
#include <antares/array/matrix-io.h>

namespace fs = std::filesystem;

namespace Antares::Data
{
class RuleCurvesLoader
{
public:
    RuleCurvesLoader(const std::filesystem::path& baseFolder,
                     const std::string& areaID,
                     TimeSeries& max,
                     TimeSeries& avg,
                     TimeSeries& min):
        baseFolder_(baseFolder),
        areaID_(areaID),
        max_(max),
        avg_(avg),
        min_(min)
    {
    }

    virtual ~RuleCurvesLoader() = default;
    virtual bool load() = 0;

protected:
    const std::filesystem::path& baseFolder_;
    const std::string& areaID_;
    TimeSeries& max_;
    TimeSeries& avg_;
    TimeSeries& min_;
};

class StandardRuleCurvesLoader: public RuleCurvesLoader
{
public:
    using RuleCurvesLoader::RuleCurvesLoader;

private:
    bool load() final
    {
        const fs::path filePath = baseFolder_ / "common" / "capacity"
                                  / std::string("reservoir_" + areaID_ + ".txt");

        Matrix<double> standardRuleCurves;
        standardRuleCurves.reset(3L, DAYS_PER_YEAR);

        bool ret = MatrixIO::load(standardRuleCurves, filePath.string(),
                                                      3,
                                                      DAYS_PER_YEAR,
                                                       Matrix<>::optFixedSize);

        min_.timeSeries.reset(1U, DAYS_PER_YEAR);
        min_.timeSeries.pasteToColumn(0, standardRuleCurves[RuleCurves::minimum]);
        avg_.timeSeries.reset(1U, DAYS_PER_YEAR);
        avg_.timeSeries.pasteToColumn(0, standardRuleCurves[RuleCurves::average]);
        max_.timeSeries.reset(1U, DAYS_PER_YEAR);
        max_.timeSeries.pasteToColumn(0, standardRuleCurves[RuleCurves::maximum]);

        return ret;
    }
};

class ScenarizedRuleCurvesLoader: public RuleCurvesLoader
{
public:
    using RuleCurvesLoader::RuleCurvesLoader;

private:
    bool load() final
    {
        const fs::path path = baseFolder_ / "series" / areaID_;
        bool ret = true;

        fs::path filePath = path / "maxDailyReservoirLevels.txt";
        ret = MatrixIO::load(max_.timeSeries, filePath.string(), 1, DAYS_PER_YEAR, 0)
              && ret;
        filePath = path / "minDailyReservoirLevels.txt";
        ret = MatrixIO::load(min_.timeSeries, filePath.string(), 1, DAYS_PER_YEAR, 0)
              && ret;
        filePath = path / "avgDailyReservoirLevels.txt";
        ret = MatrixIO::load(avg_.timeSeries, filePath.string(), 1, DAYS_PER_YEAR, 0)
              && ret;

        return ret;
    }
};

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

std::unique_ptr<RuleCurvesLoader> RuleCurvesLoaderService::createRuleCurvesLoader(
  Parameters::Compatibility::HydroRuleCurves hydroRuleCurves,
  const fs::path& filePath,
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
  const fs::path& folder,
  Parameters::Compatibility::HydroRuleCurves hydroRuleCurves)
{
    auto loader = createRuleCurvesLoader(hydroRuleCurves, folder, areaID);
    return loader->load();
}
} // namespace Antares::Data
