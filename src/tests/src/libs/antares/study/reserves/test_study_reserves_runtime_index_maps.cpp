// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE study
#include <algorithm>
#include <fstream>
#include <files-system.h>
#include <filesystem>

#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <antares/solver/simulation/forTestsOnlySimCalculEco.h>
#include <antares/solver/simulation/sim_structure_probleme_economique.h>
#include <antares/study/area/forTestsOnlyList.h>
#include <antares/study/study.h>
#include "antares/study/runtime/runtime.h"

using namespace Antares::Data;

/*!
 * \brief Helper to add a thermal cluster to an area.
 */
static void addThermalCluster(Area* area, const std::string& name)
{
    auto c = std::make_shared<ThermalCluster>(area);
    c->setName(name);
    area->thermal.list.addToCompleteList(c);
}

/*!
 * \brief Helper to add a short-term storage cluster to an area.
 */
static void addShortTermStorage(Area* area, const std::string& name)
{
    ShortTermStorage::STStorageCluster cluster;
    cluster.properties.name = name;
    cluster.id = name;
    area->shortTermStorage.storagesByIndex.push_back(cluster);
}

/*!
 * \brief Helper to populate thermal participations in PROBLEME_HEBDO.
 *
 * \note importCapacityReservations iterates over a std::map sorted by reserveID,
 *       so "reservedown" (index 0) comes before "reserveup" (index 1).
 */
static void populateThermalParticipation(PROBLEME_HEBDO& problemeHebdo,
                                         int areaIndex,
                                         int reserveArrayIndex,
                                         int clusterId,
                                         const std::string& clusterName,
                                         int globalIdx,
                                         int areaIdx)
{
    auto& areaReserves = problemeHebdo.allReserves.value()[areaIndex];
    RESERVE_PARTICIPATION_THERMAL part;
    part.participationCost = 5.0;
    part.maxPower = 10.0;
    part.maxPowerOff = 0.0;
    part.participationCostOff = 0.0;
    part.clusterName = clusterName;
    part.clusterIdInArea = clusterId;
    part.clusterId = clusterId;
    part.globalIndexClusterParticipation = globalIdx;
    part.areaIndexClusterParticipation = areaIdx;
    areaReserves.areaCapacityReservations[reserveArrayIndex]
      .AllThermalReservesParticipation.emplace(clusterId, part);
}

/*!
 * \brief Helper to populate ST storage participations in PROBLEME_HEBDO.
 */
static void populateSTSParticipation(PROBLEME_HEBDO& problemeHebdo,
                                     int areaIndex,
                                     int reserveArrayIndex,
                                     const std::string& clusterName,
                                     int globalIdx,
                                     int areaIdx)
{
    auto& areaReserves = problemeHebdo.allReserves.value()[areaIndex];
    RESERVE_PARTICIPATION_STSTORAGE part;
    part.participationCost = 6.6;
    part.maxStore = 5.5;
    part.maxRelease = 4.4;
    part.clusterName = clusterName;
    part.clusterIdInArea = 0;
    part.clusterId = 0;
    part.globalIndexClusterParticipation = globalIdx;
    part.areaIndexClusterParticipation = areaIdx;
    areaReserves.areaCapacityReservations[reserveArrayIndex]
      .AllSTStorageReservesParticipation.emplace(0, part);
}

/*!
 * \brief Base fixture: study with one area "A" and two capacity reservations
 *        (ReserveUp, ReserveDown). Derived fixtures add clusters and participations.
 */
struct ReservesIndexMapsFixture
{
    ReservesIndexMapsFixture()
    {
        problemeHebdo = std::make_unique<PROBLEME_HEBDO>();
        study = std::make_unique<Study>();
        study->parameters.include.reserves = true;
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;

        areaA = addAreaToListOfAreas(study->areas, "A");

        // Set up two capacity reservations
        CapacityReservation reserveUp;
        reserveUp.type = ReserveType::UP;
        reserveUp.unsuppliedCost = 1000.;
        reserveUp.referenceActivationDuration = 2;
        reserveUp.powerActivationRatio = 0.5;
        reserveUp.energyActivationRatio = 1.0;
        reserveUp.setName("ReserveUp");

        CapacityReservation reserveDown;
        reserveDown.type = ReserveType::DOWN;
        reserveDown.unsuppliedCost = 800.;
        reserveDown.referenceActivationDuration = 4;
        reserveDown.powerActivationRatio = 0.3;
        reserveDown.energyActivationRatio = 0.8;
        reserveDown.setName("ReserveDown");

        areaA->allCapacityReservations = AllCapacityReservations();
        areaA->allCapacityReservations.value()
          .areaCapacityReservations.emplace(reserveUp.id(), reserveUp);
        areaA->allCapacityReservations.value()
          .areaCapacityReservations.emplace(reserveDown.id(), reserveDown);
    }

