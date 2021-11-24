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
#ifndef __SOLVER_VARIABLE_ECONOMY_NTC_H__
#define __SOLVER_VARIABLE_ECONOMY_NTC_H__

#include "../../variable.h"
#include <antares/study/area/constants.h>

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{
/*
 NTC capacities.
 Note that no columns are produced (categoryFileLevel = 0)
 These data exist for the sake of quadratic optimization (FLOW QUAD variable, see flowQuad.h),
 for which we need the average houly direct/indirect NTC for each link.
*/
struct VCardNTC
{
    //! Caption
    static const char* Caption()
    {
        return "NTC";
    }
    //! Unit
    static const char* Unit()
    {
        return "MWh";
    }

    //! The short description of the variable
    static const char* Description()
    {
        return "Net transfer capacities";
    }

    //! The expecte results
    typedef Results<R::AllYears::Average< // The average values throughout all years
      R::AllYears::StdDeviation<          // The standard deviation values throughout all years
        R::AllYears::Min<                 // The minimum values throughout all years
          R::AllYears::Max<               // The maximum values throughout all years
            >>>>>
      ResultsType;

    enum
    {
        //! Data Level
        categoryDataLevel = Category::link,
        //! File level (provided by the type of the results)
        categoryFileLevel = 0, // No output is produced, this is just a temporary variable
                               //! Precision (views)
        precision = Category::all,
        //! Indentation (GUI)
        nodeDepthForGUI = +0,
        //! Decimal precision
        decimal = 0,
        //! Number of columns used by the variable (One ResultsType per column)
        columnCount = 2,
        //! The Spatial aggregation
        spatialAggregate = Category::spatialAggregateSum,
        spatialAggregateMode = Category::spatialAggregateEachYear,
        spatialAggregatePostProcessing = 0,
        //! Intermediate values
        hasIntermediateValues = 0,
        //! Can this variable be non applicable (0 : no, 1 : yes)
        isPossiblyNonApplicable = 1,
    };

    typedef IntermediateValues IntermediateValuesBaseType[columnCount];
    typedef IntermediateValuesBaseType* IntermediateValuesType;

    struct Multiple
    {
        static const char* Caption(uint indx)
        {
            switch (indx)
            {
            case 0:
                return "NTC direct";
            case 1:
                return "NTC indirect";
            default:
                return "<unknown>";
            }
        }
    };

}; // class VCard

/*!
** \brief NTC
*/
template<class NextT = Container::EndOfList>
class NTC : public Variable::IVariable<NTC<NextT>, NextT, VCardNTC>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardNTC VCardType;
    //! Ancestor
    typedef Variable::IVariable<NTC<NextT>, NextT, VCardType> AncestorType;

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
    ~NTC()
    {
        delete[] pValuesForTheCurrentYear;
        delete[] pValuesForYearLocalReport;
    }

    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;

        yearsWeight = study.parameters.getYearsWeight();
        yearsWeightSum = study.parameters.getYearsWeightSum();

        // Average on all years
        for (uint i = 0; i != VCardType::columnCount; ++i)
        {
            AncestorType::pResults[i].initializeFromStudy(study);
            AncestorType::pResults[i].reset();
        }

        pValuesForTheCurrentYear = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
            for (unsigned int i = 0; i != VCardType::columnCount; ++i)
                pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(study);

        pValuesForYearLocalReport = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
            for (unsigned int i = 0; i != VCardType::columnCount; ++i)
                pValuesForYearLocalReport[numSpace][i].initializeFromStudy(study);

        // Next
        NextType::initializeFromStudy(study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        // Next
        NextType::initializeFromArea(study, area);
    }

    void initializeFromAreaLink(Data::Study* study, Data::AreaLink* link)
    {
        // Next
        NextType::initializeFromAreaLink(study, link);
    }

    void simulationBegin()
    {
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
        {
            pValuesForTheCurrentYear[numSpace][0].reset();
            pValuesForTheCurrentYear[numSpace][1].reset();
        }

        // Next
        NextType::simulationBegin();
    }

    void simulationEnd()
    {
        // Next variable
        NextType::simulationEnd();
    }

    void yearBegin(uint year, unsigned int numSpace)
    {
        // Reset
        pValuesForTheCurrentYear[numSpace][0].reset();
        pValuesForTheCurrentYear[numSpace][1].reset();

        pValuesForYearLocalReport[numSpace][0].reset();
        pValuesForYearLocalReport[numSpace][1].reset();

        // Next variable
        NextType::yearBegin(year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year)
    {
        // Next variable
        NextType::yearEndBuild(state, year);
    }

    void yearEnd(uint year, uint numSpace)
    {
        for (uint i = 0; i != VCardType::columnCount; ++i)
        {
            // Compute all statistics for the current year (daily,weekly,monthly)
            pValuesForTheCurrentYear[numSpace][i].computeProbabilitiesForTheCurrentYear();
        }

        // Next variable
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary)
    {
        for (unsigned int numSpace = 0; numSpace < nbYearsForCurrentSummary; ++numSpace)
        {
            for (uint i = 0; i != VCardType::columnCount; ++i)
            {
                // Merge all those values with the global results
                AncestorType::pResults[i].merge(numSpaceToYear[numSpace],
                                                pValuesForTheCurrentYear[numSpace][i]);
            }
        }

        // Next variable
        NextType::computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
    }

    void hourBegin(uint hourInTheYear)
    {
        // Next variable
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    void hourForEachLink(State& state, unsigned int numSpace)
    {
        // Ratio take into account MC year weight
        float ratio = yearsWeight[state.year] / yearsWeightSum;

        assert(state.link != NULL);
        auto& linkDirectCapa = state.link->directCapacities;
        auto& linkIndirectCapa = state.link->indirectCapacities;
        const int tsIndex = NumeroChroniquesTireesParInterconnexion[numSpace][state.link->index]
                              .TransmissionCapacities;
        // NTC direct
        pValuesForTheCurrentYear[numSpace][0].hour[state.hourInTheYear]
          = linkDirectCapa.entry[tsIndex][state.hourInTheYear];
        // NTC indirect
        pValuesForTheCurrentYear[numSpace][1].hour[state.hourInTheYear]
          = linkIndirectCapa.entry[tsIndex][state.hourInTheYear];

        // Next item in the list
        NextType::hourForEachLink(state, numSpace);
    }

    void hourEnd(State& state, uint hourInTheYear)
    {
        // Next
        NextType::hourEnd(state, hourInTheYear);
    }

    void beforeYearByYearExport(uint /*year*/, uint numSpace)
    {
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      uint column,
      uint numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace][column].hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      uint numSpace) const
    {
        // The current variable is actually a multiple-variable.
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        for (uint i = 0; i != VCardType::columnCount; ++i)
        {
            if (AncestorType::isPrinted[i])
            {
                // Write the data for the current year
                results.variableCaption = VCardType::Multiple::Caption(i);
                pValuesForYearLocalReport[numSpace][i].template buildAnnualSurveyReport<VCardType>(
                  results, fileLevel, precision);
            }
            results.isCurrentVarNA++;
        }
    }

private:
    std::vector<float> yearsWeight;
    float yearsWeightSum;
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    typename VCardType::IntermediateValuesType pValuesForYearLocalReport;
    unsigned int pNbYearsParallel;

}; // class NTC

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_ECONOMY_NTC_H__
