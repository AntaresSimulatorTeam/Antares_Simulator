// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_JOIN_ALL_H__
#define __SOLVER_VARIABLE_JOIN_ALL_H__

#include <tuple>

namespace Antares::Solver::Variable
{

template<class... Wrappers>
class JoinAll
{
public:
    static constexpr std::size_t count = (0 + ... + Wrappers::count);

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        static constexpr int count = (0 + ...
                                      + Wrappers::template Statistics<CDataLevel, CFile>::count);
    };

public:
    template<class PredicateT>
    static void RetrieveVariableList(PredicateT& predicate)
    {
        (Wrappers::RetrieveVariableList(predicate), ...);
    }

    void initializeFromStudy(Data::Study& study)
    {
        std::apply([&](auto&... w) { (w.initializeFromStudy(study), ...); }, wrappers_);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        std::apply([&](auto&... w) { (w.initializeFromArea(study, area), ...); }, wrappers_);
    }

    void initializeFromLink(Data::Study* study, Data::AreaLink* link)
    {
        std::apply([&](auto&... w) { (w.initializeFromLink(study, link), ...); }, wrappers_);
    }

    void simulationBegin()
    {
        std::apply([](auto&... w) { (w.simulationBegin(), ...); }, wrappers_);
    }

    void simulationEnd()
    {
        std::apply([](auto&... w) { (w.simulationEnd(), ...); }, wrappers_);
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        std::apply([&](auto&... w) { (w.yearBegin(year, numSpace), ...); }, wrappers_);
    }

    void yearEndBuildPrepareDataForEachThermalCluster(State& state, uint year)
    {
        std::apply([&](auto&... w)
                   { (w.yearEndBuildPrepareDataForEachThermalCluster(state, year), ...); },
                   wrappers_);
    }

    void yearEndBuildForEachThermalCluster(State& state, uint year)
    {
        std::apply([&](auto&... w) { (w.yearEndBuildForEachThermalCluster(state, year), ...); },
                   wrappers_);
    }

    void buildThermalClusterYearEndResults(State& state, unsigned int year, unsigned int numSpace)
    {
        std::apply([&](auto&... w)
                   { (w.buildThermalClusterYearEndResults(state, year, numSpace), ...); },
                   wrappers_);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        std::apply([&](auto&... w) { (w.yearEnd(year, numSpace), ...); }, wrappers_);
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        std::apply([&](auto&... w) { (w.computeSummary(year, numSpace), ...); }, wrappers_);
    }

    void weekBegin(State& state)
    {
        std::apply([&](auto&... w) { (w.weekBegin(state), ...); }, wrappers_);
    }

    void weekForEachArea(State& state, unsigned int numSpace)
    {
        std::apply([&](auto&... w) { (w.weekForEachArea(state, numSpace), ...); }, wrappers_);
    }

    void weekEnd(State& state)
    {
        std::apply([&](auto&... w) { (w.weekEnd(state), ...); }, wrappers_);
    }

    void hourBegin(unsigned int hourInTheYear)
    {
        std::apply([&](auto&... w) { (w.hourBegin(hourInTheYear), ...); }, wrappers_);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        std::apply([&](auto&... w) { (w.hourForEachArea(state, numSpace), ...); }, wrappers_);
    }

    void hourForEachLink(State& state)
    {
        std::apply([&](auto&... w) { (w.hourForEachLink(state), ...); }, wrappers_);
    }

    void hourEnd(State& state, unsigned int hourInTheYear)
    {
        std::apply([&](auto&... w) { (w.hourEnd(state, hourInTheYear), ...); }, wrappers_);
    }

    void buildSurveyReport(SurveyResults& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const
    {
        std::apply([&](const auto&... w)
                   { (w.buildSurveyReport(results, dataLevel, fileLevel, precision), ...); },
                   wrappers_);
    }

    void buildAnnualSurveyReport(SurveyResults& results,
                                 int dataLevel,
                                 int fileLevel,
                                 int precision,
                                 uint numSpace) const
    {
        std::apply(
          [&](const auto&... w)
          { (w.buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace), ...); },
          wrappers_);
    }

    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
    {
        std::apply([&](const auto&... w) { (w.buildDigest(results, digestLevel, dataLevel), ...); },
                   wrappers_);
    }

    void beforeYearByYearExport(uint year, uint numSpace)
    {
        std::apply([&](auto&... w) { (w.beforeYearByYearExport(year, numSpace), ...); }, wrappers_);
    }

    template<class SearchVCardT, class O>
    void computeSpatialAggregateWith(O& out)
    {
        std::apply([&](auto&... w)
                   { (w.template computeSpatialAggregateWith<SearchVCardT, O>(out), ...); },
                   wrappers_);
    }

    template<class SearchVCardT, class O>
    void computeSpatialAggregateWith(O& out, const Data::Area* area, uint numSpace)
    {
        std::apply(
          [&](auto&... w)
          { (w.template computeSpatialAggregateWith<SearchVCardT, O>(out, area, numSpace), ...); },
          wrappers_);
    }

    template<class V>
    void yearEndSpatialAggregates(V& allVars, unsigned int year, unsigned int numSpace)
    {
        std::apply([&](auto&... w) { (w.yearEndSpatialAggregates(allVars, year, numSpace), ...); },
                   wrappers_);
    }

    template<class V>
    void computeSpatialAggregatesSummary(V& allVars, unsigned int year, unsigned int numSpace)
    {
        std::apply([&](auto&... w)
                   { (w.computeSpatialAggregatesSummary(allVars, year, numSpace), ...); },
                   wrappers_);
    }

    template<class V>
    void simulationEndSpatialAggregates(V& allVars)
    {
        std::apply([&](auto&... w) { (w.simulationEndSpatialAggregates(allVars), ...); },
                   wrappers_);
    }

    template<class I>
    static void provideInformations(I& infos)
    {
        (Wrappers::provideInformations(infos), ...);
    }

    template<class VCardToFindT>
    void retrieveResultsForArea(typename Variable::Storage<VCardToFindT>::ResultsType** result,
                                const Data::Area* area)
    {
        std::apply([&](auto&... w)
                   { (w.template retrieveResultsForArea<VCardToFindT>(result, area), ...); },
                   wrappers_);
    }

    template<class VCardToFindT>
    void retrieveResultsForThermalCluster(
      typename Variable::Storage<VCardToFindT>::ResultsType** result,
      const Data::ThermalCluster* cluster)
    {
        std::apply(
          [&](auto&... w)
          { (w.template retrieveResultsForThermalCluster<VCardToFindT>(result, cluster), ...); },
          wrappers_);
    }

    template<class VCardToFindT>
    void retrieveResultsForLink(typename Variable::Storage<VCardToFindT>::ResultsType** result,
                                const Data::AreaLink* link)
    {
        std::apply([&](auto&... w)
                   { (w.template retrieveResultsForLink<VCardToFindT>(result, link), ...); },
                   wrappers_);
    }

    void localBuildAnnualSurveyReport(SurveyResults& results, int fileLevel, int precision) const
    {
        std::apply([&](const auto&... w)
                   { (w.localBuildAnnualSurveyReport(results, fileLevel, precision), ...); },
                   wrappers_);
    }

private:
    std::tuple<Wrappers...> wrappers_;
};

} // namespace Antares::Solver::Variable

#endif // __SOLVER_VARIABLE_JOIN_ALL_H__