    std::unique_ptr<PROBLEME_HEBDO> problemeHebdo;
    std::unique_ptr<Study> study;
    Area* areaA = nullptr;
};

/*!
 * \brief Extended fixture: two areas with thermal clusters and different reserves.
 */
struct ReservesIndexMapsTwoAreasFixture : ReservesIndexMapsFixture
{
    ReservesIndexMapsTwoAreasFixture()
    {
        areaB = addAreaToListOfAreas(study->areas, "B");
        addThermalCluster(areaA, "ThermalA1");
        addThermalCluster(areaA, "ThermalA2");
        addThermalCluster(areaB, "ThermalB1");

        // Area A already has ReserveUp and ReserveDown from parent fixture
        // but we need to rename them for the two-area scenario
        areaA->allCapacityReservations.value().areaCapacityReservations.clear();

        CapacityReservation reserveUpA;
        reserveUpA.type = ReserveType::UP;
        reserveUpA.unsuppliedCost = 1000.;
        reserveUpA.referenceActivationDuration = 2;
        reserveUpA.powerActivationRatio = 0.5;
        reserveUpA.energyActivationRatio = 1.0;
        reserveUpA.setName("ReserveUpA");

        CapacityReservation reserveDownA;
        reserveDownA.type = ReserveType::DOWN;
        reserveDownA.unsuppliedCost = 800.;
        reserveDownA.referenceActivationDuration = 4;
        reserveDownA.powerActivationRatio = 0.3;
        reserveDownA.energyActivationRatio = 0.8;
        reserveDownA.setName("ReserveDownA");

        areaA->allCapacityReservations.value()
          .areaCapacityReservations.emplace(reserveUpA.id(), reserveUpA);
        areaA->allCapacityReservations.value()
          .areaCapacityReservations.emplace(reserveDownA.id(), reserveDownA);

        // Area B: only ReserveUpB
        CapacityReservation reserveUpB;
        reserveUpB.type = ReserveType::UP;
        reserveUpB.unsuppliedCost = 1100.;
        reserveUpB.referenceActivationDuration = 3;
        reserveUpB.powerActivationRatio = 0.6;
        reserveUpB.energyActivationRatio = 1.1;
        reserveUpB.setName("ReserveUpB");

        areaB->allCapacityReservations = AllCapacityReservations();
        areaB->allCapacityReservations.value()
          .areaCapacityReservations.emplace(reserveUpB.id(), reserveUpB);
    }

    Area* areaB = nullptr;
};

BOOST_AUTO_TEST_SUITE(reserves_runtime_index_maps)

// ============================================================================
// Test: initializeReservesIndexMaps with no reserve participations
// Validates that maps are initialized but empty when no participations exist.
// ============================================================================
BOOST_FIXTURE_TEST_CASE(initializeReservesIndexMaps_no_participations,
                        ReservesIndexMapsFixture)
{
    importCapacityReservations(study->areas, *problemeHebdo);

    StudyRuntimeInfos runtimeInfos;
    runtimeInfos.initializeReservesIndexMaps(*study, *problemeHebdo);

    BOOST_CHECK(runtimeInfos.reserveParticipationIndexMaps.has_value());
    BOOST_CHECK(runtimeInfos.reserveIDToName.has_value());
    BOOST_CHECK(runtimeInfos.reserveParticipationIndexMaps.value().contains("a"));

    // Verify reserve ID to name mapping
    BOOST_CHECK_EQUAL(runtimeInfos.reserveIDToName.value().size(), 2);
    BOOST_CHECK_EQUAL(runtimeInfos.reserveIDToName.value().at("reserveup"), "ReserveUp");
    BOOST_CHECK_EQUAL(runtimeInfos.reserveIDToName.value().at("reservedown"), "ReserveDown");

    // Index maps should be empty
    auto& aMaps = runtimeInfos.reserveParticipationIndexMaps.value().at("a");
    BOOST_CHECK_EQUAL(aMaps.thermalClusters.size(), 0);
    BOOST_CHECK_EQUAL(aMaps.thermalClustersByIndex.size(), 0);
    BOOST_CHECK_EQUAL(aMaps.STStorageClusters.size(), 0);
    BOOST_CHECK_EQUAL(aMaps.STStorageClustersByIndex.size(), 0);
    BOOST_CHECK_EQUAL(aMaps.Hydro.size(), 0);
    BOOST_CHECK_EQUAL(aMaps.HydroByIndex.size(), 0);
}

