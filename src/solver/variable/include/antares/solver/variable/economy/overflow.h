// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "standard_variable.h"

namespace Antares::Solver::Variable::Economy
{

struct OverflowTraits
{
    static constexpr std::string_view kCaption = "H. OVFL";
    static constexpr std::string_view kUnit = "%";
    static constexpr std::string_view kDescription = "Hydro overflow";
    static constexpr uint8_t decimal = 0;
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

    static double getValue(const State& state, unsigned int)
    {
        return state.hourlyResults->debordementsHoraires[state.hourInTheWeek];
    }
};

using VCardOverflows = VCardStandardVariable<OverflowTraits>;

template<class NextT = Container::EndOfList>
class Overflows: public StandardVariableBase<Overflows<NextT>, NextT, VCardOverflows>
{
};

} // namespace Antares::Solver::Variable::Economy
