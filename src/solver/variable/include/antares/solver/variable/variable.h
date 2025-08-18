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

// Remove the visual studio warning: decorated name length exceeded, name was truncated
#ifdef YUNI_OS_MSVC
#pragma warning(disable : 4503)
#endif

#include <concepts>
#include <type_traits>

#include <yuni/yuni.h>
#include <yuni/core/static/if.h>

#include "categories.h"
#include "container.h"
#include "endoflist.h"
#include "info.h"
#include "storage/intermediate.h"
#include "storage/results.h"
#include "surveyresults.h"
#include "variable_traits.h"

namespace Antares::Solver::Variable
{
// Traits de détection optionnels pour NextT
namespace detail
{
template<typename T, typename = void>
struct is_variable_next: std::false_type
{
};

template<typename T>
struct is_variable_next<T, std::void_t<decltype(T::count), typename T::template Statistics<0, 0>>>
    : std::true_type
{
};
} // namespace detail

template<typename T>
concept VariableNextLike = std::is_same_v<T, Yuni::Default> || detail::is_variable_next<T>::value;

/*! Interface for any variable */
template<class ChildT, VariableNextLike NextT, class VCardT>
class IVariable: protected NextT
{
public:
    using ChildType = ChildT;
    using NextType = NextT;
    using VariableType = IVariable<ChildT, NextT, VCardT>;
    using VCardType = VCardT;
    using ResultsType = typename VCardType::ResultsType;
    using VariableAccessorType = VariableAccessor<ResultsType, VCardType::columnCount>;
    using StoredResultType = typename Storage<VCardT>::ResultsType;

    static constexpr uint8_t categoryDataLevel = VCardType::categoryDataLevel;
    static constexpr uint8_t categoryFileLevel = VCardType::categoryFileLevel;
    static constexpr int effective_column_count = detail::variable_category_traits<
      VCardType>::effective_column_count;
    static constexpr bool is_multiple = detail::variable_category_traits<VCardType>::is_multiple;
    static constexpr bool is_dynamic = detail::variable_category_traits<VCardType>::is_dynamic;
    static constexpr bool is_single = detail::variable_category_traits<VCardType>::is_single;

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        enum
        {
            count = ((categoryDataLevel & CDataLevel && categoryFileLevel & CFile)
                       ? (NextType::template Statistics<CDataLevel, CFile>::count
                          + ResultsType::count)
                       : NextType::template Statistics<CDataLevel, CFile>::count),
        };
    };

public:
    template<class R>
    static void InitializeResultsFromStudy(R& results, Data::Study& study);

    template<class PredicateT>
    static void RetrieveVariableList(PredicateT& predicate);

    void getPrintStatusFromStudy(Data::Study& study);
    void supplyMaxNumberOfColumns(Data::Study& study);

public:
    IVariable();
    ~IVariable();

    void initializeFromStudy(Data::Study& study);

    [[nodiscard]] size_t getMaxNumberColumns() const noexcept;

    void initializeFromArea(Data::Study* study, Data::Area* area);
    void initializeFromLink(Data::Study* study, Data::AreaLink* link);
    void initializeFromThermalCluster(Data::Study* study,
                                      Data::Area* area,
                                      Data::ThermalCluster* cluster);

    void broadcastNonApplicability(bool applyNonApplicable);

    void simulationBegin();
    void simulationEnd();

    void yearBegin(uint year);
    void yearEndBuild(State& state, uint year);
    void yearEndBuildPrepareDataForEachThermalCluster(State& state, uint year, uint numSpace);
    void yearEndBuildForEachThermalCluster(State& state, uint year, uint numSpace);
    void yearEnd(uint year);

    template<class V>
    void yearEndSpatialAggregates(V& allVars, uint year, unsigned int numSpace);

    template<class V, class SetT>
    void yearEndSpatialAggregates(V& allVars, uint year, const SetT& set);

    template<class V>
    void simulationEndSpatialAggregates(V& allVars);

    template<class V, class SetT>
    void simulationEndSpatialAggregates(V& allVars, const SetT& set);

    void hourBegin(uint hourInTheYear);
    void hourForEachArea(State& state);
    void hourForEachArea(State& state, unsigned int numSpace);
    void hourForEachLink(State& state, uint numSpace);
    void hourEnd(State& state, uint hourInTheYear);

    void weekBegin(State& state);
    void weekForEachArea(State& state, uint numSpace);
    void weekEnd(State& state);

    void buildSurveyReport(SurveyResults& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const;
    void buildAnnualSurveyReport(SurveyResults& results,
                                 int dataLevel,
                                 int fileLevel,
                                 int precision,
                                 uint numSpace) const;
    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const;
    void beforeYearByYearExport(uint year, uint numSpace);

    template<class I>
    static void provideInformations(I& infos);

    template<class VCardSearchT, class O>
    void computeSpatialAggregateWith(O& out, uint numSpace);

    template<class VCardSearchT, class O>
    void computeSpatialAggregateWith(O& out, const Data::Area* area);

    template<class VCardToFindT>
    [[nodiscard]] const double* retrieveHourlyResultsForCurrentYear(uint numSpace) const;

    template<class VCardToFindT>
    void retrieveResultsForArea(typename Storage<VCardToFindT>::ResultsType** result,
                                const Data::Area* area);

    template<class VCardToFindT>
    void retrieveResultsForThermalCluster(typename Storage<VCardToFindT>::ResultsType** result,
                                          const Data::ThermalCluster* cluster);

    template<class VCardToFindT>
    void retrieveResultsForLink(typename Storage<VCardToFindT>::ResultsType** result,
                                const Data::AreaLink* link);

    [[nodiscard]] Antares::Memory::Stored<double>::ConstReturnType
    retrieveRawHourlyValuesForCurrentYear(uint column, uint) const;

    [[nodiscard]] const StoredResultType& results() const noexcept;

    [[nodiscard]] static constexpr int EffectiveColumnCount() noexcept
    {
        return effective_column_count;
    }

protected:
    StoredResultType pResults;
    bool* isNonApplicable;
    bool* isPrinted;
    uint pColumnCount;
};

} // namespace Antares::Solver::Variable

#include "variable.hxx"
