// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <antares/logs/logs.h>

#include "economy_base.h"
#include "max-mrg-utils.h"

namespace Antares::Solver::Variable::Economy
{

struct MaxMargeCsrTraits
{
    static std::string Caption()
    {
        return "MAX MRG CSR";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Max margin for CSR";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    // This variable produces no per-hour value: it is filled weekly in
    // weekForEachArea. We still implement an explicit no-op setHourlyValue so
    // the economy_base contract is satisfied intentionally (rather than by a
    // silent fallback), and we log it once.
    template<class AuxiliaryData>
    static void setHourlyValue(IntermediateValues& /*values*/,
                               AuxiliaryData& /*auxiliaryData*/,
                               const State& /*state*/,
                               unsigned int /*numSpace*/)
    {
        [[maybe_unused]] static const bool logged = []
        {
            Antares::logs.info() << "Variable '" << Caption()
                                 << "' has no hourly value (computed weekly)";
            return true;
        }();
    }

    static void weekForEachArea(IntermediateValues& iv, State& state, unsigned int numSpace)
    {
        double* rawhourly = Memory::RawPointer(iv.hour);
        MaxMrgCSRdataFactory maxMRGcsrDataFactory(state, numSpace);
        MaxMRGinput maxMRGinput = maxMRGcsrDataFactory.data();
        computeMaxMRG(rawhourly + state.hourInTheYear, maxMRGinput);
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using MaxMrgCsr = EconomyVariableBase<MaxMargeCsrTraits>;

} // namespace Antares::Solver::Variable::Economy
