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

#include <antares/study/study.h>
#include "antares/solver/optimisation/opt_reserves.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_donnees.h"

using namespace Antares;
using namespace Antares::Data;

namespace
{
struct ReserveVariablesRightSidesSetter
{
    PROBLEME_HEBDO* problemeHebdo;
    const std::unique_ptr<PROBLEME_ANTARES_A_RESOUDRE>& ProblemeAResoudre;
    std::vector<double>& SecondMembre;
    std::vector<double*>& AdresseOuPlacerLaValeurDesCoutsMarginaux;

    int pdtJour = 0;
    int pdtHebdo = 0;
    int pdtGlobal = 0;
    int pays = 0;

    explicit ReserveVariablesRightSidesSetter(PROBLEME_HEBDO* hebdo):
        problemeHebdo(hebdo),
        ProblemeAResoudre(hebdo->ProblemeAResoudre),
        SecondMembre(ProblemeAResoudre->SecondMembre),
        AdresseOuPlacerLaValeurDesCoutsMarginaux(
          ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux)
    {
    }

    void setContext(int jour, int hebdo, int global, int p)
    {
        pdtJour = jour;
        pdtHebdo = hebdo;
        pdtGlobal = global;
        pays = p;
    }

    const auto& indices() const
    {
        return problemeHebdo->CorrespondanceCntNativesCntOptim[pdtJour].reservesIndices.value();
    }

    void setSecondMember(int cnt, double value, double* marginalAddress = nullptr)
    {
        if (cnt >= 0)
        {
            SecondMembre[cnt] = value;
            AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = marginalAddress;
        }
    }

    // Set the right sides of equations for a reserve
    void setReserveRightSides(const CAPACITY_RESERVATION& reserve)
    {
        const auto& CorrespondanceCntNativesCntOptim = problemeHebdo
                                                         ->CorrespondanceCntNativesCntOptim
                                                           [pdtJour];
        int cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .need[reserve.globalReserveIndex];

        setSecondMember(cnt,
                        reserve.need.at(pdtGlobal),
                        &(problemeHebdo->ResultatsHoraires[pays]
                            .Reserves.value()[pdtHebdo]
                            .CoutsMarginauxHoraires[reserve.areaReserveIndex]));
    }

    // Set the right sides of equations for a Thermal cluster participation to a reserve up
    void setThermalReserveUpParticipationRightSides(
      const RESERVE_PARTICIPATION_THERMAL& reserveParticipation)
    {
        const auto& CorrespondanceCntNativesCntOptim = problemeHebdo
                                                         ->CorrespondanceCntNativesCntOptim
                                                           [pdtJour];
        int cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .powerOffGroupUnitsInThermalClusterParticipating
                      [reserveParticipation.globalIndexClusterParticipation];

        setSecondMember(cnt,
                        problemeHebdo->PaliersThermiquesDuPays[pays]
                            .PuissanceDisponibleEtCout[reserveParticipation.clusterIdInArea]
                            .NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtJour]
                          * reserveParticipation.maxPowerOff);
    }

    // Set the right sides of equations for a Thermal cluster
    void setThermalClusterRightSides(const int areaClusterId)
    {
        const auto& CorrespondanceCntNativesCntOptim = problemeHebdo
                                                         ->CorrespondanceCntNativesCntOptim
                                                           [pdtJour];
        int globalClusterIdx = problemeHebdo->PaliersThermiquesDuPays[pays]
                                 .NumeroDuPalierDansLEnsembleDesPaliersThermiques[areaClusterId];

        int cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .thermalClusterPOutBoundMin[globalClusterIdx];
        setSecondMember(cnt,
                        problemeHebdo->PaliersThermiquesDuPays[pays]
                          .PuissanceDisponibleEtCout[areaClusterId]
                          .PuissanceMinDuPalierThermiqueRef[pdtJour]);

        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                .thermalClusterPOutBoundMax[globalClusterIdx];

        setSecondMember(cnt,
                        problemeHebdo->PaliersThermiquesDuPays[pays]
                          .PuissanceDisponibleEtCout[areaClusterId]
                          .PuissanceDisponibleDuPalierThermiqueRef[pdtJour]);

        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                .maxPowerOffUnitsInThermalCluster[globalClusterIdx];

        setSecondMember(cnt,
                        problemeHebdo->PaliersThermiquesDuPays[pays]
                            .PuissanceDisponibleEtCout[areaClusterId]
                            .NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtJour]
                          * problemeHebdo->PaliersThermiquesDuPays[pays]
                              .PmaxDUnGroupeDuPalierThermique[areaClusterId]);
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

