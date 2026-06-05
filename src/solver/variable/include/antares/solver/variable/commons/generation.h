// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

// Renewable generation time series variables (solar and wind), built on the
// time series base framework in timeseries_base.h. When renewable aggregation
// is enabled, the whole year's series is bulk-copied in yearBegin; there is no
// per-hour processing.

#include <algorithm>
#include <string_view>

#include <antares/study/area/area.h>

#include "timeseries_base.h"

namespace Antares::Solver::Variable::Economy
{

// Per-generation-type traits: output caption/description and the Area member
// holding the time series.
struct SolarTraits
{
    inline static constexpr std::string_view kCaption = "SOLAR";
    // NOTE: keep the "thoughout" spelling, it is part of the published output caption.
    inline static constexpr std::string_view kDescription = "Solar generation, thoughout all MC "
                                                            "years";
    static constexpr auto areaMember = &Data::Area::solar;
};

struct WindTraits
{
    inline static constexpr std::string_view kCaption = "WIND";
    // NOTE: keep the "thoughout" spelling, it is part of the published output caption.
    inline static constexpr std::string_view kDescription = "Wind generation, thoughout all MC "
                                                            "years";
    static constexpr auto areaMember = &Data::Area::wind;
};

using VCardTimeSeriesValuesSolar = VCardTimeSeriesBase<SolarTraits>;
using VCardTimeSeriesValuesWind = VCardTimeSeriesBase<WindTraits>;

// Shared implementation for solar and wind generation time series.
template<class TraitsType>
class TimeSeriesValuesGenerationImpl
    : public TimeSeriesValuesBase<TimeSeriesValuesGenerationImpl<TraitsType>,
                                  VCardTimeSeriesBase<TraitsType>>
{
public:
    using BaseType = TimeSeriesValuesBase<TimeSeriesValuesGenerationImpl<TraitsType>,
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

    // Nothing to do per hour: the whole year is bulk-copied in yearBeginImpl.
    void hourForEachAreaImpl(State&, unsigned int)
    {
    }

private:
    bool isRenewableGenerationAggregated{true};
};

using TimeSeriesValuesSolar = TimeSeriesValuesGenerationImpl<SolarTraits>;
using TimeSeriesValuesWind = TimeSeriesValuesGenerationImpl<WindTraits>;

} // namespace Antares::Solver::Variable::Economy
