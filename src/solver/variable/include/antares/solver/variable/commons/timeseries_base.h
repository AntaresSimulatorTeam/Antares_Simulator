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

#include <algorithm>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <antares/study/area/area.h>
#include "antares/solver/variable/variable.h"

namespace Antares::Solver::Variable::Economy
{

// Base traits for time series variables
template<typename Derived>
struct TimeSeriesTraits
{
    static constexpr std::string_view kUnit = "MWh";

    // Common VCard properties
    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::id
                                                    | Category::FileLevel::va);
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

// Base VCard template
template<typename TraitsType>
struct VCardTimeSeriesBase: public TimeSeriesTraits<TraitsType>
{
    using BaseType = TimeSeriesTraits<TraitsType>;
    using VCardForSpatialAggregate = VCardTimeSeriesBase<TraitsType>;

    // Modern API using string_view
    inline static constexpr std::string_view kCaption = TraitsType::kCaption;
    inline static constexpr std::string_view kUnit = BaseType::kUnit;
    inline static constexpr std::string_view kDescription = TraitsType::kDescription;

    // Legacy API (kept for compatibility)
    static std::string Caption()
    {
        return std::string(kCaption);
    }

    static std::string Unit()
    {
        return std::string(kUnit);
    }

    static std::string Description()
    {
        return std::string(kDescription);
    }

    using ResultsType = typename BaseType::ResultsType;
    using IntermediateValuesBaseType = typename BaseType::IntermediateValuesBaseType;
    using IntermediateValuesType = typename BaseType::IntermediateValuesType;
    using IntermediateValuesTypeForSpatialAg = typename BaseType::
      IntermediateValuesTypeForSpatialAg;
};

// Helper meta for statistics
namespace detail
{
template<class VCard, class Next, int CDataLevel, int CFile>
inline constexpr int StatisticsCount = ((VCard::categoryDataLevel & CDataLevel
                                         && VCard::categoryFileLevel & CFile)
                                          ? (Next::template Statistics<CDataLevel, CFile>::count
                                             + VCard::columnCount * VCard::ResultsType::count)
                                          : Next::template Statistics<CDataLevel, CFile>::count);
}

// Base implementation for time series variables
template<typename Derived, typename NextT, typename VCardType>
class TimeSeriesValuesBase: public Variable::IVariable<Derived, NextT, VCardType>
{
public:
    using NextType = NextT;
    using AncestorType = Variable::IVariable<Derived, NextT, VCardType>;
    using ResultsType = typename VCardType::ResultsType;
    using VariableAccessorType = VariableAccessor<ResultsType, VCardType::columnCount>;

    static constexpr int count = 1 + NextT::count;

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        static constexpr int count = detail::
          StatisticsCount<VCardType, NextType, CDataLevel, CFile>;
    };

    void initializeFromStudy(Data::Study& study)
    {
        nbYearsParallel = study.maxNbYearsInParallel;

        InitializeResultsFromStudy(AncestorType::pResults, study);

        yearlyValues.resize(nbYearsParallel);
        std::for_each(yearlyValues.begin(),
                      yearlyValues.end(),
                      [&study](auto& value) { value.initializeFromStudy(study); });

        static_cast<Derived*>(this)->initializeDerivedFromStudy(study);
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
        std::for_each(yearlyValues.begin(), yearlyValues.end(), [](auto& value) { value.reset(); });
        NextType::simulationBegin();
    }

    void simulationEnd()
    {
        NextType::simulationEnd();
    }

    void yearBegin(unsigned int year, unsigned int space)
    {
        static_cast<Derived*>(this)->yearBeginImpl(year, space);
        NextType::yearBegin(year, space);
    }

    void yearEndBuild(State& state, unsigned int year, unsigned int space)
    {
        NextType::yearEndBuild(state, year, space);
    }

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

    void hourBegin(unsigned int hourInTheYear)
    {
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int space)
    {
        static_cast<Derived*>(this)->hourForEachAreaImpl(state, space);
        NextType::hourForEachArea(state, space);
    }

    void hourEnd(State& state, unsigned int hourInTheYear)
    {
        NextType::hourEnd(state, hourInTheYear);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int,
      unsigned int space) const noexcept
    {
        return yearlyValues[space].hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      unsigned int space) const
    {
        results.isCurrentVarNA = AncestorType::isNonApplicable;
        if (AncestorType::isPrinted[0])
        {
            results.variableCaption = VCardType::kCaption.data();
            results.variableUnit = std::string(VCardType::kUnit);
            yearlyValues[space].template buildAnnualSurveyReport<VCardType>(results,
                                                                            fileLevel,
                                                                            precision);
        }
    }

protected:
    Data::Area* areaPtr{nullptr};
    typename VCardType::IntermediateValuesType yearlyValues;
    unsigned int nbYearsParallel{0};
};

} // namespace Antares::Solver::Variable::Economy
