// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <utility>
#include <vector>

#include "antares/study/binding_constraint/BindingConstraint.h"
#include "antares/study/fwd.h"

#include "bindConstraints.h"

namespace Antares::Solver::Variable
{
template<class VariableList>
void BindingConstraints<VariableList>::buildSurveyReport(SurveyResults& results,
                                                         int dataLevel,
                                                         int fileLevel,
                                                         int precision) const
{
    if (bool bcDataLevel = dataLevel & Category::DataLevel::bindingConstraint; !bcDataLevel)
    {
        return;
    }

    for (uint i = 0; i != pBCcount; ++i)
    {
        const VariableList& bc = pBindConstraints[i];

        bc.buildSurveyReport(results, dataLevel, fileLevel, precision);
    }
}

template<class VariableList>
void BindingConstraints<VariableList>::buildAnnualSurveyReport(SurveyResults& results,
                                                               int dataLevel,
                                                               int fileLevel,
                                                               int precision,
                                                               uint numSpace) const
{
    if (bool bcDataLevel = dataLevel & Category::DataLevel::bindingConstraint; !bcDataLevel)
    {
        return;
    }

    for (uint i = 0; i != pBCcount; ++i)
    {
        const VariableList& bc = pBindConstraints[i];

        bc.buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace);
    }
}

template<class VariableList>
template<class PredicateT>
inline void BindingConstraints<VariableList>::RetrieveVariableList(PredicateT& predicate)
{
    VariableList::RetrieveVariableList(predicate);
}

template<class VariableList>
void BindingConstraints<VariableList>::simulationBegin()
{
    for (auto& bc: pBindConstraints)
    {
        bc.simulationBegin();
    }
}

template<class VariableList>
void BindingConstraints<VariableList>::simulationEnd()
{
    for (auto& bc: pBindConstraints)
    {
        bc.simulationEnd();
    }
}

template<class VariableList>
void BindingConstraints<VariableList>::buildThermalClusterYearEndResults(State& /*state*/,
                                                                         uint /*year*/,
                                                                         uint /*numSpace*/)
{
}

template<class VariableList>
void BindingConstraints<VariableList>::initializeFromStudy(Data::Study& study)
{
    const std::vector<std::shared_ptr<Data::BindingConstraint>>
      inequalityByPtr = study.bindingConstraints.getPtrForInequalityBindingConstraints();

    // The total number of inequality binding constraints count
    // (we don't count BCs with equality sign)
    pBCcount = (uint)inequalityByPtr.size();

    // Reserving the memory
    if (pBCcount > 0)
    {
        pBindConstraints.resize(pBCcount);
    }

    for (uint i = 0; i != pBCcount; ++i)
    {
        VariableList& bc = pBindConstraints[i];

        bc.setAssociatedBindConstraint(inequalityByPtr[i]);
        bc.initializeFromStudy(study);

        // Does user want to print output results related to the current binding constraint ?
        bc.getPrintStatusFromStudy(study);
    }

    // Here we supply the max number of columns to the variable print info collector
    // This is a ugly hack (it's a work around).
    // We should have a simple call to :
    //      VariableList::supplyMaxNumberOfColumns(study);
    // Instead, we have a few lines as a hack.
    // What we have to do is add to the print info collector a single VariablePrintInfo
    // that has a max columns size of : (nb of inequality BCs) x ResultsType::count
    // But note that for now, BC output variables are chained statically (one output variable per
    // inequality BC). The hack is to make the first BC output variable able to supply max columns
    // size for all BC output variables with its method getMaxNumberColumns(). A solution would be
    // to make BC output variables (like BindingConstMarginCost) some DYNAMIC variables.
    if (pBCcount > 0)
    {
        VariableList& bc = pBindConstraints[0];
        bc.setBindConstraintsCount(pBCcount);
        bc.supplyMaxNumberOfColumns(study);
    }
}

template<class VariableList>
void BindingConstraints<VariableList>::computeSummary(unsigned int year, unsigned int numSpace)
{
    for (uint i = 0; i != pBCcount; ++i)
    {
        // Broadcast to all constraints
        pBindConstraints[i].computeSummary(year, numSpace);
    }
}