        setSecondMember(cnt,
                        cluster.series->maxWithdrawalModulation[pdtGlobal]
                          * cluster.withdrawalNominalCapacity);

        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                .STStorageClusterReleaseCapacityThresholdsMin[cluster.clusterGlobalIndex];

        setSecondMember(cnt,
                        cluster.series->lowerRuleCurve[pdtGlobal]
                          * cluster.withdrawalNominalCapacity);

        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                .STStorageClusterStoreCapacityThresholds[cluster.clusterGlobalIndex];

        setSecondMember(cnt,
                        cluster.series->maxInjectionModulation[pdtGlobal]
                          * cluster.injectionNominalCapacity);

        double level_max = cluster.reservoirCapacity * cluster.series->upperRuleCurve[pdtGlobal];
        double level_min = cluster.reservoirCapacity * cluster.series->lowerRuleCurve[pdtGlobal];

        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                .STStorageLevelParticipation.down[cluster.clusterGlobalIndex];

        setSecondMember(cnt, level_max);

        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                .STStorageLevelParticipation.up[cluster.clusterGlobalIndex];
        setSecondMember(cnt, -level_min);

        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                .STStorageGlobalStockEnergyLevelParticipation.down[cluster.clusterGlobalIndex];
        setSecondMember(cnt,
                        reserves.referenceGlobalActivationDuration.down
                          * reserves.maxGlobalEnergyActivationRatio.down * level_max);

        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                .STStorageGlobalStockEnergyLevelParticipation.up[cluster.clusterGlobalIndex];
        setSecondMember(cnt,
                        -reserves.referenceGlobalActivationDuration.up
                          * reserves.maxGlobalEnergyActivationRatio.up * level_min);
    }

    // Set the right sides of equations for a ShortTerm cluster participation to a reserve
    void setSTStorageReserveParticipationRightSides(
      const RESERVE_PARTICIPATION_STSTORAGE& reserveParticipation,
      const CAPACITY_RESERVATION& reserve,
      ReserveType type)
    {
        const auto& CorrespondanceCntNativesCntOptim = problemeHebdo
                                                         ->CorrespondanceCntNativesCntOptim
                                                           [pdtJour];
        int cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                    .STStorageClusterMaxReleaseParticipation[reserveParticipation
                                                               .globalIndexClusterParticipation];

        setSecondMember(cnt, reserveParticipation.maxRelease);

        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                .STStorageClusterMaxStoreParticipation[reserveParticipation
                                                         .globalIndexClusterParticipation];
        setSecondMember(cnt, reserveParticipation.maxStore);

        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                .STStorageEnergyLevelParticipation[reserveParticipation
                                                     .globalIndexClusterParticipation];
        auto& cluster = problemeHebdo->ShortTermStorage[pays][reserveParticipation.clusterIdInArea];
        if (type == ReserveType::UP)
        {
            double level_min = cluster.reservoirCapacity
                               * cluster.series->lowerRuleCurve[pdtGlobal];
            setSecondMember(cnt,
                            -reserve.energyActivationRatio * reserve.referenceActivationDuration
                              * level_min);
        }
        else
        {
            double level_max = cluster.reservoirCapacity
                               * cluster.series->upperRuleCurve[pdtGlobal];
            setSecondMember(cnt,
                            reserve.energyActivationRatio * reserve.referenceActivationDuration
                              * level_max);
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
        setSecondMember(cnt, hydroCluster.ContrainteDePmaxHydrauliqueHoraire[pdtJour]);

        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                .HydroReleaseCapacityThresholdsMin[globalClusterIdx];
        setSecondMember(cnt, hydroCluster.MingenHoraire[pdtJour]);

        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                .HydroStoreCapacityThresholds[globalClusterIdx];
        setSecondMember(cnt, hydroCluster.ContrainteDePmaxPompageHoraire[pdtJour]);

        double level_max = hydroCluster.NiveauHoraireSup[pdtHebdo];
        double level_min = hydroCluster.NiveauHoraireInf[pdtHebdo];
        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                .HydroLevelParticipation.down[globalClusterIdx];
        setSecondMember(cnt, level_max);

        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                .HydroLevelParticipation.up[globalClusterIdx];
        setSecondMember(cnt, level_min);

        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                .HydroGlobalEnergyLevelParticipationDown[globalClusterIdx];
        setSecondMember(cnt,
                        reserves.referenceGlobalActivationDuration.down
                          * reserves.maxGlobalEnergyActivationRatio.down * level_max);

        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                .HydroGlobalEnergyLevelParticipationUp[globalClusterIdx];
        setSecondMember(cnt,
                        -reserves.referenceGlobalActivationDuration.up
                          * reserves.maxGlobalEnergyActivationRatio.up * level_min);
    }

    // Set the right sides of equations for a Hydro participation to a reserve
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
        setSecondMember(cnt, reserveParticipation.maxRelease);

        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value()
                .HydroMaxStoreParticipation[reserveParticipation.globalIndexClusterParticipation];
        setSecondMember(cnt, reserveParticipation.maxStore);

        cnt = CorrespondanceCntNativesCntOptim.reservesIndices.value().HydroEnergyLevelParticipation
                [reserveParticipation.globalIndexClusterParticipation];

        auto& hydroCluster = problemeHebdo->CaracteristiquesHydrauliques[pays];
        if (type == ReserveType::UP)
        {
            double level_min = hydroCluster.NiveauHoraireInf[pdtHebdo];
            setSecondMember(cnt,
                            -reserve.energyActivationRatio * reserve.referenceActivationDuration
                              * level_min);
        }
        else
        {
            double level_max = hydroCluster.NiveauHoraireSup[pdtHebdo];
            setSecondMember(cnt,
                            reserve.energyActivationRatio * reserve.referenceActivationDuration
                              * level_max);
        }
    }
};
} // anonymous namespace

