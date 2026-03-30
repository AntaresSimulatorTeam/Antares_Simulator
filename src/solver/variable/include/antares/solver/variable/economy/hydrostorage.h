// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "standard_variable.h"

namespace Antares::Solver::Variable::Economy
{

struct HydroStorageTraits
{
    static constexpr std::string_view kCaption = "H. STOR";
    static constexpr std::string_view kUnit = "MWh";
    static constexpr std::string_view kDescription = "Hydro Storage Generation";
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
        return state.hourlyResults->TurbinageHoraire[state.hourInTheWeek];
    }
};

using VCardHydroStorage = VCardStandardVariable<HydroStorageTraits>;

template<class NextT = Container::EndOfList>
class HydroStorage: public StandardVariableBase<HydroStorage<NextT>, NextT, VCardHydroStorage>
{
};

} // namespace Antares::Solver::Variable::Economy
