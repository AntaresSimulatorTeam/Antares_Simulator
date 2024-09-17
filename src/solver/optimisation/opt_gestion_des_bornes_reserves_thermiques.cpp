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

#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "variables/VariableManagement.h"
#include "variables/VariableManagerUtils.h"
#include "antares/solver/simulation/simulation.h"
#include "antares/solver/simulation/sim_structure_donnees.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/optimisation/opt_fonctions.h"

using namespace Yuni;

void OPT_InitialiserLesBornesDesVariablesDuProblemeLineaireReservesThermiques(
  PROBLEME_HEBDO* problemeHebdo,
  const int PremierPdtDeLIntervalle,
  const int DernierPdtDeLIntervalle)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    std::vector<double>& Xmin = ProblemeAResoudre->Xmin;
    std::vector<double>& Xmax = ProblemeAResoudre->Xmax;

    std::vector<double*>& AdresseOuPlacerLaValeurDesVariablesOptimisees
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees;

    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        const CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
          = problemeHebdo->CorrespondanceVarNativesVarOptim[pdtJour];

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            auto areaReserves = problemeHebdo->allReserves[pays];

            for (const auto& areaReserveUp : areaReserves.areaCapacityReservationsUp)
            {
                int var = CorrespondanceVarNativesVarOptim
                            .internalUnsatisfiedReserveIndex[areaReserveUp.globalReserveIndex];
                Xmin[var] = 0;
                Xmax[var] = LINFINI_ANTARES;
                double* adresseDuResultat1
                  = &(problemeHebdo->ResultatsHoraires[pays]
                        .Reserves[pdtHebdo]
                        .ValeursHorairesInternalUnsatisfied[areaReserveUp.areaReserveIndex]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat1;

                var = CorrespondanceVarNativesVarOptim
                        .internalExcessReserveIndex[areaReserveUp.globalReserveIndex];
                Xmin[var] = 0;
                Xmax[var] = LINFINI_ANTARES;
                double* adresseDuResultat2
                  = &(problemeHebdo->ResultatsHoraires[pays]
                        .Reserves[pdtHebdo]
                        .ValeursHorairesInternalExcessReserve[areaReserveUp.areaReserveIndex]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat2;

                // Thermal Cluster
                for (const auto& clusterReserveParticipation :
                     areaReserveUp.AllThermalReservesParticipation)
                {
                    if (clusterReserveParticipation.maxPower > 0)
                    {
                        var = CorrespondanceVarNativesVarOptim
                                .runningThermalClusterReserveParticipationIndex
                                  [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat3
                          = &(problemeHebdo->ResultatsHoraires[pays]
                                .ProductionThermique[pdtHebdo]
                                .ParticipationReservesDuPalierOn[clusterReserveParticipation
                                                                   .areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat3;
                    }
                    if (clusterReserveParticipation.maxPowerOff > 0)
                    {
                        var = CorrespondanceVarNativesVarOptim
                                .offThermalClusterReserveParticipationIndex
                                  [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat3 = &(
                          problemeHebdo->ResultatsHoraires[pays]
                            .ProductionThermique[pdtHebdo]
                            .ParticipationReservesDuPalierOff[clusterReserveParticipation
                                                                .areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat3;

                        var = CorrespondanceVarNativesVarOptim
                                .nbOffGroupUnitsParticipatingToReservesInThermalClusterIndex
                                  [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat4
                          = &(problemeHebdo->ResultatsHoraires[pays]
                                .ProductionThermique[pdtHebdo]
                                .NombreDeGroupesEteintDuPalierQuiParticipentAuxReserves
                                  [clusterReserveParticipation.areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat4;
                    }
                    if (clusterReserveParticipation.maxPower > 0
                        || clusterReserveParticipation.maxPowerOff > 0)
                    {
                        var
                          = CorrespondanceVarNativesVarOptim.thermalClusterReserveParticipationIndex
                              [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat3
                          = &(problemeHebdo->ResultatsHoraires[pays]
                                .ProductionThermique[pdtHebdo]
                                .ParticipationReservesDuPalier[clusterReserveParticipation
                                                                 .areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat3;
                    }
                }

                // Short Term Storage Cluster
                for (const auto& clusterReserveParticipation :
                     areaReserveUp.AllSTStorageReservesParticipation)
                {
                    if (clusterReserveParticipation.maxTurbining > 0)
                    {
                        var = CorrespondanceVarNativesVarOptim
                                .STStorageTurbiningClusterReserveParticipationIndex
                                  [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat
                          = &(problemeHebdo->ResultatsHoraires[pays]
                                .ShortTermStorage[pdtHebdo]
                                .reserveParticipationOfCluster[clusterReserveParticipation
                                                                 .areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
                    }
                    if (clusterReserveParticipation.maxPumping > 0)
                    {
                        var = CorrespondanceVarNativesVarOptim
                                .STStoragePumpingClusterReserveParticipationIndex
                                  [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat
                          = &(problemeHebdo->ResultatsHoraires[pays]
                                .ShortTermStorage[pdtHebdo]
                                .reserveParticipationOfCluster[clusterReserveParticipation
                                                                 .areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
                    }
                    if ((clusterReserveParticipation.maxPumping > 0)
                        || (clusterReserveParticipation.maxTurbining > 0))
                    {
                        var = CorrespondanceVarNativesVarOptim
                                .STStorageClusterReserveUpParticipationIndex
                                  [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat1
                          = &(problemeHebdo->ResultatsHoraires[pays]
                                .ShortTermStorage[pdtHebdo]
                                .reserveParticipationOfCluster[clusterReserveParticipation
                                                                 .areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat1;
                    }
                }

                // Long Term Storage Cluster
                for (const auto& clusterReserveParticipation :
                     areaReserveUp.AllLTStorageReservesParticipation)
                {
                    if (clusterReserveParticipation.maxTurbining > 0)
                    {
                        var = CorrespondanceVarNativesVarOptim
                                .LTStorageTurbiningClusterReserveParticipationIndex
                                  [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat
                          = &(problemeHebdo->ResultatsHoraires[pays]
                                .HydroUsage[pdtHebdo]
                                .reserveParticipationOfCluster[clusterReserveParticipation.areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
                    }
                    if (clusterReserveParticipation.maxPumping > 0)
                    {
                        var = CorrespondanceVarNativesVarOptim
                                .LTStoragePumpingClusterReserveParticipationIndex
                                  [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat
                          = &(problemeHebdo->ResultatsHoraires[pays]
                                .HydroUsage[pdtHebdo]
                                .reserveParticipationOfCluster[clusterReserveParticipation.areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
                    }
                    if ((clusterReserveParticipation.maxPumping > 0)
                        || (clusterReserveParticipation.maxTurbining > 0))
                    {
                        var = CorrespondanceVarNativesVarOptim
                                .LTStorageClusterReserveUpParticipationIndex
                                  [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat1
                          = &(problemeHebdo->ResultatsHoraires[pays]
                                .HydroUsage[pdtHebdo]
                                .reserveParticipationOfCluster[clusterReserveParticipation.areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat1;
                    }
                }
            }
            for (const auto& areaReserveDown : areaReserves.areaCapacityReservationsDown)
            {
                int var = CorrespondanceVarNativesVarOptim
                            .internalUnsatisfiedReserveIndex[areaReserveDown.globalReserveIndex];
                Xmin[var] = 0;
                Xmax[var] = LINFINI_ANTARES;
                double* adresseDuResultat1
                  = &(problemeHebdo->ResultatsHoraires[pays]
                        .Reserves[pdtHebdo]
                        .ValeursHorairesInternalUnsatisfied[areaReserveDown.areaReserveIndex]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat1;

                var = CorrespondanceVarNativesVarOptim
                        .internalExcessReserveIndex[areaReserveDown.globalReserveIndex];
                Xmin[var] = 0;
                Xmax[var] = LINFINI_ANTARES;
                double* adresseDuResultat2
                  = &(problemeHebdo->ResultatsHoraires[pays]
                        .Reserves[pdtHebdo]
                        .ValeursHorairesInternalExcessReserve[areaReserveDown.areaReserveIndex]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat2;

                // Thermal Clusters
                for (const auto& clusterReserveParticipation :
                     areaReserveDown.AllThermalReservesParticipation)
                {
                    if (clusterReserveParticipation.maxPower > 0)
                    {
                        var = CorrespondanceVarNativesVarOptim
                                .runningThermalClusterReserveParticipationIndex
                                  [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat3
                          = &(problemeHebdo->ResultatsHoraires[pays]
                                .ProductionThermique[pdtHebdo]
                                .ParticipationReservesDuPalierOn[clusterReserveParticipation
                                                                   .areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat3;

                        var
                          = CorrespondanceVarNativesVarOptim.thermalClusterReserveParticipationIndex
                              [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat4
                          = &(problemeHebdo->ResultatsHoraires[pays]
                                .ProductionThermique[pdtHebdo]
                                .ParticipationReservesDuPalier[clusterReserveParticipation
                                                                 .areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat4;
                    }
                }

                // Short Term Storage Cluster
                for (const auto& clusterReserveParticipation :
                     areaReserveDown.AllSTStorageReservesParticipation)
                {
                    if (clusterReserveParticipation.maxTurbining > 0)
                    {
                        var = CorrespondanceVarNativesVarOptim
                                .STStorageTurbiningClusterReserveParticipationIndex
                                  [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat
                          = &(problemeHebdo->ResultatsHoraires[pays]
                                .ShortTermStorage[pdtHebdo]
                                .reserveParticipationOfCluster[clusterReserveParticipation
                                                                 .areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
                    }
                    if (clusterReserveParticipation.maxPumping > 0)
                    {
                        var = CorrespondanceVarNativesVarOptim
                                .STStoragePumpingClusterReserveParticipationIndex
                                  [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat
                          = &(problemeHebdo->ResultatsHoraires[pays]
                                .ShortTermStorage[pdtHebdo]
                                .reserveParticipationOfCluster[clusterReserveParticipation
                                                                 .areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
                    }
                    if ((clusterReserveParticipation.maxPumping > 0)
                        || (clusterReserveParticipation.maxTurbining > 0))
                    {
                        var = CorrespondanceVarNativesVarOptim
                                .STStorageClusterReserveDownParticipationIndex
                                  [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat
                          = &(problemeHebdo->ResultatsHoraires[pays]
                                .ShortTermStorage[pdtHebdo]
                                .reserveParticipationOfCluster[clusterReserveParticipation
                                                                 .areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
                    }
                }

                // Long Term Storage Cluster
                for (const auto& clusterReserveParticipation :
                     areaReserveDown.AllLTStorageReservesParticipation)
                {
                    if (clusterReserveParticipation.maxTurbining > 0)
                    {
                        var = CorrespondanceVarNativesVarOptim
                                .LTStorageTurbiningClusterReserveParticipationIndex
                                  [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat
                          = &(problemeHebdo->ResultatsHoraires[pays]
                                .HydroUsage[pdtHebdo]
                                .reserveParticipationOfCluster[clusterReserveParticipation.areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
                    }
                    if (clusterReserveParticipation.maxPumping > 0)
                    {
                        var = CorrespondanceVarNativesVarOptim
                                .LTStoragePumpingClusterReserveParticipationIndex
                                  [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat
                          = &(problemeHebdo->ResultatsHoraires[pays]
                                .HydroUsage[pdtHebdo]
                                .reserveParticipationOfCluster[clusterReserveParticipation.areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
                    }
                    if ((clusterReserveParticipation.maxPumping > 0)
                        || (clusterReserveParticipation.maxTurbining > 0))
                    {
                        var = CorrespondanceVarNativesVarOptim
                                .LTStorageClusterReserveDownParticipationIndex
                                  [clusterReserveParticipation.globalIndexClusterParticipation];
                        Xmin[var] = 0;
                        Xmax[var] = LINFINI_ANTARES;
                        double* adresseDuResultat
                          = &(problemeHebdo->ResultatsHoraires[pays]
                                .HydroUsage[pdtHebdo]
                                .reserveParticipationOfCluster[clusterReserveParticipation.areaIndexClusterParticipation]);
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
                    }
                }
            }
        }
    }
}
