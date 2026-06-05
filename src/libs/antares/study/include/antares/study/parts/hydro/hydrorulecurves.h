// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <antares/series/series.h>
#include <antares/study/parameters.h>

namespace Antares::Data
{

class RuleCurves
{
public:
    enum
    {
        minimum = 0,
        average,
        maximum,
    };

    explicit RuleCurves(TimeSeriesNumbers& timeseriesNumbers);

    void averageTimeSeries();

    TimeSeriesNumbers& timeseriesNumbers;

    /*!
     ** \brief max, min, average Reservoir Levels (%)
     **
     ** Matrices of dimensions TimeSeriesCount * DAYS_PER_YEAR values
     */
    TimeSeries max;
    TimeSeries min;
    TimeSeries avg;
};

class RuleCurvesLoader;

class RuleCurvesLoaderService
{
private:
    RuleCurves& ruleCurves_;

public:
    explicit RuleCurvesLoaderService(RuleCurves& ruleCurves):
        ruleCurves_(ruleCurves)
    {
    }

    bool LoadFromFolder(const std::string& areaID,
                        const std::filesystem::path& folder,
                        Parameters::Compatibility::HydroRuleCurves hydroRuleCurves);

private:
    std::unique_ptr<RuleCurvesLoader> createRuleCurvesLoader(
      Parameters::Compatibility::HydroRuleCurves hydroRuleCurves,
      const std::filesystem::path& filePath,
      const std::string& areaID);
};
} // namespace Antares::Data
