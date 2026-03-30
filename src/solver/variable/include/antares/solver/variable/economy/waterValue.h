// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "standard_variable.h"

namespace Antares::Solver::Variable::Economy
{

struct WaterValueTraits
{
    static constexpr std::string_view kCaption = "H. VAL";
    static constexpr std::string_view kUnit = "Euro/MWh";
    static constexpr std::string_view kDescription = "Water value";
    static constexpr uint8_t decimal = 2;
    static constexpr uint8_t isPossiblyNonApplicable = 1;
    static constexpr uint8_t spatialAggregate = Category::noSpatialAggregate;

    static std::string Caption()
    {
        return std::string(kCaption);
    }

    static std::string Unit()
    {
        return std::string(kUnit);
    }

    static std::string Description()
    {
        return std::string(kDescription);
    }
};

template<class NextT = Container::EndOfList>
class WaterValue
    : public StandardVariableBase<WaterValue<NextT>, NextT, VCardStandardVariable<WaterValueTraits>>
{
public:
    using BaseType = StandardVariableBase<WaterValue<NextT>,
                                          NextT,
                                          VCardStandardVariable<WaterValueTraits>>;
    using NextType = NextT;

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        this->pValuesForTheCurrentYear[numSpace].computeAveragesForCurrentYearFromHourlyResults();
        NextT::yearEnd(year, numSpace);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        this->pValuesForTheCurrentYear[numSpace].hour[state.hourInTheYear]
          = state.hourlyResults->valeurH2oHoraire[state.hourInTheWeek];
        NextT::hourForEachArea(state, numSpace);
    }
};

} // namespace Antares::Solver::Variable::Economy
