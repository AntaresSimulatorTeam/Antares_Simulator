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
#ifndef __SOLVER_VARIABLE_ECONOMY_SPATIAL_AGGREGATE_H__
#define __SOLVER_VARIABLE_ECONOMY_SPATIAL_AGGREGATE_H__

#include "../variable.h"
// #include <antares/logs.h>	// In case it is needed

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Common
{
template<int ColumnCountT, class VCardT>
struct MultipleCaptionProxy
{
    static const char* Caption(const uint indx)
    {
        return VCardT::Multiple::Caption(indx);
    }
};

template<class VCardT>
struct MultipleCaptionProxy<0, VCardT>
{
    static const char* Caption(const uint indx)
    {
        return NULL;
    }
};

template<class VCardT>
struct MultipleCaptionProxy<1, VCardT>
{
    static const char* Caption(const uint indx)
    {
        return NULL;
    }
};

template<class VCardT>
struct MultipleCaptionProxy<Category::dynamicColumns, VCardT>
{
    static const char* Caption(const uint indx)
    {
        return NULL;
    }
};

template<template<class> class V>
struct VCardProxy
{
    //! The real VCard for the variable
    typedef typename V<Container::EndOfList>::VCardType VCardOrigin;

    //! Caption
    static const char* Caption()
    {
        return VCardOrigin::Caption();
    }
    //! Unit
    static const char* Unit()
    {
        return VCardOrigin::Unit();
    }
    //! The short description of the variable
    static const char* Description()
    {
        return VCardOrigin::Description();
    }

    //! The expecte results
    typedef typename VCardOrigin::ResultsType ResultsType;
    //! The VCard to look for for calculating spatial aggregates
    typedef typename VCardOrigin::VCardForSpatialAggregate VCardForSpatialAggregate;

    typedef typename VCardOrigin::IntermediateValuesType IntermediateValuesType;
    typedef typename VCardOrigin::IntermediateValuesBaseType IntermediateValuesBaseType;
    typedef
      typename VCardOrigin::IntermediateValuesTypeForSpatialAg IntermediateValuesTypeForSpatialAg;

    enum
    {
        //! Data Level
        categoryDataLevel = Category::setOfAreas,
        //! File level (provided by the type of the results)
        categoryFileLevel = VCardOrigin::categoryFileLevel,
        //! Precision (views)
        precision = VCardOrigin::precision,
        //! Indentation (GUI)
        nodeDepthForGUI = +0,
        //! Decimal precision
        decimal = VCardOrigin::decimal,
        //! Number of columns used by the variable (One ResultsType per column)
        columnCount = VCardOrigin::columnCount,
        //! The Spatial aggregation
        spatialAggregate = Category::noSpatialAggregate,
        spatialAggregateMode = Category::spatialAggregateEachYear,
        spatialAggregatePostProcessing = 0,

        //! Intermediate values
        hasIntermediateValues = 1,
        //! Can this variable be non applicable (0 : no, 1 : yes)
        isPossiblyNonApplicable = VCardOrigin::isPossiblyNonApplicable,
        clusterType = Category::None
    };

    struct Multiple
    {
        static const char* Caption(const uint indx)
        {
            return MultipleCaptionProxy<columnCount, VCardOrigin>::Caption(indx);
        }
    };

}; // class VCard

template<template<class> class VarT, class NextT = Container::EndOfList>
class SpatialAggregate
 : public Variable::IVariable<SpatialAggregate<VarT, NextT>, NextT, VCardProxy<VarT>>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;

    //! VCard
    typedef VCardProxy<VarT> VCardType;
    //! Ancestor
    typedef Variable::IVariable<SpatialAggregate<VarT, NextT>, NextT, VCardType> AncestorType;

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
    SpatialAggregate()
    {
    }

    ~SpatialAggregate()
    {
        delete[] pValuesForTheCurrentYear;
    }

    void initializeFromStudy(Data::Study& study)
    {
        typedef
          typename VCardType::VCardOrigin::IntermediateValuesBaseType IntermediateValuesBaseType;
        pNbYearsParallel = study.maxNbYearsInParallel;

        // Intermediate values
        VarT<Container::EndOfList>::InitializeResultsFromStudy(AncestorType::pResults, study);
        pValuesForTheCurrentYear = new IntermediateValuesBaseType[pNbYearsParallel];
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            VariableAccessorType::InitializeAndReset(pValuesForTheCurrentYear[numSpace], study);

        auto& limits = study.runtime->rangeLimits;

        pRatioYear = 100. / (double)limits.year[Data::rangeCount];
        pRatioDay = 100. / (double)limits.day[Data::rangeCount];
        pRatioMonth = 100. / (double)limits.month[Data::rangeCount];
        pRatioWeek = 100. / (double)limits.week[Data::rangeCount];

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

    void yearBegin(uint year)
    {
        // Next variable
        NextType::yearBegin(year);
    }

    void yearEndBuildPrepareDataForEachThermalCluster(State& state, uint year)
    {
        // Next variable
        NextType::yearEndBuildPrepareDataForEachThermalCluster(state, year);
    }

    void yearEndBuildForEachThermalCluster(State& state, uint year)
    {
        // Next variable
        NextType::yearEndBuildForEachThermalCluster(state, year);
    }

    void yearEndBuild(State& state, unsigned int year)
    {
        // Next variable
        NextType::yearEndBuild(state, year);
    }

    void yearEnd(uint year)
    {
        // Next variable
        NextType::yearEnd(year);
    }

    void weekBegin(State& state)
    {
        // Next variable
        NextType::weekBegin(state);
    }

    void weekEnd(State& state)
    {
        // Next variable
        NextType::weekEnd(state);
    }

    void hourBegin(uint hourInTheYear)
    {
        // Next variable
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state)
    {
        // Next variable
        NextType::hourForEachArea(state);
    }

    void hourForEachThermalCluster(State& state)
    {
        // Next item in the list
        NextType::hourForEachThermalCluster(state);
    }

    void hourEnd(State& state, uint hourInTheYear)
    {
        // Next variable
        NextType::hourEnd(state, hourInTheYear);
    }

    template<class V, class SetT>
    void yearEndSpatialAggregates(V& allVars, uint year, const SetT& set, uint numSpace)
    {
        if (VCardType::VCardOrigin::spatialAggregateMode & Category::spatialAggregateEachYear)
            internalSpatialAggregateForCurrentYear(allVars, set, numSpace);

        // Next variable
        NextType::template yearEndSpatialAggregates(allVars, year, set, numSpace);
    }

    template<class V>
    void computeSpatialAggregatesSummary(V& allVars,
                                         std::map<unsigned int, unsigned int>& numSpaceToYear,
                                         uint nbYearsForCurrentSummary)
    {
        if (VCardType::VCardOrigin::spatialAggregateMode & Category::spatialAggregateEachYear)
            internalSpatialAggregateForParallelYears(numSpaceToYear, nbYearsForCurrentSummary);

        // Next variable
        NextType::computeSpatialAggregatesSummary(
          allVars, numSpaceToYear, nbYearsForCurrentSummary);
    }

    template<class V, class SetT>
    void simulationEndSpatialAggregates(V& allVars, const SetT& set)
    {
        if (VCardType::VCardOrigin::spatialAggregateMode & Category::spatialAggregateOnce)
            internalSpatialAggregate(allVars, 0, set);

        // Next variable
        NextType::template simulationEndSpatialAggregates(allVars, set);
    }

    inline void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
    {
        // Generate the Digest for the local results (districts part)
        if (VCardType::columnCount != 0 && (VCardType::categoryDataLevel & Category::setOfAreas))
        {
            // Initializing pointer on variable non applicable and print stati arrays to beginning
            results.isPrinted = AncestorType::isPrinted;
            results.isCurrentVarNA = AncestorType::isNonApplicable;
            results.clusterType = AncestorType::clusterType;

            VariableAccessorType::template BuildDigest<typename VCardType::VCardOrigin>(
              results, AncestorType::pResults, digestLevel, dataLevel);
        }
        // Ask to build the digest to the next variable
        NextType::buildDigest(results, digestLevel, dataLevel);
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      uint numSpace) const
    {
        if (VCardType::columnCount != 0 && (VCardType::categoryDataLevel & Category::setOfAreas))
        {
            // Initializing pointer on variable non applicable and print stati arrays to beginning
            results.isPrinted = AncestorType::isPrinted;
            results.isCurrentVarNA = AncestorType::isNonApplicable;
            results.clusterType = AncestorType::clusterType;

            typedef VariableAccessor<typename VCardType::IntermediateValuesBaseType,
                                     VCardType::columnCount>
              VAType;
            VAType::template BuildAnnualSurveyReport<typename VCardType::VCardOrigin>(
              results, pValuesForTheCurrentYear[numSpace], fileLevel, precision);
        }
    }

private:
    template<class V, class SetT>
    void internalSpatialAggregate(V& allVars, uint year, const SetT& set)
    {
        typedef typename VCardType::VCardOrigin VCardOrigin;
        // Reset the results
        VariableAccessorType::Reset(pValuesForTheCurrentYear[0]);

        // Make the spatial cluster
        if (!set.empty())
        {
            auto end = set.end();
            for (auto i = set.begin(); i != end; ++i)
            {
                allVars.template computeSpatialAggregateWith<
                  typename VCardType::VCardForSpatialAggregate> //<typename VCardType::VCardOrigin>
                  (pValuesForTheCurrentYear[0], *i /* the current area */, 0);
            }

            // The spatial cluster may be an average
            if (VCardType::VCardOrigin::spatialAggregate & Category::spatialAggregateAverage)
                VariableAccessorType::MultiplyHourlyResultsBy(pValuesForTheCurrentYear[0],
                                                              1. / set.size());
            // The spatial cluster may be an average
            if (VCardType::VCardOrigin::spatialAggregate
                & Category::spatialAggregateSumThen1IfPositive)
                VariableAccessorType::SetTo1IfPositive(pValuesForTheCurrentYear[0]);
            if (VCardType::VCardOrigin::spatialAggregate & Category::spatialAggregateOr)
                VariableAccessorType::Or(pValuesForTheCurrentYear[0]);
        }
        else
            assert(!set.empty() && "The set should not be empty at this point");

        // Compute all statistics for the current year (daily,weekly,monthly,...)
        VariableAccessorType::template ComputeStatistics<VCardOrigin>(pValuesForTheCurrentYear[0]);
        VariableAccessorType::ComputeSummary(
          pValuesForTheCurrentYear[0], AncestorType::pResults, year);
    }

    template<class V, class SetT>
    void internalSpatialAggregateForCurrentYear(V& allVars, const SetT& set, uint numSpace)
    {
        typedef typename VCardType::VCardOrigin VCardOrigin;
        // Reset the results
        VariableAccessorType::Reset(pValuesForTheCurrentYear[numSpace]);

        // Make the spatial cluster
        if (!set.empty())
        {
            auto end = set.end();
            for (auto i = set.begin(); i != end; ++i)
            {
                allVars.template computeSpatialAggregateWith<
                  typename VCardType::VCardForSpatialAggregate> //<typename VCardType::VCardOrigin>
                  (pValuesForTheCurrentYear[numSpace], *i /* the current area */, numSpace);
            }

            // The spatial cluster may be an average
            if (VCardType::VCardOrigin::spatialAggregate & Category::spatialAggregateAverage)
                VariableAccessorType::MultiplyHourlyResultsBy(pValuesForTheCurrentYear[numSpace],
                                                              1. / set.size());
            // The spatial cluster may be an average
            if (VCardType::VCardOrigin::spatialAggregate
                & Category::spatialAggregateSumThen1IfPositive)
                VariableAccessorType::SetTo1IfPositive(pValuesForTheCurrentYear[numSpace]);
            if (VCardType::VCardOrigin::spatialAggregate & Category::spatialAggregateOr)
                VariableAccessorType::Or(pValuesForTheCurrentYear[numSpace]);
        }
        else
            assert(!set.empty() && "The set should not be empty at this point");

        // Compute all statistics for the current year (daily,weekly,monthly,...)
        VariableAccessorType::template ComputeStatistics<VCardOrigin>(
          pValuesForTheCurrentYear[numSpace]);
    }

    void internalSpatialAggregateForParallelYears(
      std::map<unsigned int, unsigned int>& numSpaceToYear,
      uint nbYearsForCurrentSummary)
    {
        for (unsigned int numSpace = 0; numSpace < nbYearsForCurrentSummary; ++numSpace)
        {
            // Merge all those values with the global results
            VariableAccessorType::ComputeSummary(
              pValuesForTheCurrentYear[numSpace], AncestorType::pResults, numSpaceToYear[numSpace]);
        }
    }

private:
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesTypeForSpatialAg pValuesForTheCurrentYear;

    double pRatioYear;
    double pRatioDay;
    double pRatioMonth;
    double pRatioWeek;
    unsigned int pNbYearsParallel;

}; // class SpatialAggregate

} // namespace Common
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_ECONOMY_SPATIAL_AGGREGATE_H__