// ============================================================================
// Test: initializeReservesIndexMaps with mixed participations (thermal + STS + hydro)
// Validates all three participation types and their reverse mappings in a single area.
// ============================================================================
BOOST_FIXTURE_TEST_CASE(initializeReservesIndexMaps_mixed_participations,
                        ReservesIndexMapsFixture)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    addThermalCluster(areaA, "Thermal1");
    addShortTermStorage(areaA, "STStorage1");

    // Create thermal reserve participation YAML
    std::ofstream fileThermal(studyPath / "thermal.yml");
    fileThermal << R"(participations:
  - cluster: Thermal1
    certifications:
      - reserve: ReserveUp
        max-power: 10.0
        participation-cost: 5.0
)";
    fileThermal.close();

    // Create ST storage reserve participation YAML
    std::ofstream fileSTS(studyPath / "sts.yml");
    fileSTS << R"(participations:
  - storage: STStorage1
    certifications:
      - reserve: ReserveDown
        participation-cost: 6.6
        max-store: 5.5
        max-release: 4.4
)";
    fileSTS.close();

    // Create hydro reserve participation YAML
    std::ofstream fileHydro(studyPath / "hydro.yml");
    fileHydro << R"(participations:
  certifications:
    - reserve: ReserveUp
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
    - reserve: ReserveDown
      participation-cost: 6.6
      max-store: 5.5
      max-release: 4.4
)";
    fileHydro.close();

    // Load reserve participations
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "thermal.yml");
    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "sts.yml");
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "hydro.yml");

    importCapacityReservations(study->areas, *problemeHebdo);
    importHydroReserves(study->areas, *problemeHebdo);

    // Manually populate thermal and STS participations
    // Note: "reservedown" (index 0) < "reserveup" (index 1) alphabetically
    int indexA = study->areas.find("a")->index;
    {
        auto& areaReserves = problemeHebdo->allReserves.value()[indexA];
        populateThermalParticipation(*problemeHebdo, indexA, 0, 0, "Thermal1", 0, 0);
        populateSTSParticipation(*problemeHebdo, indexA, 1, "STStorage1", 1, 1);
    }

    StudyRuntimeInfos runtimeInfos;
    runtimeInfos.initializeReservesIndexMaps(*study, *problemeHebdo);

    auto& aMaps = runtimeInfos.reserveParticipationIndexMaps.value().at("a");

    // Verify all three map types are populated
    BOOST_CHECK_EQUAL(aMaps.thermalClusters.size(), 1);
    BOOST_CHECK_EQUAL(aMaps.STStorageClusters.size(), 1);
    BOOST_CHECK_EQUAL(aMaps.Hydro.size(), 2);

    // Verify reverse mapping consistency for each type
    for (const auto& [key, idx]: aMaps.thermalClusters)
    {
        BOOST_CHECK(aMaps.thermalClustersByIndex.contains(idx));
        auto revKey = aMaps.thermalClustersByIndex.at(idx);
        BOOST_CHECK_EQUAL(revKey.first, key.first);
        BOOST_CHECK_EQUAL(revKey.second, key.second);
    }
    for (const auto& [key, idx]: aMaps.STStorageClusters)
    {
        BOOST_CHECK(aMaps.STStorageClustersByIndex.contains(idx));
        auto revKey = aMaps.STStorageClustersByIndex.at(idx);
        BOOST_CHECK_EQUAL(revKey.first, key.first);
        BOOST_CHECK_EQUAL(revKey.second, key.second);
    }
    for (const auto& [reserveID, idx]: aMaps.Hydro)
    {
        BOOST_CHECK(aMaps.HydroByIndex.contains(idx));
        BOOST_CHECK_EQUAL(aMaps.HydroByIndex.at(idx), reserveID);
    }
}

