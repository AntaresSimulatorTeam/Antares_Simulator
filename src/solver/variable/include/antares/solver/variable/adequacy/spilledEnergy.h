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

// <concepts> non requis ici (unification des vérifications dans variable.h)

#include "antares/solver/variable/variable.h"

namespace Antares::Solver::Variable::Adequacy
{
// Concept minimal pour contraindre NextT
// template<typename T>
// concept SpilledEnergyNext = requires {
//     { T::count };
//     typename T::template Statistics<0, 0>;
// };

struct VCardSpilledEnergy
{
    static std::string Caption()
    {
        return "SPIL. ENRG";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Spilled Energy (generation that cannot be satisfied)";
    }

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>,
      R::AllYears::Average>;
    using VCardForSpatialAggregate = VCardSpilledEnergy;

    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::id
                                                    | Category::FileLevel::va);
    static constexpr uint8_t precision = Category::all;
    static constexpr uint8_t nodeDepthForGUI = +0;
    static constexpr uint8_t decimal = 0;
    static constexpr int columnCount = 1;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    static constexpr uint8_t hasIntermediateValues = 1;
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    using IntermediateValuesBaseType = IntermediateValues;
    using IntermediateValuesType = std::vector<IntermediateValues>;
    using IntermediateValuesTypeForSpatialAg = IntermediateValuesBaseType*;
};

/*!
** \brief Hourly spilled energy aggregated across Monte Carlo years (deficit absorption +
*curtailment).
*/
template<class NextT = Container::EndOfList>
class SpilledEnergy: public Variable::IVariable<SpilledEnergy<NextT>, NextT, VCardSpilledEnergy>
{
public:
    using NextType = NextT;
    using VCardType = VCardSpilledEnergy;
    using AncestorType = Variable::IVariable<SpilledEnergy<NextT>, NextT, VCardType>;
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

    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;
        InitializeResultsFromStudy(AncestorType::pResults, study);
        pValuesForTheCurrentYear.resize(pNbYearsParallel);
        for (unsigned int s = 0; s < pNbYearsParallel; ++s)
        {
            pValuesForTheCurrentYear[s].initializeFromStudy(study);
        }
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
        for (auto& v: pValuesForTheCurrentYear)
        {
            v.reset();
        }
        NextType::simulationBegin();
    }

    void simulationEnd() noexcept
    {
        NextType::simulationEnd();
    }

    void yearBegin(unsigned int year, unsigned int numSpace) noexcept
    {
        pValuesForTheCurrentYear[numSpace].reset();
        NextType::yearBegin(year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year, unsigned int numSpace) noexcept
    {
        NextType::yearEndBuild(state, year, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        pValuesForTheCurrentYear[numSpace].computeStatisticsForTheCurrentYear();
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        AncestorType::pResults.merge(year, pValuesForTheCurrentYear[numSpace]);
        NextType::computeSummary(year, numSpace);
    }

    void hourBegin(unsigned int hourInTheYear) noexcept
    {
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace) noexcept
    {
        assert(state.hourlyResults && "Invalid pointer to simplex results");
        pValuesForTheCurrentYear[numSpace][state.hourInTheYear]
          = state.hourlyResults->ValeursHorairesDeDefaillanceNegative[state.hourInTheWeek]
            + state.resSpilled.entry[state.area->index][state.hourInTheWeek];
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
        results.isCurrentVarNA = AncestorType::isNonApplicable;
        if (!AncestorType::isPrinted[0])
        {
            return;
        }
        results.variableCaption = VCardType::Caption();
        results.variableUnit = VCardType::Unit();
        pValuesForTheCurrentYear[numSpace].template buildAnnualSurveyReport<VCardType>(results,
                                                                                       fileLevel,
                                                                                       precision);
    }

private:
    VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    unsigned int pNbYearsParallel{0};
};

} // namespace Antares::Solver::Variable::Adequacy
