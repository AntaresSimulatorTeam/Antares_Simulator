// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/solver/variable/economy/standard_variable.h>

namespace Antares::Solver::Variable::Adequacy
{

template<typename Derived>
struct AdequacyVariableTraits: public Economy::StandardVariableTraits<Derived>
{
    static void yearEndBuildForEachThermalClusterHook(State&, unsigned int, unsigned int)
    {
    }

    static void yearEndBuildHook(State&, unsigned int, unsigned int)
    {
    }
};

template<typename Derived, typename NextT, typename VCardType>
class AdequacyVariableBase: public Economy::StandardVariableBase<Derived, NextT, VCardType>
{
public:
    using BaseType = Economy::StandardVariableBase<Derived, NextT, VCardType>;
    using NextType = NextT;
    using BaseType::pValuesForTheCurrentYear;

    void yearEndBuildForEachThermalCluster(State& state, unsigned int year, unsigned int numSpace)
    {
        VCardType::yearEndBuildForEachThermalClusterHook(state, year, numSpace);
        NextType::yearEndBuildForEachThermalCluster(state, year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year, unsigned int numSpace)
    {
        VCardType::yearEndBuildHook(state, year, numSpace);
        NextType::yearEndBuild(state, year, numSpace);
    }
};

} // namespace Antares::Solver::Variable::Adequacy