template<class VariableList>
void BindingConstraints<VariableList>::yearBegin(uint year, uint numSpace)
{
    // Broadcast to all binding constraints
    for (uint i = 0; i != pBCcount; ++i)
    {
        pBindConstraints[i].yearBegin(year, numSpace);
    }
}

template<class VariableList>
void BindingConstraints<VariableList>::yearEnd(uint year, uint numSpace)
{
    // Broadcast to all binding constraints
    for (uint i = 0; i != pBCcount; ++i)
    {
        pBindConstraints[i].yearEnd(year, numSpace);
    }
}

template<class VariableList>
void BindingConstraints<VariableList>::weekBegin(State& state)
{
    for (uint i = 0; i != pBCcount; ++i)
    {
        pBindConstraints[i].weekBegin(state);
    }
}

template<class VariableList>
void BindingConstraints<VariableList>::weekEnd(State& state)
{
    for (uint i = 0; i != pBCcount; ++i)
    {
        pBindConstraints[i].weekEnd(state);
    }
}

template<class VariableList>
void BindingConstraints<VariableList>::hourBegin(uint hourInTheYear)
{
    for (uint i = 0; i != pBCcount; ++i)
    {
        pBindConstraints[i].hourBegin(hourInTheYear);
    }
}

template<class VariableList>
void BindingConstraints<VariableList>::hourEnd(State& state, uint hourInTheYear)
{
    for (uint i = 0; i != pBCcount; ++i)
    {
        pBindConstraints[i].hourEnd(state, hourInTheYear);
    }
}

template<class VariableList>
void BindingConstraints<VariableList>::weekForEachArea(State& state, unsigned int numSpace)
{
    for (uint i = 0; i != pBCcount; ++i)
    {
        pBindConstraints[i].weekForEachArea(state, numSpace);
    }
}

template<class VariableList>
void BindingConstraints<VariableList>::hourForEachArea(State& state, unsigned int numSpace)
{
    for (uint i = 0; i != pBCcount; ++i)
    {
        pBindConstraints[i].hourForEachArea(state, numSpace);
    }
}

template<class VariableList>
template<class VCardToFindT>
inline void BindingConstraints<VariableList>::retrieveResultsForArea(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::Area* area)
{
    VariableList::template retrieveResultsForArea<VCardToFindT>(result, area);
}

template<class VariableList>
void BindingConstraints<VariableList>::buildDigest(SurveyResults& results,
                                                   int digestLevel,
                                                   int dataLevel) const
{
    for (uint i = 0; i != pBCcount; ++i)
    {
        pBindConstraints[i].buildDigest(results, digestLevel, dataLevel);
    }
}

template<class VariableList>
template<class V>
void BindingConstraints<VariableList>::simulationEndSpatialAggregates(V& allVars)
{
    VariableList::template simulationEndSpatialAggregates<V>(allVars);
}

template<class VariableList>
template<class V>
void BindingConstraints<VariableList>::computeSpatialAggregatesSummary(V& allVars,
                                                                       unsigned int year,
                                                                       unsigned int numSpace)
{
    VariableList::template computeSpatialAggregatesSummary<V>(allVars, year, numSpace);
}

template<class VariableList>
void BindingConstraints<VariableList>::beforeYearByYearExport(uint year, uint numSpace)
{
    for (uint i = 0; i != pBCcount; ++i)
    {
        pBindConstraints[i].beforeYearByYearExport(year, numSpace);
    }
}

template<class VariableList>
template<class SearchVCardT, class O>
inline void BindingConstraints<VariableList>::computeSpatialAggregateWith(O& out,
                                                                          const Data::Area* area,
                                                                          uint numSpace)
{
    VariableList::template computeSpatialAggregateWith<SearchVCardT, O>(out, area, numSpace);
}

template<class VariableList>
template<class VCardToFindT>
inline void BindingConstraints<VariableList>::retrieveResultsForLink(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::AreaLink* link)
{
    VariableList::template retrieveResultsForLink<VCardToFindT>(result, link);
}

template<class VariableList>
template<class VCardToFindT>
inline void BindingConstraints<VariableList>::retrieveResultsForThermalCluster(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::ThermalCluster* cluster)
{
    VariableList::template retrieveResultsForThermalCluster<VCardToFindT>(result, cluster);
}
} // namespace Antares::Solver::Variable