// ============================================================================
// Test: initializeReservesIndexMaps with two areas and different reserves
// Validates that each area gets its own independent index maps.
// ============================================================================
BOOST_FIXTURE_TEST_CASE(initializeReservesIndexMaps_two_areas_different_reserves,
                        ReservesIndexMapsTwoAreasFixture)
{
    importCapacityReservations(study->areas, *problemeHebdo);

    int indexA = study->areas.find("a")->index;
    int indexB = study->areas.find("b")->index;

    // Area A: ThermalA1 participates in ReserveUpA (index 1, since "reserveup" > "reservedown")
    populateThermalParticipation(*problemeHebdo, indexA, 1, 0, "ThermalA1", 0, 0);

    // Area B: ThermalB1 participates in ReserveUpB (index 0, only one reserve)
    populateThermalParticipation(*problemeHebdo, indexB, 0, 0, "ThermalB1", 1, 0);

    StudyRuntimeInfos runtimeInfos;
    runtimeInfos.initializeReservesIndexMaps(*study, *problemeHebdo);

    // Both areas have entries
    BOOST_CHECK(runtimeInfos.reserveParticipationIndexMaps.value().contains("a"));
    BOOST_CHECK(runtimeInfos.reserveParticipationIndexMaps.value().contains("b"));

    // Area A: 1 thermal cluster for ReserveUpA
    auto& aMaps = runtimeInfos.reserveParticipationIndexMaps.value().at("a");
    BOOST_CHECK_EQUAL(aMaps.thermalClusters.size(), 1);
    BOOST_CHECK(aMaps.thermalClusters.find({"reserveupa", "ThermalA1"})
                != aMaps.thermalClusters.end());

    // Area B: 1 thermal cluster for ReserveUpB
    auto& bMaps = runtimeInfos.reserveParticipationIndexMaps.value().at("b");
    BOOST_CHECK_EQUAL(bMaps.thermalClusters.size(), 1);
    BOOST_CHECK(bMaps.thermalClusters.find({"reserveupb", "ThermalB1"})
                != bMaps.thermalClusters.end());
}

// ============================================================================
// Test: initializeReservesIndexMaps with empty area list (edge case)
// Validates graceful handling when no areas are present.
// ============================================================================
BOOST_AUTO_TEST_CASE(initializeReservesIndexMaps_empty_study)
{
    Study study;
    study.parameters.include.reserves = true;
    study.parameters.simulationDays.first = 0;
    study.parameters.simulationDays.end = 7;

    PROBLEME_HEBDO problemeHebdo;
    problemeHebdo.allReserves = std::vector<::AREA_RESERVES_VECTOR>();

    StudyRuntimeInfos runtimeInfos;
    runtimeInfos.initializeReservesIndexMaps(study, problemeHebdo);

    BOOST_CHECK(runtimeInfos.reserveParticipationIndexMaps.has_value());
    BOOST_CHECK(runtimeInfos.reserveIDToName.has_value());
    BOOST_CHECK_EQUAL(runtimeInfos.reserveParticipationIndexMaps.value().size(), 0);
    BOOST_CHECK_EQUAL(runtimeInfos.reserveIDToName.value().size(), 0);
}

// ============================================================================
// Test: initializeReservesIndexMaps when reserves are disabled
// Validates that maps are initialized even when reserves are disabled in study params.
// ============================================================================
BOOST_FIXTURE_TEST_CASE(initializeReservesIndexMaps_reserves_disabled,
                        ReservesIndexMapsFixture)
{
    study->parameters.include.reserves = false;
    importCapacityReservations(study->areas, *problemeHebdo);

    StudyRuntimeInfos runtimeInfos;
    runtimeInfos.initializeReservesIndexMaps(*study, *problemeHebdo);

    BOOST_CHECK(runtimeInfos.reserveParticipationIndexMaps.has_value());
    BOOST_CHECK(runtimeInfos.reserveIDToName.has_value());
    BOOST_CHECK_EQUAL(runtimeInfos.reserveParticipationIndexMaps.value().size(), 1);
}

BOOST_AUTO_TEST_SUITE_END()
