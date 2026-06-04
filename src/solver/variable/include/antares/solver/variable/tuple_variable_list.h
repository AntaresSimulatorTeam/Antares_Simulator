// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef SOLVER_VARIABLE_TUPLE_VARIABLE_LIST_H__
#define SOLVER_VARIABLE_TUPLE_VARIABLE_LIST_H__

// Flat variable list. Stores each leaf standalone and aggregates by folding
// `std::apply` over the tuple. Used as the per-element list inside scope
// wrappers (`Areas`, `SetsOfAreas`, `BindingConstraints`).
//
// Hook semantics fall into four classes:
//   (1) broadcast            — unconditional fold:  (v.hook(a...), ...);
//   (2) first-match short-ckt — find-and-stop:       ((Match<V> ? (do(v), true) : false) || ...);
//   (3) templated broadcast   — static hooks taking a predicate/collector
//   (4) compile-time fold     — Statistics<CD,CF>::count via constexpr (+ ...)

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include <antares/study/study.h>

#include "info.h" // for Variable::Storage<VCardT>
#include "state.h"
#include "surveyresults.h"

namespace Antares::Solver::Variable::Container
{

namespace detail
{
// Drop-in type-match predicate for search hooks. Mirrors the original
// Yuni::Static::Type::StrictlyEqual<VCardT, Target>::Yes usage.
template<class VCardT, class TargetT>
inline constexpr bool isSameVCard = std::is_same_v<VCardT, TargetT>;
} // namespace detail

/*!
** \brief Flat storage + fold dispatch for a list of output variables.
**
** Each variable is held as an independent member; aggregation happens here
** via fold expressions over `std::apply`.
*/
template<class... Vars>
class TupleVariableList
{
public:
    //! Number of variables in this list
    static constexpr std::size_t count = sizeof...(Vars);

    /*!
    ** \brief Compile-time column count filtered by (dataLevel, fileLevel).
    **
    ** Each Vi exposes `Statistics<CD, CF>::count` (nested struct). We fold over
    ** the pack to sum.
    */
    template<int CDataLevel, int CFile>
    struct Statistics
    {
        static constexpr int count = (0 + ...
                                      + Vars::template Statistics<CDataLevel, CFile>::count);
    };

    // ------------------------------------------------------------------
    // (1) Broadcast hooks — unconditional fold over every variable.
    // ------------------------------------------------------------------

    void initializeFromStudy(Data::Study& study)
    {
        std::apply([&](auto&... v) { (v.initializeFromStudy(study), ...); }, vars_);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        std::apply([&](auto&... v) { (v.initializeFromArea(study, area), ...); }, vars_);
    }

    void initializeFromLink(Data::Study* study, Data::AreaLink* link)
    {
        std::apply([&](auto&... v) { (v.initializeFromLink(study, link), ...); }, vars_);
    }

    void broadcastNonApplicability(bool applyNonApplicable)
    {
        std::apply([&](auto&... v) { (v.broadcastNonApplicability(applyNonApplicable), ...); },
                   vars_);
    }

    void getPrintStatusFromStudy(Data::Study& study)
    {
        std::apply([&](auto&... v) { (v.getPrintStatusFromStudy(study), ...); }, vars_);
    }

    void supplyMaxNumberOfColumns(Data::Study& study)
    {
        std::apply([&](auto&... v) { (v.supplyMaxNumberOfColumns(study), ...); }, vars_);
    }

    void simulationBegin()
    {
        std::apply([](auto&... v) { (v.simulationBegin(), ...); }, vars_);
    }

    void simulationEnd()
    {
        std::apply([](auto&... v) { (v.simulationEnd(), ...); }, vars_);
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        std::apply([&](auto&... v) { (v.yearBegin(year, numSpace), ...); }, vars_);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        std::apply([&](auto&... v) { (v.yearEnd(year, numSpace), ...); }, vars_);
    }

    void yearEndBuildPrepareDataForEachThermalCluster(State& state,
                                                      unsigned int year,
                                                      unsigned int numSpace)
    {
        std::apply(
          [&](auto&... v)
          { (v.yearEndBuildPrepareDataForEachThermalCluster(state, year, numSpace), ...); },
          vars_);
    }

