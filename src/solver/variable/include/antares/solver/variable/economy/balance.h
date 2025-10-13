/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#ifndef __SOLVER_VARIABLE_ECONOMY_BALANCE_H__
#define __SOLVER_VARIABLE_ECONOMY_BALANCE_H__

#include "antares/solver/variable/variable.h"
#include <antares/solver/variable/economy/economy_base.h>

namespace Antares::Solver::Variable::Economy
{
struct BalanceTraits: public UnitMWH
{
    static std::string Caption()
    {
        return "BALANCE";
    }

    static std::string Description()
    {
        return "Nodal energy balance, throughout all MC years";
    }

    typedef Results<R::AllYears::Average< // The average values throughout all years
      R::AllYears::StdDeviation<          // The standard deviation values throughout all years
        R::AllYears::Min<                 // The minimum values throughout all years
          R::AllYears::Max<               // The maximum values throughout all years
            >>>>>
      ResultsType;

    static constexpr uint8_t decimal = 0;

    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static bool checkCondition(const State& /*state*/)
    {
        return true;
    }

    static double value(const State& state)
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
        return bilanPays;
    }

    static void computeStats(IntermediateValues& values)
    {
        values.computeStatisticsForTheCurrentYear();
    }
};

typedef VCard_Base<BalanceTraits> VCardBalance;

/*!
** \brief Marginal Balance
*/
template<class NextT = Container::EndOfList>
class Balance: public Economy_Base<BalanceTraits, NextT>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardBalance VCardType;
    //! Ancestor
    typedef Variable::IVariable<Balance<NextT>, NextT, VCardType> AncestorType;

    //! List of expected results
    typedef typename VCardType::ResultsType ResultsType;

    typedef VariableAccessor<ResultsType, VCardType::columnCount> VariableAccessorType;

    enum
    {
        //! How many items have we got
        count = 1 + NextT::count,
    };

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        enum
        {
            count = ((VCardType::categoryDataLevel & CDataLevel
                      && VCardType::categoryFileLevel & CFile)
                       ? (NextType::template Statistics<CDataLevel, CFile>::count
                          + VCardType::columnCount * ResultsType::count)
                       : NextType::template Statistics<CDataLevel, CFile>::count),
        };
    };
};

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_BALANCE_H__
