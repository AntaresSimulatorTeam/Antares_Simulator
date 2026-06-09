// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ECONOMY_WaterValue_H__
#define __SOLVER_VARIABLE_ECONOMY_WaterValue_H__

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{
struct WaterValueTraits
{
    static std::string Caption()
    {
        return "H. VAL";
    }

    static std::string Unit()
    {
        return "Euro/MWh";
    }

    static std::string Description()
    {
        return "Water value";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 2;
    static constexpr uint8_t spatialAggregate = Category::noSpatialAggregate;
    static constexpr uint8_t isPossiblyNonApplicable = 1;

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeAveragesForCurrentYearFromHourlyResults();
    }

    template<class Aux>
    static void setHourlyValue(IntermediateValues& iv, Aux&, const State& state, unsigned int)
    {
        iv.hour[state.hourInTheYear] = state.hourlyResults->valeurH2oHoraire[state.hourInTheWeek];
    }
};

/*!
** \brief Water values
*/
using WaterValue = EconomyVariableBase<WaterValueTraits>;

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_WaterValue_H__
