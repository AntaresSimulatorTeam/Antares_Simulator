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

#include "antares/solver/optimisation/opt_construction_variables_reserves.h"

#include <spx_constantes_externes.h>

#include "antares/solver/optimisation/opt_rename_problem.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/optimisation/variables/VariableManagerUtils.h"

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireReserves(
  PROBLEME_HEBDO* problemeHebdo,
  bool Simulation)
{
    VariableNamer variableNamer(problemeHebdo->ProblemeAResoudre->NomDesVariables);
    int NombreDePasDeTempsPourUneOptimisation = problemeHebdo
                                                  ->NombreDePasDeTempsPourUneOptimisation;

    struct ReserveVariablesInitializer
    {
        PROBLEME_HEBDO* problemeHebdo;
        bool Simulation;
        const std::unique_ptr<PROBLEME_ANTARES_A_RESOUDRE>& ProblemeAResoudre;
        int& NombreDeVariables;
        VariableNamer& variableNamer;
        VariableManagement::VariableManager variableManager;

        ReserveVariablesInitializer(PROBLEME_HEBDO* hebdo, bool sim, VariableNamer& namer):
            problemeHebdo(hebdo),
            Simulation(sim),
            ProblemeAResoudre(hebdo->ProblemeAResoudre),
            NombreDeVariables(ProblemeAResoudre->NombreDeVariables),
            variableNamer(namer),
            variableManager(VariableManagerFromProblemHebdo(hebdo))
        {
        }

        // Init variables for a reserve
        void initReserve(int pdt, const int reserveIndex, const std::string& reserveName)
        {
            if (Simulation)
            {
                NombreDeVariables += 2;
            }
            else
            {
                // For Unsatisfied Reserves
                variableManager.InternalUnsatisfiedReserve(reserveIndex, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.InternalUnsatisfiedReserve(NombreDeVariables, reserveName);
                NombreDeVariables++;

                // For Excess Reserves
                variableManager.InternalExcessReserve(reserveIndex, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.InternalExcessReserve(NombreDeVariables, reserveName);
                NombreDeVariables++;
            }
        }

        // Init variables for a Thermal cluster participation to a reserve up or down
        void initThermalReserveParticipation(
          ReserveType type,
          int pdt,
          const RESERVE_PARTICIPATION_THERMAL& clusterReserveParticipation,
          const std::string& reserveName)
        {
            const auto& clusterName = clusterReserveParticipation.clusterName;
            if (Simulation)
            {
                NombreDeVariables += (type == ReserveType::UP ? 4
                                                              : 2); // 4 for up reserves, 2 for down
            }
            else
            {
                // For running units in cluster
                variableManager.RunningThermalClusterReserveParticipation(
                  clusterReserveParticipation.globalIndexClusterParticipation,
                  pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ParticipationOfRunningUnitsToReserve(NombreDeVariables,
                                                                   clusterName,
                                                                   reserveName);
                NombreDeVariables++;

                if (type == ReserveType::UP) // For off units in cluster (off units can not
                                             // participate to down reserves)
                {
                    variableManager.OffThermalClusterReserveParticipation(
                      clusterReserveParticipation.globalIndexClusterParticipation,
                      pdt)
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_DES_DEUX_COTES;
                    variableNamer.ParticipationOfOffUnitsToReserve(NombreDeVariables,
                                                                   clusterName,
                                                                   reserveName);
                    NombreDeVariables++;

                    variableManager.OffThermalClusterReserveParticipation(
                      clusterReserveParticipation.globalIndexClusterParticipation,
                      pdt)
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_DES_DEUX_COTES;
                    ProblemeAResoudre->VariablesEntieres[NombreDeVariables]
                      = problemeHebdo->OptimisationAvecVariablesEntieres;
                    variableNamer.ParticipationOfOffUnitsToReserve(NombreDeVariables,
                                                                   clusterName,
                                                                   reserveName);
                    NombreDeVariables++;
                }

                // For all units in cluster
                variableManager.ThermalClusterReserveParticipation(
                  clusterReserveParticipation.globalIndexClusterParticipation,
                  pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ThermalClusterReserveParticipation(NombreDeVariables,
                                                                 clusterName,
                                                                 reserveName);
                NombreDeVariables++;
            }
        }

        // Init variables for a ShortTerm cluster participation to a reserve
        void initSTStorageReserveParticipation(
          ReserveType type,
          int pdt,
          const RESERVE_PARTICIPATION_STSTORAGE& clusterReserveParticipation,
          const std::string& reserveName)
        {
            const auto& clusterName = clusterReserveParticipation.clusterName;
            if (Simulation)
            {
                NombreDeVariables += 3;
            }
            else
            {
                // For Release participation to the reserves
                variableManager.STStorageReleaseClusterReserveParticipation(
                  clusterReserveParticipation.globalIndexClusterParticipation,
                  pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ParticipationOfSTStorageReleaseToReserve(NombreDeVariables,
                                                                       clusterName,
                                                                       reserveName);
                NombreDeVariables++;

                // For Store participation to the reserves
                variableManager.STStorageStoreClusterReserveParticipation(
                  clusterReserveParticipation.globalIndexClusterParticipation,
                  pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ParticipationOfSTStorageStoreToReserve(NombreDeVariables,
                                                                     clusterName,
                                                                     reserveName);
                NombreDeVariables++;

                // For Short Term Storage participation to the up reserves
                variableManager.STStorageClusterReserveParticipation(
                  type,
                  clusterReserveParticipation.globalIndexClusterParticipation,
                  pdt)
                  = NombreDeVariables;

                variableNamer.ParticipationOfSTStorageToReserve(type,
                                                                NombreDeVariables,
                                                                clusterName,
                                                                reserveName);
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                NombreDeVariables++;
            }
        }

        // Init variables for a Hydro participation to a reserve
        void initHydroReserveParticipation(
          ReserveType type,
          int pdt,
          const RESERVE_PARTICIPATION_HYDRO& clusterReserveParticipation,
          const std::string& reserveName)
        {
            const auto& clusterName = clusterReserveParticipation.clusterName;
            if (Simulation)
            {
                NombreDeVariables += 3;
            }
            else
            {
                // For Release participation to the reserves
                variableManager.HydroReleaseReserveParticipation(
                  clusterReserveParticipation.globalIndexClusterParticipation,
                  pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ParticipationOfHydroReleaseToReserve(NombreDeVariables,
                                                                   clusterName,
                                                                   reserveName);
                NombreDeVariables++;

                // For Store participation to the reserves
                variableManager.HydroStoreReserveParticipation(
                  clusterReserveParticipation.globalIndexClusterParticipation,
                  pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ParticipationOfHydroStoreToReserve(NombreDeVariables,
                                                                 clusterName,
                                                                 reserveName);
                NombreDeVariables++;

                // For Hydro participation to the reserves
                variableManager.HydroReserveParticipation(
                  type,
                  clusterReserveParticipation.globalIndexClusterParticipation,
                  pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;

                variableNamer.ParticipationOfHydroToReserve(type,
                                                            NombreDeVariables,
                                                            clusterName,
                                                            reserveName);

                NombreDeVariables++;
            }
        }
    };

    ReserveVariablesInitializer reserveVariablesInitializer(problemeHebdo,
                                                            Simulation,
                                                            variableNamer);
    for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        variableNamer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        auto& CorrespondanceVarNativesVarOptim = problemeHebdo
                                                   ->CorrespondanceVarNativesVarOptim[pdt];

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            variableNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

            for (auto& areaReserve:
                 problemeHebdo->allReserves.value()[pays].areaCapacityReservations)
            {
                reserveVariablesInitializer.initReserve(pdt,
                                                        areaReserve.globalReserveIndex,
                                                        areaReserve.reserveName);

                // Thermal Clusters
                for (auto& [clusterId, clusterReserveParticipation]:
                     areaReserve.AllThermalReservesParticipation)
                {
                    reserveVariablesInitializer.initThermalReserveParticipation(
                      areaReserve.type,
                      pdt,
                      clusterReserveParticipation,
                      areaReserve.reserveName);
                }

                // Short Term Storage Clusters
                for (auto& [clusterId, clusterReserveParticipation]:
                     areaReserve.AllSTStorageReservesParticipation)
                {
                    reserveVariablesInitializer.initSTStorageReserveParticipation(
                      areaReserve.type,
                      pdt,
                      clusterReserveParticipation,
                      areaReserve.reserveName);
                }

                // Hydro
                for (auto& clusterReserveParticipation: areaReserve.AllHydroReservesParticipation)
                {
                    reserveVariablesInitializer.initHydroReserveParticipation(
                      areaReserve.type,
                      pdt,
                      clusterReserveParticipation,
                      areaReserve.reserveName);
                }
            }
        }
    }
}
