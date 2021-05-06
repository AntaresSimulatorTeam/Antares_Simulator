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

#include "variable.h"
#include "area.h"
#include <cassert>

using namespace Yuni;

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{
// forward declaration
struct VCardDispatchableGenMargin;

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares

namespace Antares
{
namespace Solver
{
namespace Variable
{
template<>
Areas<NEXTTYPE>::~Areas()
{
    // Releasing the memory occupied by the areas
    delete[] pAreas;
}

template<>
void Areas<NEXTTYPE>::initializeFromStudy(Data::Study& study)
{
    // The total number of areas
    pAreaCount = study.areas.size();

    // Reserving the memory
    pAreas = new NextType[pAreaCount];

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
}

template<>
void Areas<NEXTTYPE>::simulationBegin()
{
    for (uint i = 0; i != pAreaCount; ++i)
    {
        pAreas[i].simulationBegin();
        if (Antares::Memory::swapSupport)
            Antares::memory.flushAll();
    }
}

template<>
void Areas<NEXTTYPE>::simulationEnd()
{
    for (uint i = 0; i != pAreaCount; ++i)
    {
        pAreas[i].simulationEnd();
        if (Antares::Memory::swapSupport)
            Antares::memory.flushAll();
    }
}

template<>
void Areas<NEXTTYPE>::hourForEachArea(State& state, uint numSpace)
{
    // For each area...
    state.study.areas.each([&](Data::Area& area) {
        state.area = &area; // the current area

        // Initializing the state for the current area
        state.initFromAreaIndex(area.index, numSpace);

        // Variables
        auto& variablesForArea = pAreas[area.index];
        variablesForArea.hourForEachArea(state, numSpace);

        // For each thermal cluster
        for (uint j = 0; j != area.thermal.clusterCount; ++j)
        {
            // Intiializing the state for the current thermal cluster
            state.initFromThermalClusterIndex(j, numSpace);
            // Variables
            variablesForArea.hourForEachThermalCluster(state, numSpace);

        } // for each thermal cluster

        // All links
        auto end = area.links.end();
        for (auto i = area.links.begin(); i != end; ++i)
        {
            state.link = i->second;
            // Variables
            variablesForArea.hourForEachLink(state, numSpace);
        }
    }); // for each area
}

template<>
void Areas<NEXTTYPE>::weekForEachArea(State& state, uint numSpace)
{
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
}

template<>
void Areas<NEXTTYPE>::yearBegin(uint year, uint numSpace)
{
    for (uint i = 0; i != pAreaCount; ++i)
        pAreas[i].yearBegin(year, numSpace);
}

/*template<>
void Areas<NEXTTYPE>::yearEndBuildPrepareDataForEachThermalCluster(State& state, uint year)
{
        for (uint i = 0; i != pAreaCount; ++i)
                pAreas[i].yearEndBuildPrepareDataForEachThermalCluster(state, year);
}

template<>
void Areas<NEXTTYPE>::yearEndBuildForEachThermalCluster(State& state, uint year)
{
        for (uint i = 0; i != pAreaCount; ++i)
                pAreas[i].yearEndBuildForEachThermalCluster(state, year);
}*/

template<>
void Areas<NEXTTYPE>::yearEndBuild(State& state, uint year, uint numSpace)
{
    // For each area...
    state.study.areas.each([&](Data::Area& area) {
        state.area = &area; // the current area

        // Initializing the state for the current area
        state.initFromAreaIndex(area.index, numSpace);

        // Variables
        auto& variablesForArea = pAreas[area.index];

        // For each thermal cluster
        for (uint j = 0; j != area.thermal.clusterCount; ++j)
        {
            state.thermalCluster = area.thermal.clusters[j];
            state.yearEndReset();

            // Variables
            variablesForArea.yearEndBuildPrepareDataForEachThermalCluster(state, year, numSpace);

            // Building the end of year
            state.yearEndBuildFromThermalClusterIndex(j, numSpace);

            // Variables
            variablesForArea.yearEndBuildForEachThermalCluster(state, year, numSpace);
        } // for each thermal cluster
    });   // for each area
}

template<>
void Areas<NEXTTYPE>::yearEnd(uint year, uint numSpace)
{
    for (uint i = 0; i != pAreaCount; ++i)
    {
        // Broadcast to all areas
        pAreas[i].yearEnd(year, numSpace);

        // Flush all memory into the swap files
        // This is mandatory for big studies with numerous areas
        if (Antares::Memory::swapSupport)
            Antares::memory.flushAll();
    }
}

template<>
void Areas<NEXTTYPE>::computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                                     unsigned int nbYearsForCurrentSummary)
{
    for (uint i = 0; i != pAreaCount; ++i)
    {
        // Broadcast to all areas
        pAreas[i].computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
    }
}

template<>
void Areas<NEXTTYPE>::weekBegin(State& state)
{
    for (uint i = 0; i != pAreaCount; ++i)
        pAreas[i].weekBegin(state);
}

template<>
void Areas<NEXTTYPE>::weekEnd(State& state)
{
    for (uint i = 0; i != pAreaCount; ++i)
        pAreas[i].weekEnd(state);
}

template<>
void Areas<NEXTTYPE>::hourBegin(uint hourInTheYear)
{
    for (uint i = 0; i != pAreaCount; ++i)
        pAreas[i].hourBegin(hourInTheYear);
}

template<>
void Areas<NEXTTYPE>::hourForEachLink(State& state, uint numSpace)
{
    for (uint i = 0; i != pAreaCount; ++i)
        pAreas[i].hourForEachLink(state, numSpace);
}

template<>
void Areas<NEXTTYPE>::hourForEachThermalCluster(State& state, uint numSpace)
{
    for (uint i = 0; i != pAreaCount; ++i)
        pAreas[i].hourForEachThermalCluster(state, numSpace);
}

template<>
void Areas<NEXTTYPE>::hourEnd(State& state, uint hourInTheYear)
{
    for (uint i = 0; i != pAreaCount; ++i)
        pAreas[i].hourEnd(state, hourInTheYear);
}

template<>
void Areas<NEXTTYPE>::beforeYearByYearExport(uint year, uint numSpace)
{
    for (uint i = 0; i != pAreaCount; ++i)
        pAreas[i].beforeYearByYearExport(year, numSpace);

    // Flush all memory into the swap files
    // (only if the support is available)
    if (Memory::swapSupport)
        memory.flushAll();
}

} // namespace Variable
} // namespace Solver
} // namespace Antares
