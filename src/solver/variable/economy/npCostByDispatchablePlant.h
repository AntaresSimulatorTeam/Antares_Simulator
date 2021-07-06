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
#ifndef __SOLVER_VARIABLE_ECONOMY_NonProportionalCostByDispatchablePlant_H__
#define __SOLVER_VARIABLE_ECONOMY_NonProportionalCostByDispatchablePlant_H__

#include "../variable.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{
struct VCardNonProportionalCostByDispatchablePlant
{
    //! Caption
    static const char* Caption()
    {
        return "NP Cost by plant";
    }
    //! Unit
    static const char* Unit()
    {
        return "NP Cost - Euro";
    }

    //! The short description of the variable
    static const char* Description()
    {
        return "Non proportional costs by all the clusters";
    }

    //! The expected results
    typedef Results<R::AllYears::Average< // The average values throughout all years
      >>
      ResultsType;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardNonProportionalCostByDispatchablePlant VCardForSpatialAggregate;

    enum
    {
        //! Data Level
        categoryDataLevel = Category::area,
        //! File level (provided by the type of the results)
        categoryFileLevel = ResultsType::categoryFile & (Category::de),
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
        clusterType = Category::Thermal
    };

    typedef IntermediateValues IntermediateValuesDeepType;
    typedef IntermediateValues* IntermediateValuesBaseType;
    typedef IntermediateValuesBaseType* IntermediateValuesType;

    // typedef IntermediateValues IntermediateValuesType;

}; // class VCard

/*!
** \brief C02 Average value of the overrall OperatingCost emissions expected from all
**   the thermal dispatchable clusters
*/
template<class NextT = Container::EndOfList>
class NonProportionalCostByDispatchablePlant
 : public Variable::IVariable<NonProportionalCostByDispatchablePlant<NextT>,
                              NextT,
                              VCardNonProportionalCostByDispatchablePlant>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardNonProportionalCostByDispatchablePlant VCardType;
    //! Ancestor
    typedef Variable::IVariable<NonProportionalCostByDispatchablePlant<NextT>, NextT, VCardType>
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
        if (u.area)
        {
            for (unsigned int i = 0; i != u.area->thermal.list.size(); ++i)
            {
                Solver::Variable::IntermediateValues::EstimateMemoryUsage(u);
                ResultsType::EstimateMemoryUsage(u);
                u.requiredMemoryForOutput += sizeof(Solver::Variable::IntermediateValues);
                u.requiredMemoryForOutput += sizeof(typename VCardType::ResultsType);
                u.requiredMemoryForOutput += sizeof(void*) * 2;

                // year-by-year
                if (!u.gatheringInformationsForInput)
                {
                    if (u.study.parameters.yearByYear && u.mode != Data::stdmAdequacyDraft)
                    {
                        for (unsigned int i = 0; i != u.years; ++i)
                            u.takeIntoConsiderationANewTimeserieForDiskOutput(false);
                    }
                }
            }
        }
        NextType::EstimateMemoryUsage(u);
    }

public:
    NonProportionalCostByDispatchablePlant() : pValuesForTheCurrentYear(NULL), pSize(0)
    {
    }

    ~NonProportionalCostByDispatchablePlant()
    {
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            delete[] pValuesForTheCurrentYear[numSpace];
        delete[] pValuesForTheCurrentYear;
    }

    void initializeFromStudy(Data::Study& study)
    {
        // Next
        NextType::initializeFromStudy(study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        // Get the number of years in parallel
        pNbYearsParallel = study->maxNbYearsInParallel;
        pValuesForTheCurrentYear = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];

        // Get the area
        pSize = area->thermal.clusterCount;
        if (pSize)
        {
            AncestorType::pResults.resize(pSize);
            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
                pValuesForTheCurrentYear[numSpace]
                  = new VCardType::IntermediateValuesDeepType[pSize];

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
                pValuesForTheCurrentYear[numSpace] = nullptr;

            AncestorType::pResults.clear();
        }

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
        // Reset the values for the current year
        for (unsigned int i = 0; i != pSize; ++i)
            pValuesForTheCurrentYear[numSpace][i].reset();

        // Next variable
        NextType::yearBegin(year, numSpace);
    }

    void yearEndBuildForEachThermalCluster(State& state, uint year, unsigned int numSpace)
    {
        // Get end year calculations
        for (unsigned int i = state.study.runtime->rangeLimits.hour[Data::rangeBegin];
             i <= state.study.runtime->rangeLimits.hour[Data::rangeEnd];
             ++i)
        {
            pValuesForTheCurrentYear[numSpace][state.thermalCluster->areaWideIndex].hour[i]
              = state.thermalClusterNonProportionalCostForYear[i];
        }

        // Next variable
        NextType::yearEndBuildForEachThermalCluster(state, year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year)
    {
        // Next variable
        NextType::yearEndBuild(state, year);
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

    void hourForEachThermalCluster(State& state, unsigned int numSpace)
    {
        // Total Non Proportional cost for this hour
        // NP = startup cost + fixed cost
        // pValuesForTheCurrentYear[state.thermalCluster->areaWideIndex].hour[state.hourInTheYear]
        // += production for the current thermal dispatchable cluster
        //	(state.thermalClusterNonProportionalCost);

        // Next item in the list
        NextType::hourForEachThermalCluster(state, numSpace);
    }

    void hourEnd(State& state, unsigned int hourInTheYear)
    {
        NextType::hourEnd(state, hourInTheYear);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int,
      unsigned int numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace]->hour;
    }

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
            const auto& thermal = results.data.area->thermal;

            // Write the data for the current year
            for (uint i = 0; i < pSize; ++i)
            {
                // Write the data for the current year
                results.variableCaption = thermal.clusters[i]->name(); // VCardType::Caption();
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

}; // class NonProportionalCostByDispatchablePlant

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_ECONOMY_NonProportionalCostByDispatchablePlant_H__
