// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "standard_variable.h"

namespace Antares::Solver::Variable::Economy
{

struct PriceTraits
{
    static constexpr std::string_view kCaption = "MRG. PRICE";
    static constexpr std::string_view kUnit = "Euro";
    static constexpr std::string_view kDescription = "Marginal Price, throughout all MC years";
    static constexpr uint8_t decimal = 4;
    static constexpr uint8_t isPossiblyNonApplicable = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateAverage;
    static constexpr uint8_t spatialAggregatePostProcessing = Category::
      spatialAggregatePostProcessingPrice;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;

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
class Price: public StandardVariableBase<Price<NextT>, NextT, VCardStandardVariable<PriceTraits>>
{
public:
    using BaseType = StandardVariableBase<Price<NextT>, NextT, VCardStandardVariable<PriceTraits>>;
    using NextType = NextT;

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        this->pValuesForTheCurrentYear[numSpace].computeAveragesForCurrentYearFromHourlyResults();
        NextT::yearEnd(year, numSpace);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        this->pValuesForTheCurrentYear[numSpace][state.hourInTheYear] -= state.hourlyResults
                                                                           ->CoutsMarginauxHoraires
                                                                             [state.hourInTheWeek];
        NextT::hourForEachArea(state, numSpace);
    }
};

} // namespace Antares::Solver::Variable::Economy
