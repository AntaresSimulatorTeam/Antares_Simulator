// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/opt_construction_variables_reserves.h"

#include <spx_constantes_externes.h>

#include "antares/solver/optimisation/opt_rename_problem.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/optimisation/variables/VariableManagerUtils.h"

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireReserves(
  PROBLEME_HEBDO* problemeHebdo,
  bool Simulation)
{
    VariableNamer variableNamer(problemeHebdo->ProblemeAResoudre->NomDesVariables,
                                problemeHebdo->ProblemeAResoudre->LegacyVariablesInfo);
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
        void initReserve(int pdt, const int reserveIndex, const ReserveIdentity& reserve)
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
                variableNamer.InternalUnsatisfiedReserve(NombreDeVariables, reserve);
                NombreDeVariables++;

                // For Excess Reserves
                variableManager.InternalExcessReserve(reserveIndex, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.InternalExcessReserve(NombreDeVariables, reserve);
                NombreDeVariables++;
            }
        }

        // Init variables for a Thermal cluster participation to a reserve up or down
        void initThermalReserveParticipation(
          ReserveType type,
          int pdt,
          const RESERVE_PARTICIPATION_THERMAL& clusterReserveParticipation,
          const ReserveIdentity& reserve)
        {
            const auto& clusterName = clusterReserveParticipation.clusterName;
            if (Simulation)
            {
                NombreDeVariables += (type == ReserveType::UP ? 3
                                                              : 2); // 3 for up reserves, 2 for down
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
                                                                   reserve);
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
                                                                   reserve);
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
                                                                 reserve);
                NombreDeVariables++;
            }
        }

        // Init variables for a ShortTerm cluster participation to a reserve
        void initSTStorageReserveParticipation(
          ReserveType type,
          int pdt,
          const RESERVE_PARTICIPATION_STSTORAGE& clusterReserveParticipation,
          const ReserveIdentity& reserve)
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
                                                                       reserve);
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
                                                                     reserve);
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
                                                                reserve);
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
          const ReserveIdentity& reserve)
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
                                                                   reserve);
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
                                                                 reserve);
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
                                                            reserve);

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

            for (auto& areaReserve: problemeHebdo->allReserves->at(pays).areaCapacityReservations)
            {
                const ReserveIdentity reserve{areaReserve.reserveName, areaReserve.reserveID};

                reserveVariablesInitializer.initReserve(pdt,
                                                        areaReserve.globalReserveIndex,
                                                        reserve);

                // Thermal Clusters
                for (auto& [clusterId, clusterReserveParticipation]:
                     areaReserve.AllThermalReservesParticipation)
                {
                    reserveVariablesInitializer.initThermalReserveParticipation(
                      areaReserve.type,
                      pdt,
                      clusterReserveParticipation,
                      reserve);
                }

                // Short Term Storage Clusters
                for (auto& [clusterId, clusterReserveParticipation]:
                     areaReserve.AllSTStorageReservesParticipation)
                {
                    reserveVariablesInitializer.initSTStorageReserveParticipation(
                      areaReserve.type,
                      pdt,
                      clusterReserveParticipation,
                      reserve);
                }

                // Hydro
                for (auto& clusterReserveParticipation: areaReserve.AllHydroReservesParticipation)
                {
                    reserveVariablesInitializer.initHydroReserveParticipation(
                      areaReserve.type,
                      pdt,
                      clusterReserveParticipation,
                      reserve);
                }
            }
        }
    }
}
