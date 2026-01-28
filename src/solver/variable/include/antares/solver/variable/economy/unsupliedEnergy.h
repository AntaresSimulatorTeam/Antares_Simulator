// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "antares/solver/variable/categories.h"
#include "antares/solver/variable/state.h"
#include "antares/solver/variable/storage/intermediate.h"
#include "antares/solver/variable/storage/results.h"

namespace Antares::Solver::Variable::Economy
{

struct UnsuppliedEnergyTrait
{
    static std::string Caption()
    {
        return "UNSP. ENRG";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Unsuplied Energy (demand that cannot be satisfied)";
    }

    typedef Results<R::AllYears::Average< // The average values throughout all years
      R::AllYears::StdDeviation<          // The standard deviation values throughout all years
        R::AllYears::Min<                 // The minimum values throughout all years
          R::AllYears::Max<               // The maximum values throughout all years
            >>>>>
      ResultsType;
    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static void yearBeginLogic(unsigned, IntermediateValues& iv, Data::Area*, unsigned)
    {
        iv.reset();
    }

    static void setValue(const State& state, IntermediateValues& iv, unsigned)
    {
        iv[state.hourInTheYear] = state.hourlyResults
                                    ->ValeursHorairesDeDefaillancePositive[state.hourInTheWeek];
    }

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }

    static bool checkCondition(const State&)
    {
        return true;
    }

    static double value(const State& state)
    {
        return state.hourlyResults->ValeursHorairesDeDefaillancePositive[state.hourInTheWeek];
    }
};

struct UnsuppliedEnergyCSRTrait: UnsuppliedEnergyTrait
{
    static std::string Caption()
    {
        return "UNSP. ENRG CSR";
    }

    static std::string Description()
    {
        return "Unsuplied Energy after CSR (demand that cannot be satisfied)";
    }

    static void setValue(const State& state, IntermediateValues& iv, unsigned)
    {
        iv[state.hourInTheYear] = state.hourlyResults
                                    ->ValeursHorairesDeDefaillancePositiveCSR[state.hourInTheWeek];
    }

    static double value(const State& state)
    {
        return state.hourlyResults->ValeursHorairesDeDefaillancePositiveCSR[state.hourInTheWeek];
    }
};

template<class NextT = Container::EndOfList>
using UnsupliedEnergy = Economy_Base<UnsuppliedEnergyTrait, NextT>;

template<class NextT = Container::EndOfList>
using UnsupliedEnergyCSR = Economy_Base<UnsuppliedEnergyCSRTrait, NextT>;

} // namespace Antares::Solver::Variable::Economy
