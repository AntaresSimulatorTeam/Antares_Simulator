// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ReserveParticipationGroup.h"

#include "antares/solver/optimisation/constraints/ConstraintGroup.h"
#include "antares/solver/optimisation/constraints/HydroEnergyLevelReserveParticipation.h"
#include "antares/solver/optimisation/constraints/HydroGlobalEnergyLevelReserveParticipation.h"
#include "antares/solver/optimisation/constraints/HydroLevelReserveParticipation.h"
#include "antares/solver/optimisation/constraints/HydroReleaseCapacityThresholds.h"
#include "antares/solver/optimisation/constraints/HydroReleaseMaxReserve.h"
#include "antares/solver/optimisation/constraints/HydroReserveParticipation.h"
#include "antares/solver/optimisation/constraints/HydroStoreCapacityThresholds.h"
#include "antares/solver/optimisation/constraints/HydroStoreMaxReserve.h"
#include "antares/solver/optimisation/constraints/OffUnitsThermalParticipatingToReserves.h"
#include "antares/solver/optimisation/constraints/PMaxReserve.h"
#include "antares/solver/optimisation/constraints/POffUnits.h"
#include "antares/solver/optimisation/constraints/POutBounds.h"
#include "antares/solver/optimisation/constraints/POutCapacityThresholds.h"
#include "antares/solver/optimisation/constraints/ReserveSatisfaction.h"
#include "antares/solver/optimisation/constraints/STReleaseCapacityThresholds.h"
#include "antares/solver/optimisation/constraints/STReleaseMaxReserve.h"
#include "antares/solver/optimisation/constraints/STReserveParticipation.h"
#include "antares/solver/optimisation/constraints/STStorageEnergyLevelReserveParticipation.h"
#include "antares/solver/optimisation/constraints/STStorageGlobalEnergyLevelReserveParticipation.h"
#include "antares/solver/optimisation/constraints/STStorageLevelReserveParticipation.h"
#include "antares/solver/optimisation/constraints/STStoreCapacityThresholds.h"
#include "antares/solver/optimisation/constraints/STStoreMaxReserve.h"
#include "antares/solver/optimisation/constraints/SymmetryReserveParticipation.h"
#include "antares/solver/optimisation/constraints/ThermalReserveParticipation.h"

ReserveParticipationGroup::ReserveParticipationGroup(PROBLEME_HEBDO* problemeHebdo,
                                                     bool simulation,
                                                     ConstraintBuilder& builder):
    ConstraintGroup(problemeHebdo, builder)
{
    this->simulation_ = simulation;
}

ReserveData ReserveParticipationGroup::GetReserveDataFromProblemHebdo()
{
    return {.Simulation = simulation_,
            .areaReserves = problemeHebdo_->allReserves.value(),
            .thermalClusters = problemeHebdo_->PaliersThermiquesDuPays,
            .shortTermStorageOfArea = problemeHebdo_->ShortTermStorage,
            .hydroOfArea = problemeHebdo_->CaracteristiquesHydrauliques,
            .CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim};
}

/**
 * @brief build Reserve Participation constraints
 */
