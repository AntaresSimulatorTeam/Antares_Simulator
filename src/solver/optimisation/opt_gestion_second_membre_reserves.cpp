// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/opt_gestion_second_membre_reserves.h"

#include <antares/study/study.h>
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_donnees.h"

using namespace Antares;
using namespace Antares::Data;

void OPT_InitialiserLeSecondMembreDuProblemeLineaireReserves(PROBLEME_HEBDO* problemeHebdo,
                                                             int PremierPdtDeLIntervalle,
                                                             int DernierPdtDeLIntervalle)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    std::vector<double>& SecondMembre = ProblemeAResoudre->SecondMembre;

    std::vector<double*>& AdresseOuPlacerLaValeurDesCoutsMarginaux
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux;

    struct ReserveVariablesRightSidesSetter
    {
        PROBLEME_HEBDO* problemeHebdo;
        const std::unique_ptr<PROBLEME_ANTARES_A_RESOUDRE>& ProblemeAResoudre;
        std::vector<double>& SecondMembre;
        std::vector<double*>& AdresseOuPlacerLaValeurDesCoutsMarginaux;
        int pdtJour = 0, pdtHebdo = 0, pdtGlobal = 0, pays = 0;

        ReserveVariablesRightSidesSetter(PROBLEME_HEBDO* hebdo):
            problemeHebdo(hebdo),
            ProblemeAResoudre(hebdo->ProblemeAResoudre),
            SecondMembre(ProblemeAResoudre->SecondMembre),
            AdresseOuPlacerLaValeurDesCoutsMarginaux(
              ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux)
        {
        }

        void setPdtJour(int pdt)
        {
            pdtJour = pdt;
        }

        void setPdtHebdo(int pdt)
        {
            pdtHebdo = pdt;
        }

        void setPdtGlobal(int pdt)
        {
            pdtGlobal = pdt;
        }

        void setPays(int p)
        {
            pays = p;
        }

        // Set the rigth sides of equations for a reserve
        void setReserveRightSides(const CAPACITY_RESERVATION& reserve)
        {
            const auto& CorrespondanceCntNativesCntOptim = problemeHebdo
                                                             ->CorrespondanceCntNativesCntOptim
                                                               [pdtJour];
            int cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                        .need[reserve.globalReserveIndex];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = reserve.need.at(pdtGlobal);
                double* adresseDuResultat = &(problemeHebdo->ResultatsHoraires[pays]
                                                .Reserves.value()[pdtHebdo]
                                                .CoutsMarginauxHoraires[reserve.areaReserveIndex]);
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = adresseDuResultat;
            }
        }

        // Set the rigth sides of equations for a Thermal cluster participation to a reserve up
        void setThermalReserveUpParticipationRightSides(
          const RESERVE_PARTICIPATION_THERMAL& reserveParticipation)
        {
            const auto& CorrespondanceCntNativesCntOptim = problemeHebdo
                                                             ->CorrespondanceCntNativesCntOptim
                                                               [pdtJour];
            int cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                        .powerOffGroupUnitsInThermalClusterParticipating
                          [reserveParticipation.globalIndexClusterParticipation];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = problemeHebdo->PaliersThermiquesDuPays[pays]
                                      .PuissanceDisponibleEtCout[reserveParticipation
                                                                   .clusterIdInArea]
                                      .NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtJour]
                                    * reserveParticipation.maxPowerOff;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }

        // Set the rigth sides of equations for a Thermal cluster
        void setThermalClusterRightSides(const int areaClusterId)
        {
            const auto& CorrespondanceCntNativesCntOptim = problemeHebdo
                                                             ->CorrespondanceCntNativesCntOptim
                                                               [pdtJour];
            int globalClusterIdx = problemeHebdo->PaliersThermiquesDuPays[pays]
                                     .NumeroDuPalierDansLEnsembleDesPaliersThermiques
                                       [areaClusterId];

            int cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                        .thermalClusterPOutBoundMin[globalClusterIdx];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = problemeHebdo->PaliersThermiquesDuPays[pays]
                                      .PuissanceDisponibleEtCout[areaClusterId]
                                      .PuissanceMinDuPalierThermiqueRef[pdtJour];
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }

            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .thermalClusterPOutBoundMax[globalClusterIdx];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = problemeHebdo->PaliersThermiquesDuPays[pays]
                                      .PuissanceDisponibleEtCout[areaClusterId]
                                      .PuissanceDisponibleDuPalierThermiqueRef[pdtJour];
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }

            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .maxPowerOffUnitsInThermalCluster[globalClusterIdx];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = problemeHebdo->PaliersThermiquesDuPays[pays]
                                      .PuissanceDisponibleEtCout[areaClusterId]
                                      .NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtJour]
                                    * problemeHebdo->PaliersThermiquesDuPays[pays]
                                        .PmaxDUnGroupeDuPalierThermique[areaClusterId];
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }

        // Common setter for the ShortTerm Storage clusters
        void setSTStorageClusterRightSides(const ::PROPERTIES& cluster,
                                           const ::AREA_RESERVES_VECTOR& reserves)
        {
            const auto& CorrespondanceCntNativesCntOptim = problemeHebdo
                                                             ->CorrespondanceCntNativesCntOptim
                                                               [pdtJour];
            int cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                        .STStorageClusterReleaseCapacityThresholdsMax[cluster.clusterGlobalIndex];

            if (cnt >= 0)
            {
                SecondMembre[cnt] = cluster.series.get()->maxWithdrawalModulation[pdtGlobal]
                                    * cluster.withdrawalNominalCapacity;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }

            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .STStorageClusterReleaseCapacityThresholdsMin[cluster.clusterGlobalIndex];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = cluster.series.get()->lowerRuleCurve[pdtGlobal]
                                    * cluster.withdrawalNominalCapacity;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }

            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .STStorageClusterStoreCapacityThresholds[cluster.clusterGlobalIndex];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = cluster.series.get()->maxInjectionModulation[pdtGlobal]
                                    * cluster.injectionNominalCapacity;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
            double level_max = cluster.reservoirCapacity
                               * cluster.series->upperRuleCurve[pdtGlobal];
            double level_min = cluster.reservoirCapacity
                               * cluster.series->lowerRuleCurve[pdtGlobal];

            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .STStorageLevelParticipation.down[cluster.clusterGlobalIndex];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = level_max;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }

            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .STStorageLevelParticipation.up[cluster.clusterGlobalIndex];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = -level_min;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }

            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .STStorageGlobalStockEnergyLevelParticipation.down[cluster.clusterGlobalIndex];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = reserves.referenceGlobalActivationDuration.down
                                    * reserves.maxGlobalEnergyActivationRatio.down * level_max;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }

            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .STStorageGlobalStockEnergyLevelParticipation.up[cluster.clusterGlobalIndex];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = -reserves.referenceGlobalActivationDuration.up
                                    * reserves.maxGlobalEnergyActivationRatio.up * level_min;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }

        // Set the rigth sides of equations for a ShortTerm cluster participation to a reserve
        void setSTStorageReserveParticipationRightSides(
          const RESERVE_PARTICIPATION_STSTORAGE& reserveParticipation,
          const CAPACITY_RESERVATION& reserve,
          ReserveType type)
        {
            const auto& CorrespondanceCntNativesCntOptim = problemeHebdo
                                                             ->CorrespondanceCntNativesCntOptim
                                                               [pdtJour];
            int cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                        .STStorageClusterMaxReleaseParticipation
                          [reserveParticipation.globalIndexClusterParticipation];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = reserveParticipation.maxRelease;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .STStorageClusterMaxStoreParticipation[reserveParticipation
                                                             .globalIndexClusterParticipation];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = reserveParticipation.maxStore;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .STStorageEnergyLevelParticipation[reserveParticipation
                                                         .globalIndexClusterParticipation];
            if (cnt >= 0)
            {
                auto& cluster = problemeHebdo
                                  ->ShortTermStorage[pays][reserveParticipation.clusterIdInArea];
                if (type == ReserveType::UP)
                {
                    double level_min = cluster.reservoirCapacity
                                       * cluster.series->lowerRuleCurve[pdtGlobal];

                    SecondMembre[cnt] = -reserve.energyActivationRatio
                                        * reserve.referenceActivationDuration * level_min;
                }
                else
                {
                    double level_max = cluster.reservoirCapacity
                                       * cluster.series->upperRuleCurve[pdtGlobal];
                    SecondMembre[cnt] = reserve.energyActivationRatio
                                        * reserve.referenceActivationDuration * level_max;
                }
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }

        // Common setter for the Hydro
        void setHydroRightSides(const ::AREA_RESERVES_VECTOR& reserves)
        {
            const auto& CorrespondanceCntNativesCntOptim = problemeHebdo
                                                             ->CorrespondanceCntNativesCntOptim
                                                               [pdtJour];
            auto& hydroCluster = problemeHebdo->CaracteristiquesHydrauliques[pays];
            int globalClusterIdx = hydroCluster.GlobalHydroIndex;

            int cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                        .HydroReleaseCapacityThresholdsMax[globalClusterIdx];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = hydroCluster.ContrainteDePmaxHydrauliqueHoraire[pdtJour];
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }

            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .HydroReleaseCapacityThresholdsMin[globalClusterIdx];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = hydroCluster.MingenHoraire[pdtJour];
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }

            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .HydroStoreCapacityThresholds[globalClusterIdx];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = hydroCluster.ContrainteDePmaxPompageHoraire[pdtJour];
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }

            double level_max = hydroCluster.NiveauHoraireSup[pdtHebdo];
            double level_min = hydroCluster.NiveauHoraireInf[pdtHebdo];
            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .HydroLevelParticipation.down[globalClusterIdx];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = hydroCluster.NiveauHoraireSup[pdtHebdo];
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }

            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .HydroLevelParticipation.up[globalClusterIdx];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = -hydroCluster.NiveauHoraireInf[pdtHebdo];
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }

            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .HydroGlobalEnergyLevelParticipationDown[globalClusterIdx];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = reserves.referenceGlobalActivationDuration.down
                                    * reserves.maxGlobalEnergyActivationRatio.down * level_max;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }

            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .HydroGlobalEnergyLevelParticipationUp[globalClusterIdx];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = -reserves.referenceGlobalActivationDuration.up
                                    * reserves.maxGlobalEnergyActivationRatio.up * level_min;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }

        // Set the rigth sides of equations for a Hydro participation to a reserve
        void setHydroReserveParticipationRightSides(
          const RESERVE_PARTICIPATION_HYDRO& reserveParticipation,
          const CAPACITY_RESERVATION& reserve,
          ReserveType type)
        {
            const auto& CorrespondanceCntNativesCntOptim = problemeHebdo
                                                             ->CorrespondanceCntNativesCntOptim
                                                               [pdtJour];
            int cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                        .HydroMaxReleaseParticipation[reserveParticipation
                                                        .globalIndexClusterParticipation];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = reserveParticipation.maxRelease;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .HydroMaxStoreParticipation[reserveParticipation
                                                  .globalIndexClusterParticipation];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = reserveParticipation.maxStore;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
            cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .HydroEnergyLevelParticipation[reserveParticipation
                                                     .globalIndexClusterParticipation];
            if (cnt >= 0)
            {
                auto& hydroCluster = problemeHebdo->CaracteristiquesHydrauliques[pays];
                if (type == ReserveType::UP)
                {
                    double level_min = hydroCluster.NiveauHoraireInf[pdtHebdo];

                    SecondMembre[cnt] = -reserve.energyActivationRatio
                                        * reserve.referenceActivationDuration * level_min;
                }
                else
                {
                    double level_max = hydroCluster.NiveauHoraireSup[pdtHebdo];
                    SecondMembre[cnt] = reserve.energyActivationRatio
                                        * reserve.referenceActivationDuration * level_max;
                }

                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }
    };

    ReserveVariablesRightSidesSetter reserveVariablesRightSidesSetter(problemeHebdo);

    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        int pdtGlobal = problemeHebdo->weekInTheYear * problemeHebdo->NombreDePasDeTempsDUneJournee
                          * problemeHebdo->NombreDeJours
                        + pdtHebdo;

        reserveVariablesRightSidesSetter.setPdtJour(pdtJour);
        reserveVariablesRightSidesSetter.setPdtHebdo(pdtHebdo);
        reserveVariablesRightSidesSetter.setPdtGlobal(pdtGlobal);

        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            reserveVariablesRightSidesSetter.setPays(pays);
            const auto& areaReserves = problemeHebdo->allReserves.value()[pays];

            // Up Reserves Right Sides
            for (const auto& areaReserve: areaReserves.areaCapacityReservations)
            {
                reserveVariablesRightSidesSetter.setReserveRightSides(areaReserve);

                // Thermal Cluster
                if (areaReserve.type == ReserveType::UP)
                {
                    for (const auto& [clusterId, clusterReserveParticipation]:
                         areaReserve.AllThermalReservesParticipation)
                    {
                        reserveVariablesRightSidesSetter.setThermalReserveUpParticipationRightSides(
                          clusterReserveParticipation);
                    }
                }

                // Short Term Storage Cluster
                for (const auto& [clusterId, clusterReserveParticipation]:
                     areaReserve.AllSTStorageReservesParticipation)
                {
                    reserveVariablesRightSidesSetter.setSTStorageReserveParticipationRightSides(
                      clusterReserveParticipation,
                      areaReserve,
                      areaReserve.type);
                }

                // Hydro
                for (const auto& clusterReserveParticipation:
                     areaReserve.AllHydroReservesParticipation)
                {
                    reserveVariablesRightSidesSetter.setHydroReserveParticipationRightSides(
                      clusterReserveParticipation,
                      areaReserve,
                      areaReserve.type);
                }
            }

            // Thermal Clusters
            for (int cluster = 0;
                 cluster < problemeHebdo->PaliersThermiquesDuPays[pays].NombreDePaliersThermiques;
                 cluster++)
            {
                reserveVariablesRightSidesSetter.setThermalClusterRightSides(cluster);
            }

            // Short Term Storage clusters
            for (const auto& cluster: problemeHebdo->ShortTermStorage[pays])
            {
                reserveVariablesRightSidesSetter.setSTStorageClusterRightSides(cluster,
                                                                               areaReserves);
            }

            // Hydro
            // Checks if Hydro is participating to reserves
            if (std::ranges::any_of(
                  problemeHebdo->allReserves.value()[pays].areaCapacityReservations,
                  [](CAPACITY_RESERVATION& res)
                  { return res.AllHydroReservesParticipation.size() > 0; }))
            {
                reserveVariablesRightSidesSetter.setHydroRightSides(areaReserves);
            }
        }
    }

    return;
}
