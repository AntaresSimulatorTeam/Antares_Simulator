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
#pragma once

#include "antares/solver/variable/variable.h"

namespace Antares::Solver::Variable::Adequacy
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

    //! Expected results
    using ResultsType = Results<R::AllYears::Average<>, R::AllYears::Average>;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardOverallCost VCardForSpatialAggregate;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::id
                                                    | Category::FileLevel::va);
    //! Precision (views)
    static constexpr uint8_t precision = Category::all;
    //! Indentation (GUI)
    static constexpr uint8_t nodeDepthForGUI = +0;
    //! Decimal precision
    static constexpr uint8_t decimal = 0;
    //! Number of columns used by the variable (One ResultsType per column)
    static constexpr int columnCount = 1;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    //! Intermediate values
    static constexpr uint8_t hasIntermediateValues = 1;
    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    using IntermediateValuesBaseType = IntermediateValues;
    using IntermediateValuesType = std::vector<IntermediateValues>;
    using IntermediateValuesTypeForSpatialAg = IntermediateValuesBaseType*;

}; // class VCard

/*!
** \brief Overall aggregated operating cost across all Monte Carlo years (thermal + penalties).\n
*/
template<class NextT = Container::EndOfList>
class OverallCost: public Variable::IVariable<OverallCost<NextT>, NextT, VCardOverallCost>
{
public:
    using NextType = NextT;
    using VCardType = VCardOverallCost;
    using AncestorType = Variable::IVariable<OverallCost<NextT>, NextT, VCardType>;
    // Alias ResultsType déjà disponible via AncestorType; on évite la redondance.
    using VariableAccessorType = VariableAccessor<typename VCardType::ResultsType,
                                                  VCardType::columnCount>;

    static constexpr int count = 1 + NextT::count;

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        enum
        {
            count = ((VCardType::categoryDataLevel & CDataLevel
                      && VCardType::categoryFileLevel & CFile)
                       ? (NextType::template Statistics<CDataLevel, CFile>::count
                          + VCardType::columnCount * VCardType::ResultsType::count)
                       : NextType::template Statistics<CDataLevel, CFile>::count),
        };
    };

public:
    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;

        // Intermediate values
        InitializeResultsFromStudy(AncestorType::pResults, study);

        // Intermediate values
        pValuesForTheCurrentYear.resize(pNbYearsParallel);
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

    void initializeFromArea(Data::Study* study, Data::Area* area) noexcept
    {
        NextType::initializeFromArea(study, area);
    }

    void initializeFromLink(Data::Study* study, Data::AreaLink* link) noexcept
    {
        NextType::initializeFromAreaLink(study, link);
    }

    void simulationBegin() noexcept
    {
        NextType::simulationBegin();
    }

    void simulationEnd() noexcept
    {
        NextType::simulationEnd();
    }

    void yearBegin(unsigned int year, unsigned int numSpace) noexcept
    {
        // Reset the values for the current year
        pValuesForTheCurrentYear[numSpace].reset();
        // Next variable
        NextType::yearBegin(year, numSpace);
    }

    void yearEndBuildForEachThermalCluster(State& state, uint year, unsigned int numSpace) noexcept
    {
        // Sum of thermal clusters operating costs for all hours of the year
        const auto hoursInYear = state.study.runtime.rangeLimits.hour[Data::rangeCount];
        for (uint h = 0; h < hoursInYear; ++h)
        {
            pValuesForTheCurrentYear[numSpace][h] += state.thermalClusterOperatingCostForYear[h];
        }

        // Next variable
        NextType::yearEndBuildForEachThermalCluster(state, year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year, unsigned int numSpace) noexcept
    {
        // Next variable
        NextType::yearEndBuild(state, year, numSpace);
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

    void hourBegin(unsigned int hourInTheYear) noexcept
    {
        // Next variable
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace) noexcept
    {
        auto area = state.area;
        auto& thermal = state.thermal;
        const auto hourInYear = state.hourInTheYear;
        const auto hourInWeek = state.hourInTheWeek;

        // Unsupplied + spilled energy cost
        pValuesForTheCurrentYear[numSpace][hourInYear]
          += (state.hourlyResults->ValeursHorairesDeDefaillancePositive[hourInWeek]
              * area->thermal.unsuppliedEnergyCost)
             + ((state.hourlyResults->ValeursHorairesDeDefaillanceNegative[hourInWeek]
                 + state.resSpilled.entry[area->index][hourInWeek])
                * area->thermal.spilledEnergyCost);

        // Hydro costs: water value and pumping
        pValuesForTheCurrentYear[numSpace].hour[hourInYear]
          += state.problemeHebdo->CaracteristiquesHydrauliques[area->index]
               .WeeklyWaterValueStateRegular
             * (state.hourlyResults->TurbinageHoraire[hourInWeek]
                - area->hydro.pumpingEfficiency * state.hourlyResults->PompageHoraire[hourInWeek]);

        // Thermal costs (enabled clusters)
        for (auto& cluster: area->thermal.list.each_enabled())
        {
            pValuesForTheCurrentYear[numSpace][hourInYear] += thermal[area->index]
                                                                .thermalClustersOperatingCost
                                                                  [cluster->enabledIndex];
        }

        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    [[nodiscard]] Antares::Memory::Stored<double>::ConstReturnType
    retrieveRawHourlyValuesForCurrentYear(unsigned int, unsigned int numSpace) const noexcept
    {
        return pValuesForTheCurrentYear[numSpace].hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      unsigned int numSpace) const
    {
        // Initialize NA status for current variable
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        if (!AncestorType::isPrinted[0])
        {
            return; // Nothing to write for this variable
        }

        // Variable metadata
        results.variableCaption = VCardType::Caption();
        results.variableUnit = VCardType::Unit();

        // Annual details
        pValuesForTheCurrentYear[numSpace].template buildAnnualSurveyReport<VCardType>(results,
                                                                                       fileLevel,
                                                                                       precision);
    }

private:
    VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    unsigned int pNbYearsParallel{0};
}; // class OverallCost

} // namespace Antares::Solver::Variable::Adequacy
