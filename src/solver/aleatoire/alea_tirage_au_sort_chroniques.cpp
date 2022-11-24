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
#include "alea_sys.h"
#include <yuni/core/math.h>

#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_structure_probleme_economique.h"
#include "../simulation/sim_structure_probleme_adequation.h"
#include "../simulation/sim_extern_variables_globales.h"
#include "alea_fonctions.h"
#include <limits>
#include <antares/logs.h>
#include <antares/date.h>
#include <antares/emergency.h>
#include <cassert>

using namespace Yuni;
using namespace Antares;
using namespace Antares::Data;

static void InitializeTimeSeriesNumbers_And_ThermalClusterProductionCost(
  double** thermalNoisesByArea,
  uint numSpace,
  bool EconomicModeT)
{
    auto& study = *Data::Study::Current::Get();
    auto& runtime = *study.runtime;

    uint year = runtime.timeseriesNumberYear[numSpace];

    const size_t nbDaysPerYearDouble = runtime.nbDaysPerYear * sizeof(double);

    // each area
    const unsigned int count = study.areas.size();
    for (unsigned int i = 0; i != count; ++i)
    {
        // Variables - the current area
        NUMERO_CHRONIQUES_TIREES_PAR_PAYS& ptchro = *NumeroChroniquesTireesParPays[numSpace][i];
        auto& area = *(study.areas.byIndex[i]);
        VALEURS_GENEREES_PAR_PAYS& ptvalgen = *(ValeursGenereesParPays[numSpace][i]);

        // Load
        {
            const Data::DataSeriesLoad& data = *area.load.series;
            assert(year < data.timeseriesNumbers.height);
            ptchro.Consommation
              = (data.series.width != 1) ? (long)data.timeseriesNumbers[0][year] : 0; // zero-based
        }
        // Solar
        {
            const Data::DataSeriesSolar& data = *area.solar.series;
            assert(year < data.timeseriesNumbers.height);
            ptchro.Solar
              = (data.series.width != 1) ? (long)data.timeseriesNumbers[0][year] : 0; // zero-based
        }
        // Hydro
        {
            const Data::DataSeriesHydro& data = *area.hydro.series; //### todo for hydrocluster
            assert(year < data.timeseriesNumbers.height);
            ptchro.Hydraulique
              = (data.count != 1) ? (long)data.timeseriesNumbers[0][year] : 0; // zero-based
            // Hydro - mod
            memset(ptvalgen.HydrauliqueModulableQuotidien, 0, nbDaysPerYearDouble);
        }
        //### todo for hydrocluster as in Renewable, avec ptchro.HydroclusterParPalier
        // Wind
        {
            const Data::DataSeriesWind& data = *area.wind.series;
            assert(year < data.timeseriesNumbers.height);
            ptchro.Eolien
              = (data.series.width != 1) ? (long)data.timeseriesNumbers[0][year] : 0; // zero-based
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
                unsigned int index = cluster->areaWideIndex;

                ptchro.RenouvelableParPalier[index] = (data.series.width != 1)
                                                        ? (long)data.timeseriesNumbers[0][year]
                                                        : 0; // zero-based
            }
        }

        // Thermal
        {
            uint indexCluster = 0;
            auto end = area.thermal.list.mapping.end();
            for (auto it = area.thermal.list.mapping.begin(); it != end; ++it)
            {
                ThermalClusterList::SharedPtr cluster = it->second;
                // Draw a new random number, whatever the cluster is
                double rnd = thermalNoisesByArea[i][indexCluster];

                if (!cluster->enabled)
                {
                    indexCluster++;
                    continue;
                }

                const auto& data = *cluster->series;
                assert(year < data.timeseriesNumbers.height);
                unsigned int index = cluster->areaWideIndex;

                // the matrix data.series should be properly initialized at this stage
                // because the ts-generator has already been launched
                ptchro.ThermiqueParPalier[index] = (data.series.width != 1)
                                                     ? (long)data.timeseriesNumbers[0][year]
                                                     : 0; // zero-based

                if (EconomicModeT)
                {
                    // ptvalgen.AleaCoutDeProductionParPalier[index] =
                    //	(rnd - 0.5) * (cluster->spreadCost + 1e-4);
                    // MBO
                    // 15/04/2014 : bornage du co�t thermique
                    // 01/12/2014 : prise en compte du spreadCost non nul

                    if (cluster->spreadCost == 0) // 5e-4 < |AleaCoutDeProductionParPalier| < 6e-4
                    {
                        if (rnd < 0.5)
                            ptvalgen.AleaCoutDeProductionParPalier[index] = 1e-4 * (5 + 2 * rnd);
                        else
                            ptvalgen.AleaCoutDeProductionParPalier[index]
                              = -1e-4 * (5 + 2 * (rnd - 0.5));
                    }
                    else
                    {
                        ptvalgen.AleaCoutDeProductionParPalier[index]
                          = (rnd - 0.5) * (cluster->spreadCost);

                        if (Math::Abs(ptvalgen.AleaCoutDeProductionParPalier[index]) < 5.e-4)
                        {
                            if (Math::Abs(ptvalgen.AleaCoutDeProductionParPalier[index]) >= 0)
                                ptvalgen.AleaCoutDeProductionParPalier[index] += 5.e-4;
                            else
                                ptvalgen.AleaCoutDeProductionParPalier[index] -= 5.e-4;
                        }
                    }
                }

                indexCluster++;
            }
        } // thermal
    }     // each area
    // Transmission capacities
    // each link
    for (unsigned int i = 0; i < runtime.interconnectionsCount; ++i)
    {
        AreaLink* link = runtime.areaLink[i];
        assert(year < link->timeseriesNumbers.height);
        NUMERO_CHRONIQUES_TIREES_PAR_INTERCONNEXION& ptchro
          = NumeroChroniquesTireesParInterconnexion[numSpace][i];
        const uint directWidth = link->directCapacities.width;
        const uint indirectWidth = link->indirectCapacities.width;
        assert(directWidth == indirectWidth);
        ptchro.TransmissionCapacities
          = (directWidth != 1) ? link->timeseriesNumbers[0][year] : 0; // zero-based
    }
}

void ALEA_TirageAuSortChroniques(double** thermalNoisesByArea, uint numSpace)
{
    // Time-series numbers
    bool ecoMode = Data::Study::Current::Get()->runtime->mode != stdmAdequacyDraft;
    // Retrieve all time-series numbers
    // Initialize in the same time the production costs of all thermal clusters.
    InitializeTimeSeriesNumbers_And_ThermalClusterProductionCost(
      thermalNoisesByArea, numSpace, ecoMode);
    // Flush all memory into the swap files
    // (only if the support is available)
    if (Antares::Memory::swapSupport)
        Antares::memory.flushAll();
}
