/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
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
#ifndef __SOLVER_VARIABLE_ECONOMY_OverallCost_H__
#define __SOLVER_VARIABLE_ECONOMY_OverallCost_H__

#include "antares/solver/variable/variable.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{
struct VCardOverallCost
{
    //! Caption
    static std::string Caption()
    {
        return "OV. COST";
    }

    //! Unit
    static std::string Unit()
    {
        return "Euro";
    }

    //! The short description of the variable
    static std::string Description()
    {
        return "Overall Cost throughout all MC years";
    }

    //! The expecte results
    typedef Results<R::AllYears::Average< // The average values throughout all years
                      >,
                    R::AllYears::Average // Use these values for spatial cluster
                    >
      ResultsType;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardOverallCost VCardForSpatialAggregate;

    //! Data Level
    static constexpr uint8_t categoryDataLevel =  Category::DataLevel::area;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel =   ResultsType::categoryFile & (Category::FileLevel::id | Category::FileLevel::va);
    //! Precision (views)
    static constexpr uint8_t precision =   Category::all;
    //! Indentation (GUI)
    static constexpr uint8_t nodeDepthForGUI =   +0;
    //! Decimal precision
    static constexpr uint8_t decimal =   0;
    //! Number of columns used by the variable (One ResultsType per column)
    static constexpr int columnCount =    1;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate =   Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode =   Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing =   0;
    //! Intermediate values
    static constexpr uint8_t hasIntermediateValues =   1;
    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable =   0;

    typedef IntermediateValues IntermediateValuesBaseType;
    typedef IntermediateValues* IntermediateValuesType;

    typedef IntermediateValuesBaseType* IntermediateValuesTypeForSpatialAg;

}; // class VCard

/*!
** \brief C02 Average value of the overrall OverallCost emissions expected from all
**   the thermal dispatchable clusters
*/
template<class NextT = Container::EndOfList>
class OverallCost: public Variable::IVariable<OverallCost<NextT>, NextT, VCardOverallCost>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardOverallCost VCardType;
    //! Ancestor
    typedef Variable::IVariable<OverallCost<NextT>, NextT, VCardType> AncestorType;

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
    ~OverallCost()
    {
        delete[] pValuesForTheCurrentYear;
    }

    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;

        // Intermediate values
        InitializeResultsFromStudy(AncestorType::pResults, study);

        // Intermediate values
        pValuesForTheCurrentYear = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            pValuesForTheCurrentYear[numSpace].initializeFromStudy(study);
        }

        // Next
        NextType::initializeFromStudy(study);
    }

    template<class R>
    static void InitializeResultsFromStudy(R& results, Data::Study& study)
    {
        VariableAccessorType::InitializeAndReset(results, study);
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
        // Reset the values for the current year
        pValuesForTheCurrentYear[numSpace].reset();
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
            pValuesForTheCurrentYear[numSpace][i] += state.thermalClusterOperatingCostForYear[i];
            // Incrementing annual system cost (to be printed in output into a separate file)
            state.annualSystemCost += state.thermalClusterOperatingCostForYear[i];
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
        // Compute all statistics for the current year (daily,weekly,monthly)
        pValuesForTheCurrentYear[numSpace].computeStatisticsForTheCurrentYear();

        // Next variable
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary)
    {
        for (unsigned int numSpace = 0; numSpace < nbYearsForCurrentSummary; ++numSpace)
        {
            // Merge all those values with the global results
            AncestorType::pResults.merge(numSpaceToYear[numSpace] /*year*/,
                                         pValuesForTheCurrentYear[numSpace]);
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
        const double costForSpilledOrUnsuppliedEnergy =
          // Total UnsupliedEnergy emissions
          (state.hourlyResults->ValeursHorairesDeDefaillancePositive[state.hourInTheWeek]
           * state.area->thermal.unsuppliedEnergyCost)
          + (state.hourlyResults->ValeursHorairesDeDefaillanceNegative[state.hourInTheWeek]
             * state.area->thermal.spilledEnergyCost)
          // Current hydro storage and pumping generation costs
          + (state.hourlyResults->valeurH2oHoraire[state.hourInTheWeek]
             * (state.hourlyResults->TurbinageHoraire[state.hourInTheWeek]
                - state.area->hydro.pumpingEfficiency
                    * state.hourlyResults->PompageHoraire[state.hourInTheWeek]));

        pValuesForTheCurrentYear[numSpace][state.hourInTheYear] += costForSpilledOrUnsuppliedEnergy;

        // Incrementing annual system cost (to be printed in output into a separate file)
        state.annualSystemCost += costForSpilledOrUnsuppliedEnergy;

        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int,
      unsigned int numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace].hour;
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
            // Write the data for the current year
            results.variableCaption = VCardType::Caption();
            results.variableUnit = VCardType::Unit();
            pValuesForTheCurrentYear[numSpace]
              .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
        }
    }

private:
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    unsigned int pNbYearsParallel;

}; // class OverallCost

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_ECONOMY_OverallCost_H__