void OPT_InitialiserLeSecondMembreDuProblemeLineaireReserves(PROBLEME_HEBDO* problemeHebdo,
                                                             int PremierPdtDeLIntervalle,
                                                             int DernierPdtDeLIntervalle)
{
    ReserveVariablesRightSidesSetter setter(problemeHebdo);

    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         ++pdtHebdo, ++pdtJour)
    {
        int pdtGlobal = problemeHebdo->weekInTheYear * problemeHebdo->NombreDePasDeTempsDUneJournee
                          * problemeHebdo->NombreDeJours
                        + pdtHebdo;

        for (int pays = 0; pays < problemeHebdo->NombreDePays; ++pays)
        {
            setter.setContext(pdtJour, pdtHebdo, pdtGlobal, pays);

            const auto& areaReserves = problemeHebdo->allReserves.value()[pays];

            for (const auto& areaReserve: areaReserves.areaCapacityReservations)
            {
                setter.setReserveRightSides(areaReserve);

                if (areaReserve.type == ReserveType::UP)
                {
                    for (const auto& [_, participation]:
                         areaReserve.AllThermalReservesParticipation)
                    {
                        setter.setThermalReserveUpParticipationRightSides(participation);
                    }
                }

                for (const auto& [_, participation]: areaReserve.AllSTStorageReservesParticipation)
                {
                    setter.setSTStorageReserveParticipationRightSides(participation,
                                                                      areaReserve,
                                                                      areaReserve.type);
                }

                for (const auto& participation: areaReserve.AllHydroReservesParticipation)
                {
                    setter.setHydroReserveParticipationRightSides(participation,
                                                                  areaReserve,
                                                                  areaReserve.type);
                }
            }

            // Thermal clusters
            for (int cluster = 0;
                 cluster < problemeHebdo->PaliersThermiquesDuPays[pays].NombreDePaliersThermiques;
                 ++cluster)
            {
                setter.setThermalClusterRightSides(cluster);
            }

            // ST storage clusters
            for (const auto& cluster: problemeHebdo->ShortTermStorage[pays])
            {
                setter.setSTStorageClusterRightSides(cluster, areaReserves);
            }

            // Hydro (if participating)
            if (std::ranges::any_of(areaReserves.areaCapacityReservations,
                                    [](const CAPACITY_RESERVATION& res)
                                    { return !res.AllHydroReservesParticipation.empty(); }))
            {
                setter.setHydroRightSides(areaReserves);
            }
        }
    }
}