    void yearEndBuildForEachThermalCluster(State& state, unsigned int year, unsigned int numSpace)
    {
        std::apply([&](auto&... v)
                   { (v.yearEndBuildForEachThermalCluster(state, year, numSpace), ...); },
                   vars_);
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        std::apply([&](auto&... v) { (v.computeSummary(year, numSpace), ...); }, vars_);
    }

    void weekBegin(State& state)
    {
        std::apply([&](auto&... v) { (v.weekBegin(state), ...); }, vars_);
    }

    void weekForEachArea(State& state, unsigned int numSpace)
    {
        std::apply([&](auto&... v) { (v.weekForEachArea(state, numSpace), ...); }, vars_);
    }

    void weekEnd(State& state)
    {
        std::apply([&](auto&... v) { (v.weekEnd(state), ...); }, vars_);
    }

    void hourBegin(unsigned int hourInTheYear)
    {
        std::apply([&](auto&... v) { (v.hourBegin(hourInTheYear), ...); }, vars_);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        std::apply([&](auto&... v) { (v.hourForEachArea(state, numSpace), ...); }, vars_);
    }

    void hourForEachLink(State& state, unsigned int numSpace)
    {
        std::apply([&](auto&... v) { (v.hourForEachLink(state, numSpace), ...); }, vars_);
    }

    void hourEnd(State& state, unsigned int hourInTheYear)
    {
        std::apply([&](auto&... v) { (v.hourEnd(state, hourInTheYear), ...); }, vars_);
    }

    void beforeYearByYearExport(unsigned int year, unsigned int numSpace)
    {
        std::apply([&](auto&... v) { (v.beforeYearByYearExport(year, numSpace), ...); }, vars_);
    }

    // Reports — each leaf filters internally on (dataLevel, fileLevel, precision)
    // so the tuple just broadcasts.
    void buildSurveyReport(SurveyResults& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const
    {
        std::apply([&](const auto&... v)
                   { (v.buildSurveyReport(results, dataLevel, fileLevel, precision), ...); },
                   vars_);
    }

    void buildAnnualSurveyReport(SurveyResults& results,
                                 int dataLevel,
                                 int fileLevel,
                                 int precision,
                                 unsigned int numSpace) const
    {
        std::apply(
          [&](const auto&... v)
          { (v.buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace), ...); },
          vars_);
    }

    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
    {
        std::apply([&](const auto&... v) { (v.buildDigest(results, digestLevel, dataLevel), ...); },
                   vars_);
    }

    // ------------------------------------------------------------------
    // (2) Search hooks — first-match short-circuit fold via `|| ...`.
    // ------------------------------------------------------------------

    template<class SearchVCardT, class O>
    void computeSpatialAggregateWith(O& out, unsigned int numSpace)
    {
        std::apply([&](auto&... v) { ((tryAggregate<SearchVCardT>(v, out, numSpace)) || ...); },
                   vars_);
    }

    template<class SearchVCardT, class O>
    void computeSpatialAggregateWith(O& out, const Data::Area* area)
    {
        // No-op-at-leaf semantic: nothing to do in the tuple.
        (void)out;
        (void)area;
    }

    template<class VCardToFindT>
    void retrieveResultsForArea(typename Storage<VCardToFindT>::ResultsType** result,
                                const Data::Area* area)
    {
        std::apply([&](auto&... v) { ((tryAssignArea<VCardToFindT>(v, result, area)) || ...); },
                   vars_);
    }

    template<class VCardToFindT>
    void retrieveResultsForThermalCluster(typename Storage<VCardToFindT>::ResultsType** result,
                                          const Data::ThermalCluster* cluster)
    {
        std::apply([&](auto&... v)
                   { ((tryAssignCluster<VCardToFindT>(v, result, cluster)) || ...); },
                   vars_);
    }

    template<class VCardToFindT>
    void retrieveResultsForLink(typename Storage<VCardToFindT>::ResultsType** result,
                                const Data::AreaLink* link)
    {
        std::apply([&](auto&... v) { ((tryAssignLink<VCardToFindT>(v, result, link)) || ...); },
                   vars_);
    }

    // ------------------------------------------------------------------
    // (3) Static templated broadcast hooks.
    // ------------------------------------------------------------------

    template<class PredicateT>
    static void RetrieveVariableList(PredicateT& predicate)
    {
        (Vars::RetrieveVariableList(predicate), ...);
    }

