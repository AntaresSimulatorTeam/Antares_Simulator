// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_LIST_H__
#define __SOLVER_VARIABLE_LIST_H__

#include <vector>

#include <yuni/yuni.h>
#include <yuni/core/static/types.h>
#include <yuni/core/string.h>

#include <antares/logs/logs.h>
#include <antares/solver/variable/dynamicAggregation/dynamicAggregation.h>

#include "categories.h"
#include "info.h"
#include "state.h"
#include "surveyresults.h"

namespace Antares::Solver::Variable::Container
{
/*!
** \brief Top-level container for all output variables
**
** Owns the inner aggregator (a JoinAll<…> of scope wrappers) by composition
** and forwards every hook to it. Adds dynamic-aggregation bookkeeping that
** isn't part of the inner aggregator's responsibilities.
*/
template<class Inner>
class List
{
public:
    //! The full type of the class
    using ListType = List<Inner>;

    static constexpr std::size_t count = Inner::count;

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        static constexpr int count = Inner::template Statistics<CDataLevel, CFile>::count;
    };

    template<class PredicateT>
    static void RetrieveVariableList(PredicateT& predicate)
    {
        Inner::RetrieveVariableList(predicate);
    }

public:
    //! \name Variable initialization
    //@{
    void initializeFromStudy(Data::Study& study);
    void initializeFromArea(Data::Study* study, Data::Area* area);
    void initializeFromLink(Data::Study* study, Data::AreaLink* link);
    //@}

    //! \name Simulation events
    //@{
    void simulationBegin();
    void simulationEnd();
    //@}

    //! \name Years events
    //@{
    void yearBegin(unsigned int year, unsigned int numSpace);

    void buildThermalClusterYearEndResults(State& state, unsigned int year, unsigned int numSpace);

    void yearEnd(unsigned int year, unsigned int numSpace);

    void computeSummary(unsigned int year, unsigned int numSpace);

    template<class V>
    void yearEndSpatialAggregates(V& allVars, unsigned int year, unsigned int numSpace);

    template<class V, class SetT>
    void yearEndSpatialAggregates(V& allVars, unsigned int year, const SetT& set);

    template<class V>
    void computeSpatialAggregatesSummary(V& allVars, unsigned int year, unsigned int numSpace);

    template<class V>
    void simulationEndSpatialAggregates(V& allVars);

    template<class V, class SetT>
    void simulationEndSpatialAggregates(V& allVars, const SetT& set);

    void beforeYearByYearExport(unsigned int year, unsigned int numSpace);
    //@}

    //! \name Hourly events
    //@{
    void hourBegin(unsigned int hourInTheYear);
    void hourForEachArea(State& state, unsigned int numSpace);
    void hourForEachLink(State& state);
    void hourEnd(State& state, unsigned int hourInTheYear);
    //@}

    //! \name Weekly events
    //@{
    void weekBegin(State& state);
    void weekForEachArea(State& state, unsigned int numSpace);
    void weekEnd(State& state);
    //@}

    //! \name Spatial aggregation
    //@{
    template<class SearchVCardT, class O>
    void computeSpatialAggregateWith(O& out);

    template<class SearchVCardT, class O>
    void computeSpatialAggregateWith(O& out, const Data::Area* area, unsigned int numSpace);

    template<class VCardToFindT>
    void retrieveResultsForArea(typename Variable::Storage<VCardToFindT>::ResultsType** result,
                                const Data::Area* area);

    template<class VCardToFindT>
    void retrieveResultsForThermalCluster(
      typename Variable::Storage<VCardToFindT>::ResultsType** result,
      const Data::ThermalCluster* cluster);

    template<class VCardToFindT>
    void retrieveResultsForLink(typename Variable::Storage<VCardToFindT>::ResultsType** result,
                                const Data::AreaLink* link);
    //@}

    //! \name User reports
    //@{
    void buildSurveyReport(SurveyResults& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const;

    void buildAnnualSurveyReport(SurveyResults& results,
                                 int dataLevel,
                                 int fileLevel,
                                 int precision,
                                 unsigned int numSpace) const;

    void exportSurveyResults(bool global,
                             const Yuni::String& output,
                             unsigned int numSpace,
                             IResultWriter& writer);

    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const;
    //@}

private:
    //! Inner aggregator (held by composition).
    Inner next_;

    //! Pointer to the current study
    Data::Study* pStudy;

    // Allow delayed init
    std::unique_ptr<Solver::Variable::DynamicAggregationAllYears> dynamicAggregationAllYears_;
    std::vector<Solver::Variable::DynamicAggregationSingleYear> dynamicAggregationSingleYear_;

}; // class List

} // namespace Antares::Solver::Variable::Container

#include "container.hxx"
#include "surveyresults/reportbuilder.hxx"

#endif // __SOLVER_VARIABLE_LIST_H__
