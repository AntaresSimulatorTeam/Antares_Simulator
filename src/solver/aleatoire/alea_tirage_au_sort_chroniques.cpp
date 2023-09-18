/*
** Copyright 2007-2023 RTE
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

#include "../simulation/sim_extern_variables_globales.h"
#include "alea_fonctions.h"
#include <cassert>

using namespace Yuni;
using namespace Antares;
using namespace Antares::Data;

void ApplyRandomTSnumbers(const Study& study,
                          unsigned int year,
                          uint numSpace)
{
    // each area
    const unsigned int count = study.areas.size();
    for (unsigned int areaIndex = 0; areaIndex != count; ++areaIndex)
    {
        // Variables - the current area
        NUMERO_CHRONIQUES_TIREES_PAR_PAYS& ptchro = NumeroChroniquesTireesParPays[numSpace][areaIndex];
        auto& area = *(study.areas.byIndex[areaIndex]);

        // Load
        {
            const Data::DataSeriesLoad& data = *area.load.series;
            assert(year < data.timeseriesNumbers.height);
            ptchro.Consommation
              = (data.timeSeries.width != 1) ? (long)data.timeseriesNumbers[0][year] : 0; // zero-based
        }
        // Solar
        {
            const Data::DataSeriesSolar& data = *area.solar.series;
            assert(year < data.timeseriesNumbers.height);
            ptchro.Solar
              = (data.timeSeries.width != 1) ? (long)data.timeseriesNumbers[0][year] : 0; // zero-based
        }
        // Hydro
        {
            const Data::DataSeriesHydro& data = *area.hydro.series;
            assert(year < data.timeseriesNumbers.height);
            ptchro.Hydraulique
              = (data.count != 1) ? (long)data.timeseriesNumbers[0][year] : 0; // zero-based
        }
        // Wind
        {
            const Data::DataSeriesWind& data = *area.wind.series;
            assert(year < data.timeseriesNumbers.height);
            ptchro.Eolien
              = (data.timeSeries.width != 1) ? (long)data.timeseriesNumbers[0][year] : 0; // zero-based
        }
        // Renewable
        {
            auto end = area.renewable.list.cluster.end();
            for (auto it = area.renewable.list.cluster.begin(); it != end; ++it)
            {
                RenewableClusterList::SharedPtr cluster = it->second;
                if (!cluster->enabled)
                {
                    continue;
                }

                const auto& data = *cluster->series;
                assert(year < data.timeseriesNumbers.height);
                unsigned int clusterIndex = cluster->areaWideIndex;

                ptchro.RenouvelableParPalier[clusterIndex] = (data.timeSeries.width != 1)
                                                             ? (long)data.timeseriesNumbers[0][year]
                                                             : 0; // zero-based
            }
        }

        // Thermal
        {
            auto end = area.thermal.list.mapping.end();
            for (auto it = area.thermal.list.mapping.begin(); it != end; ++it)
            {
                ThermalClusterList::SharedPtr cluster = it->second;

                if (!cluster->enabled)
                {
                    continue;
                }

                const auto& data = *cluster->series;
                assert(year < data.timeseriesNumbers.height);
                unsigned int clusterIndex = cluster->areaWideIndex;

                // the matrix data.series should be properly initialized at this stage
                // because the ts-generator has already been launched
                ptchro.ThermiqueParPalier[clusterIndex] = (data.timeSeries.width != 1)
                                                          ? (long)data.timeseriesNumbers[0][year]
                                                          : 0; // zero-based
            }
        } // thermal
    }     // each area

    // ------------------------------
    // Transmission capacities
    // ------------------------------
    // each link
    for (unsigned int linkIndex = 0; linkIndex < study.runtime->interconnectionsCount(); ++linkIndex)
    {
        AreaLink* link = study.runtime->areaLink[linkIndex];
        assert(year < link->timeseriesNumbers.height);
        NUMERO_CHRONIQUES_TIREES_PAR_INTERCONNEXION& ptchro
          = NumeroChroniquesTireesParInterconnexion[numSpace][linkIndex];
        const uint directWidth = link->directCapacities.width;
        [[maybe_unused]] const uint indirectWidth = link->indirectCapacities.width;
        assert(directWidth == indirectWidth);
        ptchro.TransmissionCapacities
          = (directWidth != 1) ? link->timeseriesNumbers[0][year] : 0; // zero-based
    }
    
    // ------------------------------
    //Binding constraints
    // ------------------------------
    //Setting 0 for time_series of width 0 is done when using the value.
    //To do this here we would have to check every BC for its width
    for (const auto& group: study.bindingConstraintsGroups) {
        [[maybe_unused]] auto number_of_ts_numbers = group->timeseriesNumbers.height;
        assert(year < number_of_ts_numbers); //If only 1 ts_number we suppose only one TS. Any "year" will be converted to "0" later
        NumeroChroniquesTireesParGroup[numSpace][group->name()] = group->timeseriesNumbers[0][year];
    }
}
