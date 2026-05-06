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

namespace Antares::Solver::Variable::Economy::Reserves
{
struct VCardReserveParticipationCost
{
    //! Caption
    static std::string Caption()
    {
        return "RESERVE PARTICIPATION COST";
    }

    //! Unit
    static std::string Unit()
    {
        return "Euro";
    }

    //! The short description of the variable
    static std::string Description()
    {
        return "Reserve Participation Cost throughout all MC years of all clusters : Thermal, "
               "hydro and short term";
    }

    //! The expected results
    typedef Results<R::AllYears::Average<        // The average values throughout all years
                      R::AllYears::StdDeviation< // The standard deviation values throughout all
                                                 // years
                        R::AllYears::Min<        // The minimum values throughout all years
                          R::AllYears::Max<      // The maximum values throughout all years
                            >>>>,
                    R::AllYears::Average // The
                    >
      ResultsType;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardReserveParticipationCost VCardForSpatialAggregate;

    enum
    {
        //! Data Level
        categoryDataLevel = Category::DataLevel::area,
        //! File level (provided by the type of the results)
        categoryFileLevel = ResultsType::categoryFile
                            & (Category::FileLevel::id | Category::FileLevel::va),
        //! Precision (views)
        precision = Category::all,
        //! Indentation (GUI)
        nodeDepthForGUI = +0,
        //! Decimal precision
        decimal = 2,
        //! Number of columns used by the variable (One ResultsType per column)
        columnCount = 1,
        //! The Spatial aggregation
        spatialAggregate = Category::spatialAggregateSum,
        spatialAggregateMode = Category::spatialAggregateEachYear,
        spatialAggregatePostProcessing = 0,
        //! Intermediate values
        hasIntermediateValues = 1,
        //! Can this variable be non applicable (0 : no, 1 : yes)
        isPossiblyNonApplicable = 0,
    };

    typedef IntermediateValues IntermediateValuesBaseType;
    typedef std::vector<IntermediateValuesBaseType> IntermediateValuesType;

}; // class VCard

/*!
** \brief C02 Average value of the overrall OperatingCost emissions expected from all
**   the thermal dispatchable clusters
*/
template<class NextT = Container::EndOfList>
class ReserveParticipationCost
    : public IVariable<ReserveParticipationCost<NextT>, NextT, VCardReserveParticipationCost>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardReserveParticipationCost VCardType;
    //! Ancestor
    typedef IVariable<ReserveParticipationCost<NextT>, NextT, VCardType> AncestorType;

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
                          + static_cast<int>(VCardType::columnCount)
                              * static_cast<int>(ResultsType::count))
                       : NextType::template Statistics<CDataLevel, CFile>::count),
        };
    };

public:
    void initializeFromStudy(Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;

        InitializeResultsFromStudy(AncestorType::pResults, study);

        pValuesForTheCurrentYear.resize(pNbYearsParallel);
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            pValuesForTheCurrentYear[numSpace].initializeFromStudy(study);
        }

        // Next
        NextType::initializeFromStudy(study);
    }

    template<class R>
    static void InitializeResultsFromStudy(R& results, Study& study)
    {
        VariableAccessorType::InitializeAndReset(results, study);
    }

    void initializeFromArea(Study* study, Area* area)
    {
        // Next
        NextType::initializeFromArea(study, area);
    }

    void initializeFromLink(Study* study, AreaLink* link)
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

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        // Merge all those values with the global results
        AncestorType::pResults.merge(year, pValuesForTheCurrentYear[numSpace]);

        // Next variable
        NextType::computeSummary(year, numSpace);
    }

    void hourBegin(unsigned int hourInTheYear)
    {
        // Next variable
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        pValuesForTheCurrentYear[numSpace][state.hourInTheYear]
          = state.reserveData ? state.reserveData.value()
                                  .at(state.area->index)
                                  .reserveParticipationCostForYear[state.hourInTheYear]
                              : 0;
        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    [[nodiscard]] Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
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
    unsigned int pNbYearsParallel = 0;

}; // class RampingCost

} // namespace Antares::Solver::Variable::Economy::Reserves
