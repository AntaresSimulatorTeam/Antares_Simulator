/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#pragma once

// #include <antares/study/filter.h>

namespace Antares
{
namespace Solver
{
namespace Variable
{
template<class NextT>
inline BindingConstraints<NextT>::BindingConstraints()
{
    // Do nothing
}

template<class NextT>
inline void BindingConstraints<NextT>::initializeFromArea(Data::Study*, Data::Area*)
{
    // Nothing to do here
    // This method is called by initializeFromStudy() to all children
}

template<class NextT>
inline void BindingConstraints<NextT>::initializeFromAreaLink(Data::Study*, Data::AreaLink*)
{
    // Nothing to do here
}

template<class NextT>
inline void BindingConstraints<NextT>::initializeFromThermalCluster(Data::Study*,
                                                       Data::Area*,
                                                       Data::ThermalCluster*)
{
    // This method should not be called at this point
}

template<class NextT>
void BindingConstraints<NextT>::buildSurveyReport(SurveyResults& results,
                                     int dataLevel,
                                     int fileLevel,
                                     int precision) const
{
    /*
    int count_int = count;
    bool linkDataLevel = dataLevel & Category::link;
    bool areaDataLevel = dataLevel & Category::area;
    bool thermalAggregateDataLevel = dataLevel & Category::thermalAggregate;
    if (count_int && (linkDataLevel || areaDataLevel || thermalAggregateDataLevel))
    {
        assert(results.data.area != NULL
               && "The area must not be null to generate a survey report");

        // Build the survey results for the given area
        auto& area = *results.data.area;

        // Filtering
        if (0 == (dataLevel & Category::link)) // filter on all but links
        {
            switch (precision)
            {
            case Category::hourly:
                if (not(area.filterSynthesis & Data::filterHourly))
                    return;
                break;
            case Category::daily:
                if (not(area.filterSynthesis & Data::filterDaily))
                    return;
                break;
            case Category::weekly:
                if (not(area.filterSynthesis & Data::filterWeekly))
                    return;
                break;
            case Category::monthly:
                if (not(area.filterSynthesis & Data::filterMonthly))
                    return;
                break;
            case Category::annual:
                if (not(area.filterSynthesis & Data::filterAnnual))
                    return;
                break;
            case Category::all:
                break;
            }
        }
        pAreas[area.index].buildSurveyReport(results, dataLevel, fileLevel, precision);
    }
    */
}

template<class NextT>
void BindingConstraints<NextT>::buildAnnualSurveyReport(SurveyResults& results,
                                           int dataLevel,
                                           int fileLevel,
                                           int precision,
                                           uint numSpace) const
{
    /*
    int count_int = count;
    bool linkDataLevel = dataLevel & Category::link;
    bool areaDataLevel = dataLevel & Category::area;
    bool thermalAggregateDataLevel = dataLevel & Category::thermalAggregate;
    if (count_int && (linkDataLevel || areaDataLevel || thermalAggregateDataLevel))
    {
        assert(results.data.area != NULL
               && "The area must not be null to generate a survey report");

        auto& area = *results.data.area;

        // Filtering
        if (0 == (dataLevel & Category::link)) // filter on all but links
        {
            switch (precision)
            {
            case Category::hourly:
                if (!(area.filterYearByYear & Data::filterHourly))
                    return;
                break;
            case Category::daily:
                if (!(area.filterYearByYear & Data::filterDaily))
                    return;
                break;
            case Category::weekly:
                if (!(area.filterYearByYear & Data::filterWeekly))
                    return;
                break;
            case Category::monthly:
                if (!(area.filterYearByYear & Data::filterMonthly))
                    return;
                break;
            case Category::annual:
                if (!(area.filterYearByYear & Data::filterAnnual))
                    return;
                break;
            case Category::all:
                break;
            }
        }

        // Build the survey results for the given area
        pAreas[area.index].buildAnnualSurveyReport(
          results, dataLevel, fileLevel, precision, numSpace);
    }
    */
}

template<class NextT>
void BindingConstraints<NextT>::buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
{
    /*
    int count_int = count;
    if (count_int)
    {
        if (dataLevel & Category::area)
        {
            assert(pAreaCount == results.data.study.areas.size());

            // Reset captions
            results.data.rowCaptions.clear();
            results.data.rowCaptions.resize(pAreaCount);

            // For each area
            // for (uint i = 0; i != results.data.study.areas.byIndex.size(); ++i)
            for (uint i = 0; i != pAreaCount; ++i)
            {
                results.data.area = results.data.study.areas[i];
                uint index = results.data.area->index;
                results.data.rowIndex = index;
                results.data.rowCaptions[index] = results.data.area->id;
                results.data.columnIndex = 0;
                results.resetValuesAtLine(i);
                pAreas[i].buildDigest(results, digestLevel, dataLevel);
            }
        }
    }
    */
}

template<class NextT>
template<class PredicateT>
inline void BindingConstraints<NextT>::RetrieveVariableList(PredicateT& predicate)
{
    // NextType::RetrieveVariableList(predicate);
}

template<class NextT>
template<class I>
inline void BindingConstraints<NextT>::provideInformations(I& infos)
{
    /*
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
    */
}

template<class NextT>
template<class SearchVCardT, class O>
inline void BindingConstraints<NextT>::computeSpatialAggregateWith(O&)
{
    // Do nothing
}

template<class NextT>
template<class SearchVCardT, class O>
inline void BindingConstraints<NextT>::computeSpatialAggregateWith(O& out, const Data::Area* area, uint numSpace)
{
    /*
    assert(NULL != area);
    pAreas[area->index].template computeSpatialAggregateWith<SearchVCardT, O>(out, numSpace);
    */
}

template<class NextT>
template<class VCardToFindT>
const double* BindingConstraints<NextT>::retrieveHourlyResultsForCurrentYear() const
{
    return nullptr;
}

template<class NextT>
template<class VCardToFindT>
inline void BindingConstraints<NextT>::retrieveResultsForArea(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::Area* area)
{
    // pAreas[area->index].template retrieveResultsForArea<VCardToFindT>(result, area);
}

template<class NextT>
template<class VCardToFindT>
inline void BindingConstraints<NextT>::retrieveResultsForThermalCluster(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::ThermalCluster* cluster)
{
    /*
    pAreas[cluster->parentArea->index].template retrieveResultsForThermalCluster<VCardToFindT>(
      result, cluster);
    */
}

template<class NextT>
template<class VCardToFindT>
inline void BindingConstraints<NextT>::retrieveResultsForLink(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::AreaLink* link)
{
    // pAreas[link->from->index].template retrieveResultsForLink<VCardToFindT>(result, link);
}


// ==============================================
// ===========  Copy of area.inc.hxx  ===========
// ==============================================

template<class NextT>
BindingConstraints<NextT>::~BindingConstraints()
{
    // Releasing the memory occupied by the areas
    delete[] pAreas;
}

template<class NextT>
void BindingConstraints<NextT>::initializeFromStudy(Data::Study& study)
{
    // The total number of areas
    // pAreaCount = study.areas.size();

    // Reserving the memory
    // pAreas = new NextType[pAreaCount];

    /*
    // For each area...
    uint tick = 6;
    uint oldPercent = 0;
    for (uint i = 0; i != pAreaCount; ++i)
    {
        // Instancing a new set of variables of the area
        auto& n = pAreas[i];
        auto* currentArea = study.areas.byIndex[i];
        if (!(--tick))
        {
            uint newPercent = ((i * 100u) / pAreaCount);
            if (newPercent != oldPercent)
            {
                logs.info() << "Allocating resources " << ((i * 100u) / pAreaCount) << "%";
                oldPercent = newPercent;
            }
            // Reset the tick
            tick = 6;
        }

        // Initialize the variables
        // From the study
        n.initializeFromStudy(study);
        // From the area
        n.initializeFromArea(&study, currentArea);
        // Does current output variable appears non applicable in areas' output files, not
        // districts'. Note that digest gather area and district results.
        n.broadcastNonApplicability(not currentArea->hydro.reservoirManagement);

        // For each current area's variable, getting the print status, that is :
        // is variable's column(s) printed in output (areas) reports ?
        n.getPrintStatusFromStudy(study);

        // It is needed that the whole memory is flushed to swap
        // Some intermediate are not flush and it may lead
        // to an excessive unused amount of memory
        if (Antares::Memory::swapSupport)
            Antares::memory.flushAll();
    }
    */
}

template<class NextT>
void BindingConstraints<NextT>::simulationBegin()
{
    /*
    for (uint i = 0; i != pAreaCount; ++i)
    {
        pAreas[i].simulationBegin();
        if (Antares::Memory::swapSupport)
            Antares::memory.flushAll();
    }
    */
}

template<class NextT>
void BindingConstraints<NextT>::simulationEnd()
{
    /*
    for (uint i = 0; i != pAreaCount; ++i)
    {
        pAreas[i].simulationEnd();
        if (Antares::Memory::swapSupport)
            Antares::memory.flushAll();
    }
    */
}

template<class NextT>
void BindingConstraints<NextT>::hourForEachArea(State& state, uint numSpace)
{
    /*
    // For each area...
    state.study.areas.each([&](Data::Area& area) {
        state.area = &area; // the current area

        // Initializing the state for the current area
        state.initFromAreaIndex(area.index, numSpace);

        // Variables
        auto& variablesForArea = pAreas[area.index];
        variablesForArea.hourForEachArea(state, numSpace);

        // For each thermal cluster
        for (uint j = 0; j != area.thermal.clusterCount(); ++j)
        {
            // Intiializing the state for the current thermal cluster
            state.initFromThermalClusterIndex(j, numSpace);
            // Variables
            variablesForArea.hourForEachThermalCluster(state, numSpace);

        } // for each thermal cluster

        // For each renewable cluster
        for (uint j = 0; j != area.renewable.clusterCount(); ++j)
        {
            // Intitializing the state for the current thermal cluster
            state.initFromRenewableClusterIndex(j, numSpace);
            // Variables
            variablesForArea.hourForEachRenewableCluster(state, numSpace);
        } // for each renewable cluster

        // All links
        auto end = area.links.end();
        for (auto i = area.links.begin(); i != end; ++i)
        {
            state.link = i->second;
            // Variables
            variablesForArea.hourForEachLink(state, numSpace);
        }
        }); // for each area
    */
}

template<class NextT>
void BindingConstraints<NextT>::weekForEachArea(State& state, uint numSpace)
{
    /*
    // For each area...
    state.study.areas.each([&](Data::Area& area) {
        state.area = &area; // the current area

        // Initializing the state for the current area
        state.initFromAreaIndex(area.index, numSpace);

        auto& variablesForArea = pAreas[area.index];

        // DTG MRG
        state.dispatchableMargin
            = variablesForArea
            .retrieveHourlyResultsForCurrentYear<Economy::VCardDispatchableGenMargin>(numSpace);

        variablesForArea.weekForEachArea(state, numSpace);

        // NOTE
        // currently, the event is not broadcasted to thermal
        // clusters and links
        }); // for each area
    */
}

template<class NextT>
void BindingConstraints<NextT>::yearBegin(uint year, uint numSpace)
{
    /*
    for (uint i = 0; i != pAreaCount; ++i)
        pAreas[i].yearBegin(year, numSpace);
    */
}

template<class NextT>
void BindingConstraints<NextT>::yearEndBuild(State& state, uint year, uint numSpace)
{
    /*
    // For each area...
    state.study.areas.each([&](Data::Area& area) {
        state.area = &area; // the current area

        // Initializing the state for the current area
        state.initFromAreaIndex(area.index, numSpace);

        // Variables
        auto& variablesForArea = pAreas[area.index];

        // For each thermal cluster
        for (uint j = 0; j != area.thermal.clusterCount(); ++j)
        {
            state.thermalCluster = area.thermal.clusters[j];
            state.yearEndResetThermal();

            // Variables
            variablesForArea.yearEndBuildPrepareDataForEachThermalCluster(state, year, numSpace);

            // Building the end of year
            state.yearEndBuildFromThermalClusterIndex(j, numSpace);

            // Variables
            variablesForArea.yearEndBuildForEachThermalCluster(state, year, numSpace);
        } // for each thermal cluster

        // For each renewable cluster
        for (uint j = 0; j != area.renewable.clusterCount(); ++j)
        {
            state.renewableCluster = area.renewable.clusters[j];
            state.yearEndResetRenewable();

            // Variables
            variablesForArea.yearEndBuildPrepareDataForEachRenewableCluster(state, year, numSpace);
        } // for each renewable cluster
        });   // for each area
    */
}

template<class NextT>
void BindingConstraints<NextT>::yearEnd(uint year, uint numSpace)
{
    /*
    for (uint i = 0; i != pAreaCount; ++i)
    {
        // Broadcast to all areas
        pAreas[i].yearEnd(year, numSpace);

        // Flush all memory into the swap files
        // This is mandatory for big studies with numerous areas
        if (Antares::Memory::swapSupport)
            Antares::memory.flushAll();
    }
    */
}

template<class NextT>
void BindingConstraints<NextT>::computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
    unsigned int nbYearsForCurrentSummary)
{
    /*
    for (uint i = 0; i != pAreaCount; ++i)
    {
        // Broadcast to all areas
        pAreas[i].computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
    }
    */
}

template<class NextT>
void BindingConstraints<NextT>::weekBegin(State& state)
{
    /*
    for (uint i = 0; i != pAreaCount; ++i)
        pAreas[i].weekBegin(state);
    */
}

template<class NextT>
void BindingConstraints<NextT>::weekEnd(State& state)
{
    for (uint i = 0; i != pAreaCount; ++i)
        pAreas[i].weekEnd(state);
}

template<class NextT>
void BindingConstraints<NextT>::hourBegin(uint hourInTheYear)
{
    /*
    for (uint i = 0; i != pAreaCount; ++i)
        pAreas[i].hourBegin(hourInTheYear);
    */
}

template<class NextT>
void BindingConstraints<NextT>::hourForEachLink(State& state, uint numSpace)
{
    /*
    for (uint i = 0; i != pAreaCount; ++i)
        pAreas[i].hourForEachLink(state, numSpace);
    */
}

template<class NextT>
void BindingConstraints<NextT>::hourForEachThermalCluster(State& state, uint numSpace)
{
    /*
    for (uint i = 0; i != pAreaCount; ++i)
        pAreas[i].hourForEachThermalCluster(state, numSpace);
    */
}

template<class NextT>
void BindingConstraints<NextT>::hourEnd(State& state, uint hourInTheYear)
{
    /*
    for (uint i = 0; i != pAreaCount; ++i)
        pAreas[i].hourEnd(state, hourInTheYear);
    */
}

template<class NextT>
void BindingConstraints<NextT>::beforeYearByYearExport(uint year, uint numSpace)
{
    /*
    for (uint i = 0; i != pAreaCount; ++i)
        pAreas[i].beforeYearByYearExport(year, numSpace);

    // Flush all memory into the swap files
    // (only if the support is available)
    if (Memory::swapSupport)
        memory.flushAll();
    */
}

} // namespace Variable
} // namespace Solver
} // namespace Antares

