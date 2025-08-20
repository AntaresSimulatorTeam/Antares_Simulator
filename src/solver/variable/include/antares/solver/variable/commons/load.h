/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#pragma once

#include <cstring>

#include "timeseries_base.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{

// Load-specific traits
struct LoadTraits
{
    inline static constexpr std::string_view kCaption = "LOAD";
    inline static constexpr std::string_view kDescription = "Load generation, thoughout all MC "
                                                            "years";
};

// VCard for load
using VCardTimeSeriesValuesLoad = VCardTimeSeriesBase<LoadTraits>;

// Load implementation
template<class NextT = Container::EndOfList>
class TimeSeriesValuesLoad
    : public TimeSeriesValuesBase<TimeSeriesValuesLoad<NextT>, NextT, VCardTimeSeriesValuesLoad>
{
public:
    using BaseType = TimeSeriesValuesBase<TimeSeriesValuesLoad<NextT>,
                                          NextT,
                                          VCardTimeSeriesValuesLoad>;

    void initializeDerivedFromStudy(Data::Study& study)
    {
        // No specific initialization needed for load
    }

    void yearBeginImpl(unsigned int year, unsigned int space)
    {
        // The current time-series
        //
        // At this point, these values are not the raw values of the load timeseries
        // see performTransformationsBeforeLaunchingSimulation()
        // L* = L + DSM
        //
        std::memcpy(BaseType::yearlyValues[space].hour,
                    BaseType::areaPtr->load.series.getColumn(year),
                    sizeof(double) * BaseType::areaPtr->load.series.timeSeries.height);
    }

    void hourForEachAreaImpl(State& state, unsigned int space)
    {
        // No specific action needed - values are already copied in yearBeginImpl
    }
};

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares
