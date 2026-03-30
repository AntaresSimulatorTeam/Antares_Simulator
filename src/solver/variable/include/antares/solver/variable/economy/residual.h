// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "standard_variable.h"

namespace Antares::Solver::Variable::Economy
{

struct ResidualLoadTraits
{
    static constexpr std::string_view kCaption = "RES LOAD";
    static constexpr std::string_view kUnit = "MWh";
    static constexpr std::string_view kDescription = "Residual load";
    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t isPossiblyNonApplicable = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

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

    static double getValue(const State& state, unsigned int)
    {
        return state.problemeHebdo->ConsommationsAbattues[state.hourInTheWeek]
          .ConsommationAbattueDuPays[state.area->index];
    }
};

using VCardResidualLoad = VCardStandardVariable<ResidualLoadTraits>;

template<class NextT = Container::EndOfList>
class ResidualLoad: public StandardVariableBase<ResidualLoad<NextT>, NextT, VCardResidualLoad>
{
};

} // namespace Antares::Solver::Variable::Economy
