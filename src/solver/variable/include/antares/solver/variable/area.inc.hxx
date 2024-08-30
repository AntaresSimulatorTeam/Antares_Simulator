/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
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
        pAreas[i].initializeFromStudy(study);
        // From the area
        pAreas[i].initializeFromArea(&study, currentArea);
        // Does current output variable appears non applicable in areas' output files, not
        // districts'. Note that digest gather area and district results.
        pAreas[i].broadcastNonApplicability(not currentArea->hydro.reservoirManagement);

        // For each current area's variable, getting the print status, that is :
        // is variable's column(s) printed in output (areas) reports ?
        pAreas[i].getPrintStatusFromStudy(study);

        pAreas[i].supplyMaxNumberOfColumns(study);
    }
}

template<>
void Areas<NEXTTYPE>::simulationBegin()
{
    for (uint i = 0; i != pAreaCount; ++i)
    {
        pAreas[i].simulationBegin();
    }
}

template<>
void Areas<NEXTTYPE>::simulationEnd()
{
    for (uint i = 0; i != pAreaCount; ++i)
    {
        pAreas[i].simulationEnd();
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

        for (auto cluster : area.thermal.list.all())
        {
            // Intiializing the state for the current thermal cluster
            state.initFromThermalClusterIndex(cluster->areaWideIndex);
        }

        for (int i = 0; i < area.shortTermStorage.storagesByIndex.size(); i++)
        {
            // Intiializing the state for the current short term storage cluster
            state.initFromShortTermStorageClusterIndex(i);
        }

        // Ajout de l'initialisation pour l'hydro (long terme)
        state.initFromHydroStorage();

        // Variables
        auto& variablesForArea = pAreas[area.index];
        variablesForArea.hourForEachArea(state, numSpace);

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

        for(auto cluster : area.thermal.list.each_enabled())
        {
            state.thermalCluster = cluster.get();

            // Variables
            variablesForArea.yearEndBuildPrepareDataForEachThermalCluster(state, year, numSpace);

            // Building the end of year
            state.yearEndBuildFromThermalClusterIndex(cluster->areaWideIndex);

            // Variables
            variablesForArea.yearEndBuildForEachThermalCluster(state, year, numSpace);
            state.yearEndResetThermal();
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
}

} // namespace Variable
} // namespace Solver
} // namespace Antares
