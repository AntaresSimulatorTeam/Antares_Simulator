// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct BalanceTraits
{
    static std::string Caption()
    {
        return "BALANCE";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Nodal energy balance, throughout all MC years";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static void setHourlyValue(IntermediateValues& intermediateValues,
                               typename detail::AuxiliaryDataType<BalanceTraits>::type&,
                               const State& state,
                               unsigned int)
    {
        double bilanPays = 0.;
        int Interco = state.problemeHebdo->IndexDebutIntercoOrigine[state.area->index];
        while (Interco >= 0)
        {
            bilanPays += state.ntc.ValeurDuFlux[static_cast<std::size_t>(Interco)];
            Interco = state.problemeHebdo
                        ->IndexSuivantIntercoOrigine[static_cast<std::size_t>(Interco)];
        }
        Interco = state.problemeHebdo->IndexDebutIntercoExtremite[state.area->index];
        while (Interco >= 0)
        {
            bilanPays -= state.ntc.ValeurDuFlux[static_cast<std::size_t>(Interco)];
            Interco = state.problemeHebdo
                        ->IndexSuivantIntercoExtremite[static_cast<std::size_t>(Interco)];
        }
        intermediateValues[state.hourInTheYear] = bilanPays;
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardBalance = EconomyVariableCard<BalanceTraits>;

using Balance = EconomyVariableBase<BalanceTraits>;

} // namespace Antares::Solver::Variable::Economy
