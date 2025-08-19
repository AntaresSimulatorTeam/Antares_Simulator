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

/*
** Generic generation (solar / wind) variables (deduplicated)
*/
#ifndef ANTARES_SOLVER_VARIABLE_COMMONS_GENERATION_H
#define ANTARES_SOLVER_VARIABLE_COMMONS_GENERATION_H

#include <string_view>
#include <string>
#include <algorithm>
#include <vector>
#include <antares/study/area/area.h>
#include "antares/solver/variable/variable.h"

namespace Antares {
namespace Solver {
namespace Variable {
namespace Economy {

struct SolarTag { };
struct WindTag { };

// Traits
template<class Tag> struct GenerationTraits; // primary

template<> struct GenerationTraits<SolarTag>
{
    inline static constexpr std::string_view caption = "SOLAR";
    inline static constexpr std::string_view unit = "MWh";
    inline static constexpr std::string_view description = "Solar generation, thoughout all MC years"; // keep original typo for stability
    static constexpr auto areaMember = &Data::Area::solar; // pointer to member
};

template<> struct GenerationTraits<WindTag>
{
    inline static constexpr std::string_view caption = "WIND";
    inline static constexpr std::string_view unit = "MWh";
    inline static constexpr std::string_view description = "Wind generation, thoughout all MC years"; // keep original typo for stability
    static constexpr auto areaMember = &Data::Area::wind; // pointer to member
};

// VCard generic
template<class Tag>
struct VCardTimeSeriesValuesGeneration
{
    inline static constexpr std::string_view kCaption = GenerationTraits<Tag>::caption;
    inline static constexpr std::string_view kUnit = GenerationTraits<Tag>::unit;
    inline static constexpr std::string_view kDescription = GenerationTraits<Tag>::description;

    // Legacy API (kept until full migration): allocate std::string
    static std::string Caption() { return std::string(kCaption); }
    static std::string Unit() { return std::string(kUnit); }
    static std::string Description() { return std::string(kDescription); }

    using ResultsType = Results< R::AllYears::Average< R::AllYears::StdDeviation< R::AllYears::Min< R::AllYears::Max<>>>>>;

    using VCardForSpatialAggregate = VCardTimeSeriesValuesGeneration<Tag>;

    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile & (Category::FileLevel::id | Category::FileLevel::va);
    static constexpr uint8_t precision = Category::all;
    static constexpr uint8_t nodeDepthForGUI = 0;
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

// Helper meta for statistics
namespace detail {
    template<class VCard, class Next, int CDataLevel, int CFile>
    inline constexpr int StatisticsCount = ((VCard::categoryDataLevel & CDataLevel && VCard::categoryFileLevel & CFile)
        ? (Next::template Statistics<CDataLevel, CFile>::count + VCard::columnCount * VCard::ResultsType::count)
        : Next::template Statistics<CDataLevel, CFile>::count);
}

// Main variable template
template<class Tag, class NextT = Container::EndOfList>
class TimeSeriesValuesGeneration : public Variable::IVariable< TimeSeriesValuesGeneration<Tag, NextT>, NextT, VCardTimeSeriesValuesGeneration<Tag> >
{
public:
    using NextType = NextT;
    using VCardType = VCardTimeSeriesValuesGeneration<Tag>;
    using AncestorType = Variable::IVariable< TimeSeriesValuesGeneration<Tag, NextT>, NextT, VCardType>;
    using ResultsType = typename VCardType::ResultsType;
    using VariableAccessorType = VariableAccessor<ResultsType, VCardType::columnCount>;

    inline static constexpr int count = 1 + NextT::count;

    template<int CDataLevel, int CFile>
    struct Statistics { inline static constexpr int count = detail::StatisticsCount<VCardType, NextType, CDataLevel, CFile>; };

    void initializeFromStudy(Data::Study& study)
    {
        nbYearsParallel = study.maxNbYearsInParallel;
        InitializeResultsFromStudy(AncestorType::pResults, study);
        yearlyValues.resize(nbYearsParallel);
        for (unsigned int i = 0; i < nbYearsParallel; ++i)
            yearlyValues[i].initializeFromStudy(study);
        isRenewableGenerationAggregated = study.parameters.renewableGeneration.isAggregated();
        NextType::initializeFromStudy(study);
    }

    template<class R>
    static void InitializeResultsFromStudy(R& results, Data::Study& study)
    {
        VariableAccessorType::InitializeAndReset(results, study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area) noexcept
    {
        areaPtr = area;
        NextType::initializeFromArea(study, area);
    }

    void initializeFromLink(Data::Study* study, Data::AreaLink* link)
    {
        NextType::initializeFromAreaLink(study, link);
    }

    void simulationBegin()
    {
        for (auto& v : yearlyValues) v.reset();
        NextType::simulationBegin();
    }

    void simulationEnd() { NextType::simulationEnd(); }

    void yearBegin(unsigned int year, unsigned int space)
    {
        if (isRenewableGenerationAggregated)
        {
            auto& holder = (areaPtr->*GenerationTraits<Tag>::areaMember);
            std::copy_n(holder.series.getColumn(year), holder.series.timeSeries.height, yearlyValues[space].hour);
        }
        NextType::yearBegin(year, space);
    }

    void yearEndBuild(State& state, unsigned int year, unsigned int space)
    { NextType::yearEndBuild(state, year, space); }

    void yearEnd(unsigned int year, unsigned int space)
    {
        yearlyValues[space].computeStatisticsForTheCurrentYear();
        NextType::yearEnd(year, space);
    }

    void computeSummary(unsigned int year, unsigned int space)
    {
        AncestorType::pResults.merge(year, yearlyValues[space]);
        NextType::computeSummary(year, space);
    }

    void hourBegin(unsigned int hourInTheYear) { NextType::hourBegin(hourInTheYear); }
    void hourForEachArea(State& state, unsigned int space) { NextType::hourForEachArea(state, space); }
    void hourEnd(State& state, unsigned int hourInTheYear) { NextType::hourEnd(state, hourInTheYear); }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(unsigned int, unsigned int space) const noexcept
    { return yearlyValues[space].hour; }

    void localBuildAnnualSurveyReport(SurveyResults& results, int fileLevel, int precision, unsigned int space) const
    {
        results.isCurrentVarNA = AncestorType::isNonApplicable;
        if (AncestorType::isPrinted[0])
        {
            results.variableCaption = VCardType::kCaption.data();
            results.variableUnit = std::string(VCardType::kUnit);
            yearlyValues[space].template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
        }
    }

private:
    Data::Area* areaPtr {nullptr};
    typename VCardType::IntermediateValuesType yearlyValues;
    unsigned int nbYearsParallel {0};
    bool isRenewableGenerationAggregated {true};
};

// Aliases preserving previous public names
using VCardTimeSeriesValuesSolar = VCardTimeSeriesValuesGeneration<SolarTag>;
using VCardTimeSeriesValuesWind = VCardTimeSeriesValuesGeneration<WindTag>;

template<class NextT = Container::EndOfList>
using TimeSeriesValuesSolar = TimeSeriesValuesGeneration<SolarTag, NextT>;

template<class NextT = Container::EndOfList>
using TimeSeriesValuesWind = TimeSeriesValuesGeneration<WindTag, NextT>;

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // ANTARES_SOLVER_VARIABLE_COMMONS_GENERATION_H

