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
#ifndef __SOLVER_VARIABLE_ECONOMY_DispatchableGenMargin_H__
#define __SOLVER_VARIABLE_ECONOMY_DispatchableGenMargin_H__

#include <antares/study/area/scratchpad.h>
#include "antares/solver/variable/variable.h"

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{
struct DispatchableGenMarginTraits: public UnitMWH
{
    static std::string Caption()
    {
        return "DTG MRG";
    }

    static std::string Description()
    {
        return "Dispatchable Generation Margin";
    }

    typedef Results<R::AllYears::Average< // The average values thoughout all years
      R::AllYears::StdDeviation<          // The standard deviation values throughout all years
        R::AllYears::Min<                 // The minimum values thoughout all years
          R::AllYears::Max<               // The maximum values thoughout all years
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
        return state.scratchpad->dispatchableGenerationMargin[state.hourInTheWeek];
    }

    static void computeStats(IntermediateValues& values)
    {
        values.computeStatisticsForTheCurrentYear();
    }
};

typedef VCard_Base<DispatchableGenMarginTraits> VCardDispatchableGenMargin;

/*!
** \brief Marginal DispatchableGenMargin
*/
template<class NextT = Container::EndOfList>
class DispatchableGenMargin: public Economy_Base<DispatchableGenMarginTraits, NextT>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardDispatchableGenMargin VCardType;
    //! Ancestor
    typedef Variable::IVariable<DispatchableGenMargin<NextT>, NextT, VCardType> AncestorType;

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

public:
    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        pArea = area;
        // Next
        NextType::initializeFromArea(study, area);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        this->pValuesForTheCurrentYear[numSpace][state.hourInTheYear]
          += state.scratchpad->dispatchableGenerationMargin[state.hourInTheWeek];
        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    template<class VCardToFindT>
    inline const double* retrieveHourlyResultsForCurrentYear(unsigned int numSpace) const
    {
        typedef RetrieveResultsAssignment<
          Yuni::Static::Type::StrictlyEqual<VCardType, VCardToFindT>::Yes>
          AssignT;
        return (AssignT::Yes)
                 ? Memory::RawPointer(this->pValuesForTheCurrentYear[numSpace].hour)
                 : NextType::template retrieveHourlyResultsForCurrentYear<VCardToFindT>(numSpace);
    }

private:
    Data::Area* pArea;
};
} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_DispatchableGenMargin_H__
