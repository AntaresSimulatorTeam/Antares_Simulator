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

#include <type_traits>

// Traits de catégorie de variable
namespace Antares::Solver::Variable::detail
{

template<class VCardT>
struct variable_category_traits
{
    static constexpr bool is_single = (VCardT::columnCount == Category::singleColumn);
    static constexpr bool is_dynamic = (VCardT::columnCount == Category::dynamicColumns);
    static constexpr bool is_multiple = (VCardT::columnCount > 1)
                                        && !is_dynamic; // plusieurs colonnes statiques
    static constexpr int effective_column_count = is_multiple ? VCardT::columnCount
                                                              : 1; // dynamique ou single => 1 pour
                                                                   // itérations internes
};

} // namespace Antares::Solver::Variable::detail

#include <yuni/core/static/types.h>

#include <antares/study/variable-print-info.h>

namespace Antares::Solver::Variable
{

template<class ChildT, class NextT, class VCardT>
inline IVariable<ChildT, NextT, VCardT>::IVariable()
{
    // Initialization
    // You should prefer the methods initializeFromStudy() or similiar
    // to initialize the internal variables

    // Number of column, where dimension -1 (dynamic) is avoided
    pColumnCount = VCardType::columnCount > 1 ? VCardType::columnCount : 1;

    // Allocation
    // Does current output variable appear non applicable in all output reports (of any kind :
    // area or district reports, annual or over all years reports, digest, ...) ?
    isNonApplicable = new bool[pColumnCount];
    // Does current output variable column(s) appear in all reports ?
    isPrinted = new bool[pColumnCount];

    // Initializing default print to true
    for (uint i = 0; i < pColumnCount; i++)
    {
        isPrinted[i] = true;
    }
}

template<class ChildT, class NextT, class VCardT>
inline IVariable<ChildT, NextT, VCardT>::~IVariable()
{
    delete[] isNonApplicable;
    delete[] isPrinted;
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::initializeFromStudy(Data::Study& study)
{
    // Next
    NextType::initializeFromStudy(study);
}

template<class ChildT, class NextT, class VCardT>
template<class R>
inline void IVariable<ChildT, NextT, VCardT>::InitializeResultsFromStudy(R& results,
                                                                         Data::Study& study)
{
    VariableAccessorType::InitializeAndReset(results, study);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::initializeFromArea(Data::Study* study,
                                                                 Data::Area* area)
{
    // Next
    NextType::initializeFromArea(study, area);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::initializeFromLink(Data::Study* study,
                                                                 Data::AreaLink* link)
{
    // Next
    NextType::initializeFromAreaLink(study, link);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::initializeFromThermalCluster(
  Data::Study* study,
  Data::Area* area,
  Data::ThermalCluster* cluster)
{
    // Next
    NextType::initializeFromThermalCluster(study, area, cluster);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::broadcastNonApplicability(bool applyNonApplicable)
{
    if (VCardType::isPossiblyNonApplicable != 0 && applyNonApplicable)
    {
        for (uint i = 0; i != pColumnCount; ++i)
        {
            isNonApplicable[i] = true;
        }
    }
    else
    {
        for (uint i = 0; i != pColumnCount; ++i)
        {
            isNonApplicable[i] = false;
        }
    }
    NextType::broadcastNonApplicability(applyNonApplicable);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::simulationBegin()
{
    // Next
    NextType::simulationBegin();
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::simulationEnd()
{
    // Next
    NextType::simulationEnd();
}

template<class ChildT, class NextT, class VCardT>
[[nodiscard]] size_t IVariable<ChildT, NextT, VCardT>::getMaxNumberColumns() const noexcept
{
    return VCardT::ResultsType::count;
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::yearBegin(uint year)
{
    // Next variable
    NextType::yearBegin(year);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::yearEnd(uint year)
{
    // Next variable
    NextType::yearEnd(year);
}

template<class ChildT, class NextT, class VCardT>
template<class V>
inline void IVariable<ChildT, NextT, VCardT>::yearEndSpatialAggregates(V& allVars,
                                                                       uint year,
                                                                       unsigned int numSpace)
{
    // Next variable
    NextType::yearEndSpatialAggregates(allVars, year, numSpace);
}

template<class ChildT, class NextT, class VCardT>
template<class V, class SetT>
inline void IVariable<ChildT, NextT, VCardT>::yearEndSpatialAggregates(V& allVars,
                                                                       uint year,
                                                                       const SetT& set)
{
    // Next variable
    NextType::yearEndSpatialAggregates(allVars, year, set);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::yearEndBuildPrepareDataForEachThermalCluster(
  State& state,
  uint year,
  uint numSpace)
{
    // Next variable
    NextType::yearEndBuildPrepareDataForEachThermalCluster(state, year, numSpace);
}

template<class ChildT, class NextT, class VCardT>
template<class V>
inline void IVariable<ChildT, NextT, VCardT>::simulationEndSpatialAggregates(V& allVars)
{
    // Next variable
    NextType::simulationEndSpatialAggregates(allVars);
}

template<class ChildT, class NextT, class VCardT>
template<class V, class SetT>
inline void IVariable<ChildT, NextT, VCardT>::simulationEndSpatialAggregates(V& allVars,
                                                                             const SetT& set)
{
    // Next variable
    NextType::simulationEndSpatialAggregates(allVars, set);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::weekBegin(State& state)
{
    // Next variable
    NextType::weekBegin(state);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::weekForEachArea(State& state, unsigned int numSpace)
{
    // Next variable
    NextType::weekForEachArea(state, numSpace);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::hourBegin(uint hourInTheYear)
{
    // Next variable
    NextType::hourBegin(hourInTheYear);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::hourForEachArea(State& state)
{
    // Next variable
    NextType::hourForEachArea(state);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::hourForEachArea(State& state, unsigned int numSpace)
{
    // Next variable
    NextType::hourForEachArea(state, numSpace);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::yearEndBuildForEachThermalCluster(
  State& state,
  unsigned int year,
  unsigned int numSpace)
{
    // Next item in the list
    NextType::yearEndBuildForEachThermalCluster(state, year, numSpace);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::hourForEachLink(State& state, unsigned int numSpace)
{
    // Next item in the list
    NextType::hourForEachLink(state, numSpace);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::hourEnd(State& state, uint hourInTheYear)
{
    // Next
    NextType::hourEnd(state, hourInTheYear);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::weekEnd(State& state)
{
    // Next
    NextType::weekEnd(state);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::buildSurveyReport(SurveyResults& results,
                                                                int dataLevel,
                                                                int fileLevel,
                                                                int precision) const
{
    // Generating value for the area
    // Only if there are some results to export...
    if (0 != ResultsType::count)
    {
        // And only if we match the current data level _and_ precision level
        if ((dataLevel & VCardType::categoryDataLevel) && (fileLevel & VCardType::categoryFileLevel)
            && (precision & VCardType::precision))
        {
            // Initializing pointer on variable non applicable and print stati arrays to beginning
            results.isPrinted = isPrinted;
            results.isCurrentVarNA = isNonApplicable;

            VariableAccessorType::template BuildSurveyReport<VCardType>(results,
                                                                        pResults,
                                                                        dataLevel,
                                                                        fileLevel,
                                                                        precision);
        }
    }

    // Ask to the next item in the static list to export
    // its results as well
    NextType::buildSurveyReport(results, dataLevel, fileLevel, precision);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::buildAnnualSurveyReport(SurveyResults& results,
                                                                      int dataLevel,
                                                                      int fileLevel,
                                                                      int precision,
                                                                      uint numSpace) const
{
    // Generating value for the area
    // Only if there are some results to export...
    if (0 != ResultsType::count)
    {
        // And only if we match the current data level _and_ precision level
        if ((dataLevel & VCardType::categoryDataLevel) && (fileLevel & VCardType::categoryFileLevel)
            && (precision & VCardType::precision))
        {
            // Getting its intermediate results
            static_cast<const ChildT*>(this)->localBuildAnnualSurveyReport(results,
                                                                           fileLevel,
                                                                           precision,
                                                                           numSpace);
        }
    }

    // Ask to the next item in the static list to export
    // its results as well
    NextType::buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::buildDigest(SurveyResults& results,
                                                          int digestLevel,
                                                          int dataLevel) const
{
    // Generate the Digest for the local results (areas part)
    if (VCardType::columnCount != 0
        && (VCardType::categoryDataLevel & Category::DataLevel::setOfAreas
            || VCardType::categoryDataLevel & Category::DataLevel::area
            || VCardType::categoryDataLevel & Category::DataLevel::link))
    {
        // Initializing pointer on variable non applicable and print stati arrays to beginning
        results.isPrinted = isPrinted;
        results.isCurrentVarNA = isNonApplicable;

        VariableAccessorType::template BuildDigest<VCardT>(results,
                                                           pResults,
                                                           digestLevel,
                                                           dataLevel);
    }
    // Ask to build the digest to the next variable
    NextType::buildDigest(results, digestLevel, dataLevel);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::beforeYearByYearExport(uint year, uint numspace)
{
    NextType::beforeYearByYearExport(year, numspace);
}

template<class ChildT, class NextT, class VCardT>
template<class I>
inline void IVariable<ChildT, NextT, VCardT>::provideInformations(I& infos)
{
    // Begining of the node
    if (VCardType::nodeDepthForGUI)
    {
        infos.template beginNode<VCardType>();
        // Next variable in the list
        NextType::template provideInformations<I>(infos);
        // End of the node
        infos.endNode();
    }
    else
    {
        // Giving our VCard
        infos.template addVCard<VCardType>();
        // Next variable in the list
        NextType::template provideInformations<I>(infos);
    }
}

template<class ChildT, class NextT, class VCardT>
template<class SearchVCardT, class O>
inline void IVariable<ChildT, NextT, VCardT>::computeSpatialAggregateWith(O& out, uint numSpace)
{
    // if this variable has the vcard we are looking for,
    // then we will add our results
    // In the most cases, the variable `out` is intermediate results.

    if (Yuni::Static::Type::StrictlyEqual<VCardT, SearchVCardT>::Yes)
    {
        SpatialAggregateOperation<
          Yuni::Static::Type::StrictlyEqual<VCardT, SearchVCardT>::Yes, // To avoid instanciation
          VCardT::spatialAggregate, // The spatial cluster operation to perform
          VCardType                 // The VCard
          >::Perform(out, *(static_cast<ChildT*>(this)), numSpace);
        return;
    }
    // Otherwise we keep looking
    NextType::template computeSpatialAggregateWith<SearchVCardT, O>(out, numSpace);
}

template<class ChildT, class NextT, class VCardT>
template<class SearchVCardT, class O>
inline void IVariable<ChildT, NextT, VCardT>::computeSpatialAggregateWith(O& out,
                                                                          const Data::Area* area)
{
    NextType::template computeSpatialAggregateWith<SearchVCardT, O>(out, area);
}

namespace // anonymous
{
template<int Match>
struct RetrieveResultsAssignment
{
    enum
    {
        Yes = 1
    };

    template<class ResultsT, class O>
    static void Do(ResultsT& varResults, O** result)
    {
        *result = &varResults;
    }
};

template<>
struct RetrieveResultsAssignment<0>
{
    enum
    {
        Yes = 0
    };

    template<class ResultsT, class O>
    static void Do(ResultsT&, O**)
    {
        // Do nothing
    }
};

} // namespace

template<class ChildT, class NextT, class VCardT>
template<class VCardToFindT>
inline const double* IVariable<ChildT, NextT, VCardT>::retrieveHourlyResultsForCurrentYear(
  uint numSpace) const
{
    using AssignT = RetrieveResultsAssignment<
      Yuni::Static::Type::StrictlyEqual<VCardT, VCardToFindT>::Yes>;
    return (AssignT::Yes)
             ? nullptr
             : NextType::template retrieveHourlyResultsForCurrentYear<VCardToFindT>(numSpace);
}

template<class ChildT, class NextT, class VCardT>
template<class VCardToFindT>
inline void IVariable<ChildT, NextT, VCardT>::retrieveResultsForArea(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::Area* area)
{
    using AssignT = RetrieveResultsAssignment<
      Yuni::Static::Type::StrictlyEqual<VCardT, VCardToFindT>::Yes>;
    AssignT::Do(pResults, result);
    if (!AssignT::Yes)
    {
        NextType::template retrieveResultsForArea<VCardToFindT>(result, area);
    }
}

template<class ChildT, class NextT, class VCardT>
template<class VCardToFindT>
inline void IVariable<ChildT, NextT, VCardT>::retrieveResultsForThermalCluster(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::ThermalCluster* cluster)
{
    using AssignT = RetrieveResultsAssignment<
      Yuni::Static::Type::StrictlyEqual<VCardT, VCardToFindT>::Yes>;
    AssignT::Do(pResults, result);
    if (!AssignT::Yes)
    {
        NextType::template retrieveResultsForThermalCluster<VCardToFindT>(result, cluster);
    }
}

template<class ChildT, class NextT, class VCardT>
template<class VCardToFindT>
inline void IVariable<ChildT, NextT, VCardT>::retrieveResultsForLink(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::AreaLink* link)
{
    using AssignT = RetrieveResultsAssignment<
      Yuni::Static::Type::StrictlyEqual<VCardT, VCardToFindT>::Yes>;
    AssignT::Do(pResults, result);
    if (!AssignT::Yes)
    {
        NextType::template retrieveResultsForLink<VCardToFindT>(result, link);
    }
}

namespace // anonymous
{
template<int ColumnT>
struct HourlyResultsForCurrentYear
{
    template<class R>
    static Antares::Memory::Stored<double>::ConstReturnType Get(const R& results, uint column)
    {
        return results[column].hourlyValuesForSpatialAggregate();
    }
};

template<>
struct HourlyResultsForCurrentYear<Category::singleColumn>
{
    template<class R>
    static Antares::Memory::Stored<double>::ConstReturnType Get(const R& results, uint)
    {
        return results.hourlyValuesForSpatialAggregate();
    }
};

template<>
struct HourlyResultsForCurrentYear<Category::noColumn>
{
    template<class R>
    static Antares::Memory::Stored<double>::ConstReturnType Get(const R&, uint)
    {
        return Antares::Memory::Stored<double>::NullValue();
    }
};

} // anonymous namespace

template<class ChildT, class NextT, class VCardT>
inline Antares::Memory::Stored<double>::ConstReturnType
IVariable<ChildT, NextT, VCardT>::retrieveRawHourlyValuesForCurrentYear(uint column,
                                                                        uint /* numSpace */) const
{
    return HourlyResultsForCurrentYear<VCardType::columnCount>::Get(pResults, column);
}

template<class ChildT, class NextT, class VCardT>
inline const typename Storage<VCardT>::ResultsType& IVariable<ChildT, NextT, VCardT>::results()
  const noexcept
{
    return pResults;
}

// ===================================================================
// Each output variable gets registered in the print info collector
// ===================================================================

// Helper générique pour itérer sur les captions (single, dynamic, multiple)
namespace // anonymous
{
using Antares::Solver::Variable::detail::variable_category_traits;

template<class VCardT, class F>
inline void for_each_column_caption(F&& f)
{
    if constexpr (variable_category_traits<VCardT>::is_multiple)
    {
        for (int i = 0; i < VCardT::columnCount; ++i)
        {
            f(i, VCardT::Multiple::Caption(i));
        }
    }
    else
    {
        f(0, VCardT::Caption());
    }
}
} // anonymous namespace

// Suppression de RetrieveVariableListHelper : unification via for_each_column_caption
template<class ChildT, class NextT, class VCardT>
template<class PredicateT>
void IVariable<ChildT, NextT, VCardT>::RetrieveVariableList(PredicateT& predicate)
{
    using DecayedPred = std::decay_t<PredicateT>;
    using Traits = detail::variable_category_traits<VCardType>;
    if constexpr (std::is_same_v<DecayedPred, Data::variablePrintInfoCollector>)
    {
        for_each_column_caption<VCardType>(
          [&](int /*idx*/, const auto& caption)
          { predicate.add(caption, VCardT::categoryDataLevel, VCardT::categoryFileLevel); });
    }
    else
    {
        if constexpr (!Traits::is_dynamic)
        {
            for_each_column_caption<VCardType>(
              [&](int idx, const auto& caption)
              {
                  if constexpr (Traits::is_single)
                  {
                      predicate.add(caption, VCardT::Unit(), VCardT::Description());
                  }
                  else
                  {
                      predicate.add(caption, VCardT::Multiple::Unit(idx), VCardT::Description());
                  }
              });
        }
    }
    NextType::RetrieveVariableList(predicate);
}

// =============================================================================
// Each output variable gets its print status from the study parameters
// =============================================================================

// (Définition unique de for_each_column_caption déjà fournie plus haut)

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::getPrintStatusFromStudy(Data::Study& study)
{
    auto& vpi = study.parameters.variablesPrintInfo;
    for_each_column_caption<VCardType>([&](int idx, const auto& caption)
                                       { isPrinted[idx] = vpi.isPrinted(caption); });
    NextType::getPrintStatusFromStudy(study);
}

// =======================================================================
// Each output variable supplies the maximum number of columns it takes
// in an ouptut report to the variable print info instance
// =======================================================================

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::supplyMaxNumberOfColumns(Data::Study& study)
{
    auto max_columns = static_cast<const ChildT*>(this)->getMaxNumberColumns();
    auto& vpi = study.parameters.variablesPrintInfo;
    for_each_column_caption<VCardType>(
      [&](int /*idx*/, const auto& caption)
      { vpi.setMaxColumns(caption, static_cast<uint>(max_columns)); });
    NextType::supplyMaxNumberOfColumns(study);
}

} // namespace Antares::Solver::Variable
