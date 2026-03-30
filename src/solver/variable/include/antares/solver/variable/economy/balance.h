// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "standard_variable.h"

namespace Antares::Solver::Variable::Economy
{

struct BalanceTraits
{
    static constexpr std::string_view kCaption = "BALANCE";
    static constexpr std::string_view kUnit = "MWh";
    static constexpr std::string_view kDescription = "Nodal energy balance, throughout all MC "
                                                     "years";
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
};

using VCardBalance = VCardStandardVariable<BalanceTraits>;

template<class NextT = Container::EndOfList>
class Balance
    : public StandardVariableBase<Balance<NextT>, NextT, VCardStandardVariable<BalanceTraits>>
{
public:
    using BaseType = StandardVariableBase<Balance<NextT>,
                                          NextT,
                                          VCardStandardVariable<BalanceTraits>>;
    using NextType = NextT;

    void hourForEachArea(State& state, unsigned int numSpace)
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

        this->pValuesForTheCurrentYear[numSpace][state.hourInTheYear] = bilanPays;
        NextT::hourForEachArea(state, numSpace);
    }
};

} // namespace Antares::Solver::Variable::Economy
