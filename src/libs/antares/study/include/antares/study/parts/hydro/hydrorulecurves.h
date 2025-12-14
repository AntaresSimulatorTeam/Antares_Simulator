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
#ifndef __ANTARES_LIBS_STUDY_PARTS_HYDRO_RESERVOIR_LEVELS_H__
#define __ANTARES_LIBS_STUDY_PARTS_HYDRO_RESERVOIR_LEVELS_H__

#include <antares/array/matrix.h>
#include <antares/series/series.h>
#include <antares/study/parameters.h>
#include <antares/study/version.h>

#include "../../fwd.h"

namespace Antares
{
namespace Data
{

/*!
** \brief Rule Curves class that support reservoir levels data
*/

class RuleCurves
{
public:
    enum
    {
        //! The minimum value
        minimum = 0,
        //! The average value
        average,
        //! The maximum value
        maximum,
    };

    //! \name Constructor
    //@{
    /*!
    ** \brief Default constructor
    */
    explicit RuleCurves(TimeSeriesNumbers& timeseriesNumbers);

    void averageTimeSeries();

    TimeSeriesNumbers& timeseriesNumbers;

    /*!
     ** \brief Maximum Reservoir Levels (%)
     **
     ** Merely a matrix of TimeSeriesCount * DAYS_PER_YEAR values
     */
    TimeSeries max;

    /*!
    ** \brief Minimum Reservoir Levels (%)
    **
    ** Merely a matrix of TimeSeriesCount * DAYS_PER_YEAR values
    */
    TimeSeries min;

    /*!
    ** \brief Average Reservoir Levels (%)
    **
    ** Merely a matrix of TimeSeriesCount * DAYS_PER_YEAR values
    */
    TimeSeries avg;

    /*!
    ** \brief This matrix is used for UI communication
    **
    ** Merely a matrix of 3 * DAYS_PER_YEAR values
    */
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
} // namespace Data
} // namespace Antares

#endif /* __ANTARES_LIBS_STUDY_PARTS_HYDRO_RESERVOIR_LEVELS_H__ */
