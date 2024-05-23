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
//
// Created by marechaljas on 03/07/23.
//

#pragma once

#include <cstdlib>

#include "yuni/core/system/stdint.h"

#include "antares/study/binding_constraint/BindingConstraintGroup.h"
#include "antares/study/parts/hydro/series.h"
#include "antares/study/scenario-builder/TSnumberData.h"

namespace Antares::Data::ScenarioBuilder
{

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
    return (!tsGenMax) ? (value < data.TScount()) : (value < tsGenMax);
}

template<>
inline bool CheckValidity<Data::AreaLink>(uint value,
                                          const Data::AreaLink& data,
                                          uint /* tsGenMax */)
{
    // Value = index of time series
    // Direct Capacities = all time series
    // directCapacities.timeSeries.width = Number of time series
    return value < data.directCapacities.timeSeries.width;
}

template<>
inline bool CheckValidity<BindingConstraintGroup>(uint value,
                                                  const BindingConstraintGroup& group,
                                                  uint)
{
    return value < group.numberOfTimeseries();
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
    const uint nbYears = data.timeseriesNumbers.height();
    // The matrix m has only one column
    auto& target = data.timeseriesNumbers;

    for (uint y = 0; y != nbYears; ++y)
    {
        if (years[y] != 0)
        {
            // The new TS number
            uint32_t tsNum = years[y] - 1;

            // When the TS-Generators are not used
            if (!CheckValidity(tsNum, data, tsGenMax))
            {
                if (errors <= maxErrors)
                {
                    if (++errors == maxErrors)
                    {
                        logs.warning() << "scenario-builder: ... (skipped)";
                    }
                    else
                    {
                        logs.warning() << "scenario-builder: " << logprefix
                                       << "value out of bounds for the year " << (y + 1);
                    }
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
} // namespace Antares::Data::ScenarioBuilder
