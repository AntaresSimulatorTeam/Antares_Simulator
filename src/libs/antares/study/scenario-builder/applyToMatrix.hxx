//
// Created by marechaljas on 03/07/23.
//

#pragma once

#include <cstdlib>
#include "antares/study/parts/hydro/series.h"
#include "yuni/core/system/stdint.h"

namespace Antares::Data::ScenarioBuilder {

static constexpr unsigned maxErrors = 20;

template<class D>
static inline bool CheckValidity(uint value, const D& data, uint tsGenMax)
{
    // When the TS-Generators are not used
    return (!tsGenMax) ? (value < data.timeSeries.width) : (value < tsGenMax);
}

template<>
inline bool CheckValidity<Data::DataSeriesHydro>(uint value,
                                                 const Data::DataSeriesHydro& data,
                                                 uint tsGenMax)
{
    // When the TS-Generators are not used
    return (!tsGenMax) ? (value < data.count) : (value < tsGenMax);
}

template<>
inline bool CheckValidity<Data::AreaLink>(uint value,
                                          const Data::AreaLink& data,
                                          uint /* tsGenMax */)
{
    //Value = index of time series
    //Direct Capacities = all time series
    //directCapacities.width = Number of time series
    return value < data.directCapacities.width;
}

template<>
inline bool CheckValidity<BindingConstraintGroup>(uint, const BindingConstraintGroup&, uint)
{
    //TS-Generator never used
    //Should check for time-series width, but we are missing information at this point
    return true;
}

template<class StringT, class D>
bool ApplyToMatrix(uint& errors,
                   StringT& logprefix,
                   D& data,
                   const TSNumberData::MatrixType::ColumnType& years,
                   uint tsGenMax)
{
    bool ret = true;

    // In this case, m.height represents the total number of years
    const uint nbYears = data.timeseriesNumbers.height;
    // The matrix m has only one column
    assert(data.timeseriesNumbers.width == 1);
    typename Matrix<uint32>::ColumnType& target = data.timeseriesNumbers[0];

    for (uint y = 0; y != nbYears; ++y)
    {
        if (years[y] != 0)
        {
            // The new TS number
            uint tsNum = years[y] - 1;

            // When the TS-Generators are not used
            if (!CheckValidity(tsNum, data, tsGenMax))
            {
                if (errors <= maxErrors)
                {
                    if (++errors == maxErrors)
                        logs.warning() << "scenario-builder: ... (skipped)";
                    else
                        logs.warning() << "scenario-builder: " << logprefix
                                       << "value out of bounds for the year " << (y + 1);
                }
                ret = false;
                continue;
            }
            // Ok, assign. The value provided by the interface is user-friendly
            // and starts from 1.
            target[y] = tsNum;
        }
    }

    return ret;
}
}
