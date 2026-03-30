// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "standard_variable.h"

namespace Antares::Solver::Variable::Economy
{

struct PumpingTraits
{
    static constexpr std::string_view kCaption = "H. PUMP";
    static constexpr std::string_view kUnit = "MWh";
    static constexpr std::string_view kDescription = "Hydraulic pumping";
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
        return state.hourlyResults->PompageHoraire[state.hourInTheWeek];
    }
};

using VCardPumping = VCardStandardVariable<PumpingTraits>;

template<class NextT = Container::EndOfList>
class Pumping: public StandardVariableBase<Pumping<NextT>, NextT, VCardPumping>
{
};

} // namespace Antares::Solver::Variable::Economy
