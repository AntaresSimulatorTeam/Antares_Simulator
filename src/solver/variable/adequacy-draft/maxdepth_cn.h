/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __SOLVER_VARIABLE_ADEQUACY_MaxDepth_CN_H__
#define __SOLVER_VARIABLE_ADEQUACY_MaxDepth_CN_H__

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace AdequacyDraft
{
struct VCardMaxDepth_CN
{
    //! Caption
    static const char* Caption()
    {
        return "MAX DEPTH CN";
    }
    //! Unit
    static const char* Unit()
    {
        return "MW";
    }

    //! The short description of the variable
    static const char* Description()
    {
        return "Maximum shortfall expected throughout all MC years with the network";
    }

    //! The expecte results
    typedef Results<R::AllYears::Raw< // The values throughout all years
      >>
      ResultsType;

    enum
    {
        //! Data Level
        categoryDataLevel = Category::area,
        //! File level (provided by the type of the results)
        categoryFileLevel = ResultsType::categoryFile & (Category::mc),
        //! Precision (views)
        precision = Category::annual,
        //! Indentation (GUI)
        nodeDepthForGUI = +0,
        //! Decimal precision
        decimal = 0,
        //! Number of columns used by the variable (One ResultsType per column)
        columnCount = 1,
        //! The Spatial aggregation
        spatialAggregate = Category::spatialAggregateSum,
        //! Intermediate values
        hasIntermediateValues = 0,
        //! Can this variable be non applicable (0 : no, 1 : yes)
        isPossiblyNonApplicable = 0,
    };

}; // class VCard

/*!
** \brief Marginal MaxDepth_CN
*/
template<class NextT = Container::EndOfList>
class MaxDepth_CN : public Variable::IVariable<MaxDepth_CN<NextT>, NextT, VCardMaxDepth_CN>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardMaxDepth_CN VCardType;
    //! Ancestor
    typedef Variable::IVariable<MaxDepth_CN<NextT>, NextT, VCardType> AncestorType;

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
            count
            = ((VCardType::categoryDataLevel & CDataLevel && VCardType::categoryFileLevel & CFile)
                 ? (NextType::template Statistics<CDataLevel, CFile>::count
                    + VCardType::columnCount * ResultsType::count)
                 : NextType::template Statistics<CDataLevel, CFile>::count),
        };
    };

public:
    ~MaxDepth_CN()
    {
    }

    void initializeFromStudy(Data::Study& study)
    {
        // Average on all years
        AncestorType::pResults.initializeFromStudy(study);
        AncestorType::pResults.reset();

        // Next
        NextType::initializeFromStudy(study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        // Next
        NextType::initializeFromArea(study, area);
    }

    void initializeFromLink(Data::Study* study, Data::AreaLink* link)
    {
        // Next
        NextType::initializeFromAreaLink(study, link);
    }


    void simulationBegin()
    {
        // Next
        NextType::simulationBegin();
    }

    void simulationEnd()
    {
        NextType::simulationEnd();
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        pMargin = 1e100;
        // Next variable
        NextType::yearBegin(year, numSpace);
    }

    void yearEndBuildPrepareDataForEachThermalCluster(State& state,
                                                      uint year,
                                                      unsigned int numSpace)
    {
        // Next variable
        NextType::yearEndBuildPrepareDataForEachThermalCluster(state, year, numSpace);
    }

    void yearEndBuildForEachThermalCluster(State& state, uint year, unsigned int numSpace)
    {
        // Next variable
        NextType::yearEndBuildForEachThermalCluster(state, year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year)
    {
        // Next variable
        NextType::yearEndBuild(state, year);
    }

    void yearEnd(unsigned int y, unsigned int numSpace)
    {
        AncestorType::pResults.rawdata.year[y] = pMargin;
        // Next variable
        NextType::yearEnd(y, numSpace);
    }

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary)
    {
        // Next variable
        NextType::computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
    }

    void hourBegin(unsigned int hourInTheYear)
    {
        // Next variable
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        if (state.hourlyAdequacyResults->MargeAvecReseau < pMargin)
            pMargin = state.hourlyAdequacyResults->MargeAvecReseau;
        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }


    void hourEnd(State& state, unsigned int hourInTheYear)
    {
        // Next
        NextType::hourEnd(state, hourInTheYear);
    }

    void localBuildAnnualSurveyReport(SurveyResults&, int, int, unsigned int) const
    {
        // do nothing
    }

private:
    double pMargin;

}; // class MaxDepth_CN

} // namespace AdequacyDraft
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_ADEQUACY_MaxDepth_CN_H__
