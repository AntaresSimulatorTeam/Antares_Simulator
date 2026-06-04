// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "multi_column_base.h"

namespace Antares::Solver::Variable::Economy
{

struct ThermalAirPollutantEmissionsTraits
{
    static std::string Caption()
    {
        return "";
    }

    static std::string Unit()
    {
        return "Tons";
    }

    static std::string Description()
    {
        return "Overall pollutant emissions expected from all the thermal clusters";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 0;
    static constexpr int columnCount = Antares::Data::Pollutant::POLLUTANT_MAX;

    static std::string columnCaption(unsigned int indx)
    {
        if (indx < Antares::Data::Pollutant::POLLUTANT_MAX)
        {
            return Antares::Data::Pollutant::getPollutantName(indx).c_str();
        }
        return "<unknown>";
    }

    template<int ColCount>
    static void setHourlyValue(IntermediateValues (&values)[ColCount],
                               State& state,
                               [[maybe_unused]] unsigned int numSpace)
    {
        auto& area = state.area;
        auto& thermal = state.thermal;
        for (auto& cluster: area->thermal.list.each_enabled())
        {
            for (int pollutant = 0; pollutant < Antares::Data::Pollutant::POLLUTANT_MAX;
                 pollutant++)
            {
                values[pollutant][state.hourInTheYear] += cluster->emissions.factors[pollutant]
                                                          * thermal[state.area->index]
                                                              .thermalClustersProductions
                                                                [cluster->enabledIndex];
            }
        }
    }
};

using ThermalAirPollutantEmissions = MultiColumnBase<
  ThermalAirPollutantEmissionsTraits,
  ThermalAirPollutantEmissionsTraits::columnCount>;

} // namespace Antares::Solver::Variable::Economy
