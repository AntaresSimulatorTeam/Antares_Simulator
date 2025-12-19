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

class RuleCurvesLoader
{
public:
    RuleCurvesLoader(

      const std::filesystem::path& baseFolder,
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
    StandardRuleCurvesLoader(const std::filesystem::path& baseFolder,
                             const std::string& areaID,
                             TimeSeries& max,
                             TimeSeries& avg,
                             TimeSeries& min):
        RuleCurvesLoader(baseFolder, areaID, max, avg, min)

    {
    }

private:
    bool load() final;
};

class ScenarizedRuleCurvesLoader: public RuleCurvesLoader
{
public:
    ScenarizedRuleCurvesLoader(const std::filesystem::path& baseFolder,
                               const std::string& areaID,
                               TimeSeries& max,
                               TimeSeries& avg,
                               TimeSeries& min):
        RuleCurvesLoader(baseFolder, areaID, max, avg, min)
    {
    }

private:
    bool load() final;
};

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