void ReserveParticipationGroup::BuildConstraints()
{
    {
        auto data = GetReserveDataFromProblemHebdo();
        PMaxReserve pMaxReserve(builder_, data);
        OffUnitsThermalParticipatingToReserves offUnitsThermalParticipatingToReserves(builder_,
                                                                                      data);
        POffUnits pOffUnits(builder_, data);
        ThermalReserveParticipation thermalReserveParticipation(builder_, data);
        ReserveSatisfaction reserveSatisfaction(builder_, data);
        STReleaseMaxReserve STReleaseMaxReserve(builder_, data);
        STStoreMaxReserve STStoreMaxReserve(builder_, data);
        STReserveParticipation STReserveParticipation(builder_, data);
        STStorageEnergyLevelReserveParticipation STStorageEnergyLevelReserveParticipation(builder_,
                                                                                          data);
        HydroEnergyLevelReserveParticipation HydroEnergyLevelReserveParticipation(builder_, data);
        HydroReleaseMaxReserve HydroReleaseMaxReserve(builder_, data);
        HydroStoreMaxReserve HydroStoreMaxReserve(builder_, data);
        HydroReserveParticipation HydroReserveParticipation(builder_, data);

        SymmetryReserveParticipation symmetryReserveParticipation(builder_, data);

        for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            // Adding constraints for ReservesUp and ReservesDown
            for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
            {
                // Thermal clusters reserve participations
                {
                    const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo_
                                                                          ->PaliersThermiquesDuPays
                                                                            [pays];
                    for (int cluster = 0;
                         cluster < PaliersThermiquesDuPays.NombreDePaliersThermiques;
                         cluster++)
                    {
                        // 16 quater
                        pOffUnits.add(pays, cluster, pdt);
                    }

                    uint32_t reserve = 0;
                    for (const auto& areaReserve: data.areaReserves[pays].areaCapacityReservations)
                    {
                        // 24
                        reserveSatisfaction.add(pays, reserve, pdt);

                        for (const auto& [clusterId, clusterReserveParticipation]:
                             areaReserve.AllThermalReservesParticipation)
                        {
                            // 16 bis
                            pMaxReserve.add(pays, reserve, clusterId, pdt);

                            if (areaReserve.type == ReserveType::UP
                                && clusterReserveParticipation.maxPowerOff > 0)
                            {
                                // 16 ter
                                offUnitsThermalParticipatingToReserves.add(pays,
                                                                           reserve,
                                                                           clusterId,
                                                                           pdt);
                            }

                            // 17 quinquies & sexies
                            thermalReserveParticipation.add(pays, reserve, clusterId, pdt);
                        }
                        reserve++;
                    }

                    // Thermal cluster Symmetries
                    for (const auto& [clusterId, symmetries]:
                         data.areaReserves[pays].ThermalReservesParticipationSymmetries)
                    {
                        // Add symmetry constraint between first element and all others
                        for (const auto& symmetry: symmetries)
                        {
                            // 15 (j)
                            symmetryReserveParticipation.add(pays, symmetry, pdt);
                        }
                    }
                }

                // ShortTerm Storage reserve participations
                {
                    uint32_t reserve = 0;
                    for (const auto& areaReserve: data.areaReserves[pays].areaCapacityReservations)
                    {
                        for (const auto& [clusterId, clusterReserveParticipation]:
                             areaReserve.AllSTStorageReservesParticipation)
                        {
                            // 15 (k)
                            STReleaseMaxReserve.add(pays,
                                                    reserve,
                                                    clusterReserveParticipation.clusterIdInArea,
                                                    pdt);
                            // 15 (l)
                            STStoreMaxReserve.add(pays,
                                                  reserve,
                                                  clusterReserveParticipation.clusterIdInArea,
                                                  pdt);
                            // 15 (o & p)
                            STReserveParticipation.add(pays,
                                                       reserve,
                                                       clusterReserveParticipation.clusterIdInArea,
                                                       pdt);

                            // 15 (h)
                            STStorageEnergyLevelReserveParticipation
                              .add(pays, clusterReserveParticipation.clusterIdInArea, reserve, pdt);
                        }
                        reserve++;
                    }

                    // ShortTerm Storage cluster Symmetries
                    for (const auto& [clusterId, symmetries]:
                         data.areaReserves[pays].STStorageReservesParticipationSymmetries)
                    {
                        for (const auto& symmetry: symmetries)
                        {
                            // 15 (j)
                            symmetryReserveParticipation.add(pays, symmetry, pdt);
                        }
                    }
                }

                // Hydro reserve participations
                {
                    uint32_t reserve = 0;
                    for (const auto& areaReserve: data.areaReserves[pays].areaCapacityReservations)
                    {
                        for (const auto& clusterReserveParticipation:
                             areaReserve.AllHydroReservesParticipation)
                        {
                            // 15 (a)
                            HydroReleaseMaxReserve.add(pays,
                                                       reserve,
                                                       clusterReserveParticipation.clusterIdInArea,
                                                       pdt);
                            // 15 (b)
                            HydroStoreMaxReserve.add(pays,
                                                     reserve,
                                                     clusterReserveParticipation.clusterIdInArea,
                                                     pdt);
                            // 15 (e & f)
                            HydroReserveParticipation
                              .add(pays, reserve, clusterReserveParticipation.clusterIdInArea, pdt);

                            // 15 (s)
                            HydroEnergyLevelReserveParticipation
                              .add(pays, clusterReserveParticipation.clusterIdInArea, reserve, pdt);
                        }
                        reserve++;
                    }
                }

                // Hydro Symmetries
                for (const auto& symmetry:
                     data.areaReserves[pays].HydroReservesParticipationSymmetries)
                {
                    // 15 (j)
                    symmetryReserveParticipation.add(pays, symmetry, pdt);
                }
            }
        }
    }

    {
        auto data = GetReserveDataFromProblemHebdo();
        POutCapacityThresholds pOutCapacityThresholds(builder_, data);
        POutBounds pOutBounds(builder_, data);
        STReleaseCapacityThresholds STReleaseCapacityThresholds(builder_, data);
        STStoreCapacityThresholds STStoreCapacityThresholds(builder_, data);
        HydroReleaseCapacityThresholds HydroReleaseCapacityThresholds(builder_, data);
        HydroStoreCapacityThresholds HydroStoreCapacityThresholds(builder_, data);
        HydroLevelReserveParticipation HydroLevelReserveParticipation(builder_, data);
        STStorageLevelReserveParticipation STStorageLevelReserveParticipation(builder_, data);
        STStorageGlobalEnergyLevelReserveParticipation
          STStorageGlobalEnergyLevelReserveParticipation(builder_, data);
        HydroGlobalEnergyLevelReserveParticipation HydroGlobalEnergyLevelReserveParticipation(
          builder_,
          data);

        for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
            {
                // Thermal Clusters
                const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo_
                                                                      ->PaliersThermiquesDuPays
                                                                        [pays];
                for (int cluster = 0; cluster < PaliersThermiquesDuPays.NombreDePaliersThermiques;
                     cluster++)
                {
                    // 17 bis
                    pOutCapacityThresholds.add(pays, cluster, pdt);

                    // 17 ter
                    pOutBounds.add(pays, cluster, pdt);
                }

                // Short Term Storage Clusters
                const auto& STStorageDuPays = problemeHebdo_->ShortTermStorage[pays];
                for (uint32_t cluster = 0; cluster < STStorageDuPays.size(); cluster++)
                {
                    // 15 (m)
                    STReleaseCapacityThresholds.add(pays, cluster, pdt);
                    // 15 (n)
                    STStoreCapacityThresholds.add(pays, cluster, pdt);
                    // 15 (g)
                    STStorageLevelReserveParticipation.add(pays, cluster, pdt);
                    // 15 (i)
                    STStorageGlobalEnergyLevelReserveParticipation.add(pays, cluster, pdt);
                }

                // Hydro
                // Check if the Hydro is participating to the reserves
                if (std::ranges::any_of(
                      problemeHebdo_->allReserves.value()[pays].areaCapacityReservations,
                      [](CAPACITY_RESERVATION& res)
                      { return res.AllHydroReservesParticipation.size() > 0; }))
                {
                    // 15 (c)
                    HydroReleaseCapacityThresholds.add(pays, pdt);
                    // 15 (d)
                    HydroStoreCapacityThresholds.add(pays, pdt);
                    // 15 (r)
                    HydroLevelReserveParticipation.add(pays, 0, pdt);
                    // 15 (t)
                    HydroGlobalEnergyLevelReserveParticipation.add(pays, 0, pdt);
                }
            }
        }
    }
}
