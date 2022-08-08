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
#pragma once

#include "../variable.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{
struct VCardBindingConstMarginCost
{
    //! Caption
    static const char* Caption()
    {
        return "MARG. COST by BC";
    }
    //! Unit
    static const char* Unit()
    {
        return "€/MW";
    }

    //! The short description of the variable
    static const char* Description()
    {
        return "Marginal cost for binding constraints";
    }

    //! The expecte results
    typedef Results<
        R::AllYears::Average<       // The average values thoughout all years
        R::AllYears::StdDeviation<  // The standard deviation values throughout all years
        R::AllYears::Min<           // The minimum values thoughout all years
        R::AllYears::Max<           // The maximum values thoughout all years
        >>>>>
      ResultsType;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardBindingConstMarginCost VCardForSpatialAggregate;

    enum
    {
        //! Data Level
        categoryDataLevel = Category::area,
        //! File level (provided by the type of the results)
        categoryFileLevel = ResultsType::categoryFile & (Category::bc),
        //! Precision (views)
        precision = Category::all,
        //! Indentation (GUI)
        nodeDepthForGUI = +0,
        //! Decimal precision
        decimal = 0,
        //! Number of columns used by the variable
        columnCount = Category::dynamicColumns,
        //! The Spatial aggregation
        spatialAggregate = Category::spatialAggregateSum,
        spatialAggregateMode = Category::spatialAggregateEachYear,
        spatialAggregatePostProcessing = 0,
        //! Intermediate values
        hasIntermediateValues = 1,
        //! Can this variable be non applicable (0 : no, 1 : yes)
        isPossiblyNonApplicable = 0,
    };

    typedef IntermediateValues IntermediateValuesDeepType;
    typedef IntermediateValues* IntermediateValuesBaseType;
    typedef IntermediateValuesBaseType* IntermediateValuesType;

}; // class VCard

/*
    Marginal cost associated to binding constraints :
    Suppose that the BC is hourly,
    - if binding constraint is not saturated (rhs is not reached) for a given hour, the value is 0;
    - if binding constraint is saturated (rhs is reached), the value is the total benefit (€/MW) for the system
      that would result in increasing the BC's rhs of 1 MW.
*/
template<class NextT = Container::EndOfList>
class BindingConstMarginCost
 : public Variable::
     IVariable<BindingConstMarginCost<NextT>, NextT, VCardBindingConstMarginCost>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardBindingConstMarginCost VCardType;
    //! Ancestor
    typedef Variable::IVariable<BindingConstMarginCost<NextT>, NextT, VCardType>
      AncestorType;

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
        // gp : correct count if needed
        enum
        {
            count
            = ((VCardType::categoryDataLevel & CDataLevel && VCardType::categoryFileLevel & CFile)
                 ? (NextType::template Statistics<CDataLevel, CFile>::count
                    + VCardType::columnCount * ResultsType::count)
                 : NextType::template Statistics<CDataLevel, CFile>::count),
        };
    };

    static void EstimateMemoryUsage(Data::StudyMemoryUsage& u)
    {
        /*
            gp : estimate memory
        */
        NextType::EstimateMemoryUsage(u);
    }

public:
    BindingConstMarginCost() :
     pValuesForTheCurrentYear(nullptr), pSize(0)
    {
    }

    ~BindingConstMarginCost()
    {
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            delete[] pValuesForTheCurrentYear[numSpace];
        delete[] pValuesForTheCurrentYear;
    }

    void initializeFromStudy(Data::Study& study)
    {
        // gp : do we use this function ?
        // Next
        NextType::initializeFromStudy(study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        // Get the number of years in parallel
        pNbYearsParallel = study->maxNbYearsInParallel;
        pValuesForTheCurrentYear = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];
        
        // Get the area
        // pSize = area->thermal.clusterCount();
        pSize = 0; // gp : set the size
        if (pSize)
        {
            AncestorType::pResults.resize(pSize);

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
                pValuesForTheCurrentYear[numSpace]
                  = new VCardType::IntermediateValuesDeepType[pSize];

            // Minimum power values of the cluster for the whole year - from the solver in the
            // accurate mode not to be displayed in the output \todo think of a better place like
            // the DispatchableMarginForAllAreas done at the beginning of the year

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
                for (unsigned int i = 0; i != pSize; ++i)
                    pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(*study);

            // to automatically flush the memory from times to times
            unsigned int autoflush = 5;
            for (unsigned int i = 0; i != pSize; ++i)
            {
                AncestorType::pResults[i].initializeFromStudy(*study);
                AncestorType::pResults[i].reset();

                if (!--autoflush)
                {
                    autoflush = 5;
                    if (Antares::Memory::swapSupport)
                        Antares::memory.flushAll();
                }
            }
        }
        else
        {
            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                pValuesForTheCurrentYear[numSpace] = nullptr;
            }

            AncestorType::pResults.clear();
        }
        // Next
        NextType::initializeFromArea(study, area);
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        // Reset the values for the current year
        for (unsigned int i = 0; i != pSize; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].reset();
        }
        // Next variable
        NextType::yearBegin(year, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        // Merge all results for all thermal clusters
        {
            // to automatically flush the memory from times to times
            unsigned int autoflush = 5;

            for (unsigned int i = 0; i < pSize; ++i)
            {
                // Compute all statistics for the current year (daily,weekly,monthly)
                pValuesForTheCurrentYear[numSpace][i].computeStatisticsForTheCurrentYear();

                if (!--autoflush)
                {
                    autoflush = 5;
                    if (Antares::Memory::swapSupport)
                        Antares::memory.flushAll();
                }
            }
        }
        // Next variable
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary)
    {
        for (unsigned int numSpace = 0; numSpace < nbYearsForCurrentSummary; ++numSpace)
        {
            for (unsigned int i = 0; i < pSize; ++i)
            {
                // Merge all those values with the global results
                AncestorType::pResults[i].merge(numSpaceToYear[numSpace],
                                                pValuesForTheCurrentYear[numSpace][i]);
            }
        }

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
        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    void hourEnd(State& state, unsigned int hourInTheYear)
    {
        NextType::hourEnd(state, hourInTheYear);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int column,
      unsigned int numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace][column].hour;
    }

    /*
    inline Yuni::uint64 memoryUsage() const
    {
        Yuni::uint64 r = (sizeof(IntermediateValues) * pSize + IntermediateValues::MemoryUsage())
                         * pNbYearsParallel;
        r += sizeof(double) * pSize * maxHoursInAYear * pNbYearsParallel;
        r += AncestorType::memoryUsage();
        return r;
    }
    */

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      unsigned int numSpace) const
    {
        // Initializing external pointer on current variable non applicable status
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        if (AncestorType::isPrinted[0])
        {
            assert(NULL != results.data.area);
            // const auto& thermal = results.data.area->thermal;

            // Write the data for the current year
            for (uint i = 0; i < pSize; ++i)
            {
                // Write the data for the current year
                // results.variableCaption = thermal.clusters[i]->name(); // VCardType::Caption();
                pValuesForTheCurrentYear[numSpace][i].template buildAnnualSurveyReport<VCardType>(
                  results, fileLevel, precision);
            }
        }
    }

private:
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    unsigned int pSize;
    unsigned int pNbYearsParallel;

}; // class BindingConstMarginCost

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares

