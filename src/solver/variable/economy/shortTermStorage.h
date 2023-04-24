/*
** Copyright 2007-2023 RTE
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
#pragma once

#include "../variable.h"
#include "antares/study/parts/short-term-storage/STStorageOutputCaptions.h"

namespace Antares::Solver::Variable::Economy
{
struct VCardShortTermStorage
{
    //! Caption
    static std::string Caption()
    {
        return "ST storage";
    }
    //! Unit
    static const char* Unit()
    {
        return "MWh";
    }

    //! The short description of the variable
    static const char* Description()
    {
        return "Value of all short term storage throughout all MC years";
    }

    //! The expecte results
    typedef Results<R::AllYears::Average< // The average values throughout all years
      R::AllYears::StdDeviation<          // The standard deviation values throughout all years
        R::AllYears::Min<                 // The minimum values throughout all years
          R::AllYears::Max<               // The maximum values throughout all years
            >>>>>
      ResultsType;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardShortTermStorage VCardForSpatialAggregate;

    enum
    {
        //! Data Level
        categoryDataLevel = Category::area,
        //! File level (provided by the type of the results)
        categoryFileLevel = ResultsType::categoryFile & (Category::id | Category::va),
        //! Precision (views)
        precision = Category::all,
        //! Indentation (GUI)
        nodeDepthForGUI = +0,
        //! Decimal precision
        decimal = 0,
        //! Number of columns used by the variable (One ResultsType per column)
        columnCount = 27,
        //! The Spatial aggregation
        spatialAggregate = Category::spatialAggregateSum,
        spatialAggregateMode = Category::spatialAggregateEachYear,
        spatialAggregatePostProcessing = 0,
        //! Intermediate values
        hasIntermediateValues = 1,
        //! Can this variable be non applicable (0 : no, 1 : yes)
        isPossiblyNonApplicable = 0,
    };

    typedef IntermediateValues IntermediateValuesBaseType[columnCount];
    typedef IntermediateValuesBaseType* IntermediateValuesType;

    typedef IntermediateValuesBaseType* IntermediateValuesTypeForSpatialAg;

    struct Multiple
    {
        static std::string Caption(const unsigned int indx)
        {
            return Antares::Data::ShortTermStorage::getVariableCaptionFromColumnIndex(indx);
        }
    };
}; // class VCard

template<class NextT = Container::EndOfList>
class ShortTermStorageByGroup
 : public Variable::IVariable<ShortTermStorageByGroup<NextT>, NextT, VCardShortTermStorage>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardShortTermStorage VCardType;
    //! Ancestor
    typedef Variable::IVariable<ShortTermStorageByGroup<NextT>, NextT, VCardType> AncestorType;

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
    ~ShortTermStorageByGroup()
    {
        delete[] pValuesForTheCurrentYear;
    }

    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;

        InitializeResultsFromStudy(AncestorType::pResults, study);

        pValuesForTheCurrentYear = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
            for (unsigned int i = 0; i != VCardType::columnCount; ++i)
                pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(study);

        // Next
        NextType::initializeFromStudy(study);
    }

    template<class R>
    static void InitializeResultsFromStudy(R& results, Data::Study& study)
    {
        for (unsigned int i = 0; i != VCardType::columnCount; ++i)
        {
            results[i].initializeFromStudy(study);
            results[i].reset();
        }
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        // Reset the values for the current year
        for (unsigned int i = 0; i != VCardType::columnCount; ++i)
            pValuesForTheCurrentYear[numSpace][i].reset();
        // Next variable
        NextType::yearBegin(year, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        // Here we perform time-aggregations :
        // ---------------------------------
        // For a given MC year, from hourly results we compute daily, weekly, monthly and annual 
        // results by aggregation operations (averages or sums).
        // Caution : 
        //  - level results are stored in columns of which indices satisfy : col_index % 3 == 2.
        //    They are time-aggregated by means of averages
        //  - injection and withdrawal results are stored in columns of which indices 
        //    satisfy : col_index % 3 != 2.
        //    They are time-aggregated by means of sums.

        for (unsigned int col_index = 0; col_index != VCardType::columnCount; ++col_index)
        {
            bool isAnInjectionColumn = (col_index % 3) == 0;
            bool isAnWithdrawalColumn = (col_index % 3) == 1;
            bool isALevelColumn = (col_index % 3) == 2;

            if (isALevelColumn)
                pValuesForTheCurrentYear[numSpace][col_index].computeAveragesForCurrentYearFromHourlyResults();
            if (isAnInjectionColumn || isAnWithdrawalColumn)
                pValuesForTheCurrentYear[numSpace][col_index].computeStatisticsForTheCurrentYear();
        }

        // Next variable
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary)
    {
        // Here we compute synthesis : 
        //  for each interval of any time period results (hourly, daily, weekly, ...),
        //  we compute the average over all MC years :
        //  For instance :
        //      - we compute the average of the results of the first hour over all MC years
        //      - or we compute the average of the results of the n-th day over all MC years
        for (unsigned int numSpace = 0; numSpace < nbYearsForCurrentSummary; ++numSpace)
        {
            VariableAccessorType::ComputeSummary(pValuesForTheCurrentYear[numSpace],
                                                 AncestorType::pResults,
                                                 numSpaceToYear[numSpace]);
        }

        // Next variable
        NextType::computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        using namespace Antares::Data::ShortTermStorage;
        for (uint stsIndex = 0; stsIndex < state.area->shortTermStorage.count(); stsIndex++)
        {
            const auto* cluster = state.area->shortTermStorage.storagesByIndex[stsIndex];
            const uint group = groupIndex(cluster->properties.group);

            // Injection
            pValuesForTheCurrentYear[numSpace][3 * group][state.hourInTheYear]
              += (*state.hourlyResults->ShortTermStorage)[state.hourInTheWeek].injection[stsIndex];

            // Withdrawal
            pValuesForTheCurrentYear[numSpace][3 * group + 1][state.hourInTheYear]
              += (*state.hourlyResults->ShortTermStorage)[state.hourInTheWeek].withdrawal[stsIndex];

            // Levels
            pValuesForTheCurrentYear[numSpace][3 * group + 2][state.hourInTheYear]
              += (*state.hourlyResults->ShortTermStorage)[state.hourInTheWeek].level[stsIndex];
        }

        // Next item in the list
        NextType::hourForEachArea(state, numSpace);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int column,
      unsigned int numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace][column].hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      unsigned int numSpace) const
    {
        // The current variable is actually a multiple-variable.
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        for (uint i = 0; i != VCardType::columnCount; ++i)
        {
            if (AncestorType::isPrinted[i])
            {
                // Write the data for the current year
                results.variableCaption = VCardType::Multiple::Caption(i);
                pValuesForTheCurrentYear[numSpace][i].template buildAnnualSurveyReport<VCardType>(
                  results, fileLevel, precision);
            }
            results.isCurrentVarNA++;
        }
    }

private:
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    unsigned int pNbYearsParallel;

}; // class ShortTermStorageByGroup

} // namespace Antares::Solver::Variable::Economy
