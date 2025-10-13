/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __SOLVER_VARIABLE_ECONOMY_DtgMarginCsr_H__
#define __SOLVER_VARIABLE_ECONOMY_DtgMarginCsr_H__

#include <antares/solver/variable/economy/economy_base.h>
#include "antares/solver/variable/variable.h"

namespace Antares::Solver::Variable::Economy
{
struct DtgMarginCsrTraits: public UnitMWH
{
    static std::string Caption()
    {
        return "DTG MRG CSR";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Dispatchable Generation Margin (after CSR optimization)";
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
        return state.hourlyResults->ValeursHorairesDtgMrgCsr[state.hourInTheWeek];
    }

    static void computeStats(IntermediateValues& values)
    {
        values.computeStatisticsForTheCurrentYear();
    }
};

typedef VCard_Base<DtgMarginCsrTraits> VCardDtgMarginCsr;

/*!
** \brief C02 Average value of the overrall DtgMarginCsr emissions expected from all
**   the thermal dispatchable clusters
*/
template<class NextT = Container::EndOfList>
class DtgMarginCsr: public Economy_Base<DtgMarginCsrTraits, NextT>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardDtgMarginCsr VCardType;
    //! Ancestor
    typedef Variable::IVariable<DtgMarginCsr<NextT>, NextT, VCardType> AncestorType;

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

#endif // __SOLVER_VARIABLE_ECONOMY_DtgMarginCsr_H__