    template<class V>
    void yearEndSpatialAggregates(V& allVars, unsigned int year, unsigned int numSpace)
    {
        std::apply([&](auto&... v) { (v.yearEndSpatialAggregates(allVars, year, numSpace), ...); },
                   vars_);
    }

    template<class V, class SetT>
    void yearEndSpatialAggregates(V& allVars, unsigned int year, const SetT& set)
    {
        std::apply([&](auto&... v) { (v.yearEndSpatialAggregates(allVars, year, set), ...); },
                   vars_);
    }

    // 4-arg variant — only SpatialAggregate leaves expose it. setofareas.hxx:269
    // dispatches through the tuple of SpatialAggregate wrappers.
    template<class V, class SetT>
    void yearEndSpatialAggregates(V& allVars,
                                  unsigned int year,
                                  const SetT& set,
                                  unsigned int numSpace)
    {
        std::apply([&](auto&... v)
                   { (v.yearEndSpatialAggregates(allVars, year, set, numSpace), ...); },
                   vars_);
    }

    // 3-arg SpatialAggregate summary — not on the regular IVariable chain.
    template<class V>
    void computeSpatialAggregatesSummary(V& allVars, unsigned int year, unsigned int numSpace)
    {
        std::apply([&](auto&... v)
                   { (v.computeSpatialAggregatesSummary(allVars, year, numSpace), ...); },
                   vars_);
    }

    template<class V>
    void simulationEndSpatialAggregates(V& allVars)
    {
        std::apply([&](auto&... v) { (v.simulationEndSpatialAggregates(allVars), ...); }, vars_);
    }

    template<class V, class SetT>
    void simulationEndSpatialAggregates(V& allVars, const SetT& set)
    {
        std::apply([&](auto&... v) { (v.simulationEndSpatialAggregates(allVars, set), ...); },
                   vars_);
    }

    // Default constructor: default-constructs every Vi in place.
    TupleVariableList() = default;

private:
    std::tuple<Vars...> vars_;

    // Per-variable search helpers. Each returns `true` to short-circuit the fold.
    //
    // tryAggregate calls SpatialAggregateOperation::Perform directly rather than
    // re-dispatching through v.computeSpatialAggregateWith(...): the re-dispatch
    // path expects a successor signature that no longer exists, and we'd
    // otherwise force instantiation on every leaf.
    template<class Search, class V, class O>
    static bool tryAggregate(V& v, O& out, unsigned int numSpace)
    {
        using VCard = typename V::VCardType;
        if constexpr (detail::isSameVCard<VCard, Search>)
        {
            Variable::SpatialAggregateOperation<true, VCard::spatialAggregate, VCard>::Perform(
              out,
              v,
              numSpace);
            return true;
        }
        else
        {
            return false;
        }
    }

    template<class Target, class V, class R>
    static bool tryAssignArea(V& v, R** result, const Data::Area* area)
    {
        if constexpr (detail::isSameVCard<typename V::VCardType, Target>)
        {
            v.template retrieveResultsForArea<Target>(result, area);
            return true;
        }
        else
        {
            return false;
        }
    }

    template<class Target, class V, class R>
    static bool tryAssignCluster(V& v, R** result, const Data::ThermalCluster* cluster)
    {
        if constexpr (detail::isSameVCard<typename V::VCardType, Target>)
        {
            v.template retrieveResultsForThermalCluster<Target>(result, cluster);
            return true;
        }
        else
        {
            return false;
        }
    }

    // Link results don't always live on a leaf whose own VCardType matches Target:
    // the per-link variables (FlowLinear, MarginalCost, ...) sit inside the `Links`
    // container, whose VCardType is `VCardAllLinks`. A pure VCard-equality gate would
    // skip `Links` entirely and the lookup would silently return nullptr. Instead,
    // dispatch unconditionally — IVariable::retrieveResultsForLink is a no-op when
    // the leaf VCard doesn't match — and short-circuit once *result has been set.
    template<class Target, class V, class R>
    static bool tryAssignLink(V& v, R** result, const Data::AreaLink* link)
    {
        v.template retrieveResultsForLink<Target>(result, link);
        return *result != nullptr;
    }
};

} // namespace Antares::Solver::Variable::Container

#endif // SOLVER_VARIABLE_TUPLE_VARIABLE_LIST_H__
