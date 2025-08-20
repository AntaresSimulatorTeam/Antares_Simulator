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

/*
** Generic generation (solar / wind) variables (deduplicated)
*/
#pragma once

#include <algorithm>
#include <string_view>

#include <antares/study/area/area.h>

#include "timeseries_base.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{

// Tags for different generation types
struct SolarTag
{
};

struct WindTag
{
};

// Specialized traits for each generation type
struct SolarTraits
{
    inline static constexpr std::string_view kCaption = "SOLAR";
    inline static constexpr std::string_view kDescription
      = "Solar generation, thoughout all MC years"; // keep original typo for stability
    static constexpr auto areaMember = &Data::Area::solar;
};

struct WindTraits
{
    inline static constexpr std::string_view kCaption = "WIND";
    inline static constexpr std::string_view kDescription
      = "Wind generation, thoughout all MC years"; // keep original typo for stability
    static constexpr auto areaMember = &Data::Area::wind;
};

// VCard specializations
using VCardTimeSeriesValuesSolar = VCardTimeSeriesBase<SolarTraits>;
using VCardTimeSeriesValuesWind = VCardTimeSeriesBase<WindTraits>;

// Legacy compatibility - keep old VCard names
template<class Tag>
struct GenerationTraits; // primary

template<>
struct GenerationTraits<SolarTag>: public SolarTraits
{
};

template<>
struct GenerationTraits<WindTag>: public WindTraits
{
};

template<class Tag>
using VCardTimeSeriesValuesGeneration = VCardTimeSeriesBase<GenerationTraits<Tag>>;

// Implementation classes
template<class TraitsType, class NextT = Container::EndOfList>
class TimeSeriesValuesGenerationImpl
    : public TimeSeriesValuesBase<TimeSeriesValuesGenerationImpl<TraitsType, NextT>,
                                  NextT,
                                  VCardTimeSeriesBase<TraitsType>>
{
public:
    using BaseType = TimeSeriesValuesBase<TimeSeriesValuesGenerationImpl<TraitsType, NextT>,
                                          NextT,
                                          VCardTimeSeriesBase<TraitsType>>;
    using VCardType = VCardTimeSeriesBase<TraitsType>;

    void initializeDerivedFromStudy(Data::Study& study)
    {
        isRenewableGenerationAggregated = study.parameters.renewableGeneration.isAggregated();
    }

    void yearBeginImpl(unsigned int year, unsigned int space)
    {
        if (isRenewableGenerationAggregated)
        {
            auto& holder = (BaseType::areaPtr->*TraitsType::areaMember);
            std::copy_n(holder.series.getColumn(year),
                        holder.series.timeSeries.height,
                        BaseType::yearlyValues[space].hour);
        }
    }

    void hourForEachAreaImpl(State& state, unsigned int space)
    {
        // Default implementation - no specific action needed for generation
        // Values are already copied in yearBeginImpl
    }

private:
    bool isRenewableGenerationAggregated{true};
};

// Type aliases for backward compatibility
template<class Tag, class NextT = Container::EndOfList>
using TimeSeriesValuesGeneration = TimeSeriesValuesGenerationImpl<GenerationTraits<Tag>, NextT>;

template<class NextT = Container::EndOfList>
using TimeSeriesValuesSolar = TimeSeriesValuesGenerationImpl<SolarTraits, NextT>;

template<class NextT = Container::EndOfList>
using TimeSeriesValuesWind = TimeSeriesValuesGenerationImpl<WindTraits, NextT>;

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares
