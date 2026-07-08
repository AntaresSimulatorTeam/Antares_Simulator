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
 * \brief Helper to add a thermal cluster to an area
 */
static void addThermalCluster(Area* area, const std::string& name)
{
    auto c = std::make_shared<ThermalCluster>(area);
    c->setName(name);
    area->thermal.list.addToCompleteList(c);
}

/*!
 * \brief Helper to add a short-term storage cluster to an area
 */
static void addShortTermStorage(Area* area, const std::string& name)
{
    ShortTermStorage::STStorageCluster cluster;
    cluster.properties.name = name;
    cluster.id = name;
    area->shortTermStorage.storagesByIndex.push_back(cluster);
}

/*!
 * \brief Minimal fixture: study with one area "A", reserves enabled,
 *        PROBLEME_HEBDO with capacity reservations but NO reserve participations.
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
 * \brief Fixture with two areas, each with thermal clusters participating in reserves.
 */
struct ReservesIndexMapsTwoAreasFixture
{
    ReservesIndexMapsTwoAreasFixture()
    {
        problemeHebdo = std::make_unique<PROBLEME_HEBDO>();
        study = std::make_unique<Study>();
        study->parameters.include.reserves = true;
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;

        areaA = addAreaToListOfAreas(study->areas, "A");
        areaB = addAreaToListOfAreas(study->areas, "B");

        // Add thermal clusters
        addThermalCluster(areaA, "ThermalA1");
        addThermalCluster(areaA, "ThermalA2");
        addThermalCluster(areaB, "ThermalB1");

        // Set up capacity reservations for area A
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

        areaA->allCapacityReservations = AllCapacityReservations();
        areaA->allCapacityReservations.value()
          .areaCapacityReservations.emplace(reserveUpA.id(), reserveUpA);
        areaA->allCapacityReservations.value()
          .areaCapacityReservations.emplace(reserveDownA.id(), reserveDownA);

        // Set up capacity reservations for area B
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

    std::unique_ptr<PROBLEME_HEBDO> problemeHebdo;
    std::unique_ptr<Study> study;
    Area* areaA = nullptr;
    Area* areaB = nullptr;
};

BOOST_AUTO_TEST_SUITE(reserves_runtime_index_maps)

// ============================================================================
// Test: initializeReservesIndexMaps with no reserve participations
// ============================================================================
BOOST_FIXTURE_TEST_CASE(initializeReservesIndexMaps_no_participations,
                        ReservesIndexMapsFixture)
{
    // Import capacity reservations into PROBLEME_HEBDO
    importCapacityReservations(study->areas, *problemeHebdo);

    // Verify that allReserves is populated
    BOOST_CHECK(problemeHebdo->allReserves.has_value());
    int indexA = study->areas.find("a")->index;
    BOOST_CHECK_EQUAL(problemeHebdo->allReserves.value()[indexA].areaCapacityReservations.size(),
                      2);

    // Initialize runtime infos from study
    StudyRuntimeInfos runtimeInfos;

    // Initialize reserve index maps
    runtimeInfos.initializeReservesIndexMaps(*study, *problemeHebdo);

    // Verify that the maps are initialized
    BOOST_CHECK(runtimeInfos.reserveParticipationIndexMaps.has_value());
    BOOST_CHECK(runtimeInfos.reserveIDToName.has_value());

    // Verify that the area "a" has an entry
    BOOST_CHECK(runtimeInfos.reserveParticipationIndexMaps.value().contains("a"));

    // Verify that reserve IDs are mapped to names
    BOOST_CHECK_EQUAL(runtimeInfos.reserveIDToName.value().size(), 2);
    BOOST_CHECK_EQUAL(runtimeInfos.reserveIDToName.value().at("reserveup"), "ReserveUp");
    BOOST_CHECK_EQUAL(runtimeInfos.reserveIDToName.value().at("reservedown"), "ReserveDown");

    // Since there are no reserve participations, the index maps should be empty
    auto& aMaps = runtimeInfos.reserveParticipationIndexMaps.value().at("a");
    BOOST_CHECK_EQUAL(aMaps.thermalClusters.size(), 0);
    BOOST_CHECK_EQUAL(aMaps.thermalClustersByIndex.size(), 0);
    BOOST_CHECK_EQUAL(aMaps.STStorageClusters.size(), 0);
    BOOST_CHECK_EQUAL(aMaps.STStorageClustersByIndex.size(), 0);
    BOOST_CHECK_EQUAL(aMaps.Hydro.size(), 0);
    BOOST_CHECK_EQUAL(aMaps.HydroByIndex.size(), 0);
}

// ============================================================================
// Test: initializeReservesIndexMaps with thermal cluster participations
// Manually populate PROBLEME_HEBDO since importThermalReserves is not exported.
// ============================================================================
BOOST_FIXTURE_TEST_CASE(initializeReservesIndexMaps_with_thermal_participations,
                        ReservesIndexMapsTwoAreasFixture)
{
    // Import capacity reservations into PROBLEME_HEBDO
    importCapacityReservations(study->areas, *problemeHebdo);

    // Manually populate AllThermalReservesParticipation in PROBLEME_HEBDO
    // to simulate what importThermalReserves would do.
    int indexA = study->areas.find("a")->index;
    int indexB = study->areas.find("b")->index;

    // Area A: ThermalA1 participates in ReserveUpA and ReserveDownA
    // Area A: ThermalA2 participates in ReserveUpA
    {
        auto& areaReserves = problemeHebdo->allReserves.value()[indexA];
        BOOST_CHECK_EQUAL(areaReserves.areaCapacityReservations.size(), 2);

        // Note: importCapacityReservations iterates over a std::map sorted by reserveID,
        // so "reservedowna" (index 0) comes before "reserveupa" (index 1)
        // ReserveDownA (index 0) - ThermalA1 participates
        RESERVE_PARTICIPATION_THERMAL partA1Down;
        partA1Down.participationCost = 4.0;
        partA1Down.maxPower = 8.0;
        partA1Down.maxPowerOff = 0.0;
        partA1Down.participationCostOff = 0.0;
        partA1Down.clusterName = "ThermalA1";
        partA1Down.clusterIdInArea = 0;
        partA1Down.clusterId = 0;
        partA1Down.globalIndexClusterParticipation = 0;
        partA1Down.areaIndexClusterParticipation = 0;
        areaReserves.areaCapacityReservations[0]
          .AllThermalReservesParticipation.emplace(0, partA1Down);

        // ReserveUpA (index 1) - ThermalA1 participates
        RESERVE_PARTICIPATION_THERMAL partA1Up;
        partA1Up.participationCost = 5.0;
        partA1Up.maxPower = 10.0;
        partA1Up.maxPowerOff = 0.0;
        partA1Up.participationCostOff = 0.0;
        partA1Up.clusterName = "ThermalA1";
        partA1Up.clusterIdInArea = 0;
        partA1Up.clusterId = 0;
        partA1Up.globalIndexClusterParticipation = 1;
        partA1Up.areaIndexClusterParticipation = 1;
        areaReserves.areaCapacityReservations[1]
          .AllThermalReservesParticipation.emplace(0, partA1Up);

        // ReserveUpA (index 1) - ThermalA2 participates
        RESERVE_PARTICIPATION_THERMAL partA2Up;
        partA2Up.participationCost = 6.0;
        partA2Up.maxPower = 12.0;
        partA2Up.maxPowerOff = 0.0;
        partA2Up.participationCostOff = 0.0;
        partA2Up.clusterName = "ThermalA2";
        partA2Up.clusterIdInArea = 1;
        partA2Up.clusterId = 1;
        partA2Up.globalIndexClusterParticipation = 2;
        partA2Up.areaIndexClusterParticipation = 2;
        areaReserves.areaCapacityReservations[1]
          .AllThermalReservesParticipation.emplace(1, partA2Up);
    }

    // Area B: ThermalB1 participates in ReserveUpB
    {
        auto& areaReserves = problemeHebdo->allReserves.value()[indexB];
        BOOST_CHECK_EQUAL(areaReserves.areaCapacityReservations.size(), 1);

        RESERVE_PARTICIPATION_THERMAL partB1;
        partB1.participationCost = 7.0;
        partB1.maxPower = 15.0;
        partB1.maxPowerOff = 0.0;
        partB1.participationCostOff = 0.0;
        partB1.clusterName = "ThermalB1";
        partB1.clusterIdInArea = 0;
        partB1.clusterId = 0;
        partB1.globalIndexClusterParticipation = 3;
        partB1.areaIndexClusterParticipation = 0;
        areaReserves.areaCapacityReservations[0]
          .AllThermalReservesParticipation.emplace(0, partB1);
    }

    // Initialize runtime infos from study
    StudyRuntimeInfos runtimeInfos;

    // Initialize reserve index maps
    runtimeInfos.initializeReservesIndexMaps(*study, *problemeHebdo);

    // Verify that the maps are initialized
    BOOST_CHECK(runtimeInfos.reserveParticipationIndexMaps.has_value());
    BOOST_CHECK(runtimeInfos.reserveIDToName.has_value());

    // Verify area A maps
    auto& aMaps = runtimeInfos.reserveParticipationIndexMaps.value().at("a");
    BOOST_CHECK_EQUAL(aMaps.thermalClusters.size(), 3);
    BOOST_CHECK_EQUAL(aMaps.thermalClustersByIndex.size(), 3);

    // Verify area B maps
    auto& bMaps = runtimeInfos.reserveParticipationIndexMaps.value().at("b");
    BOOST_CHECK_EQUAL(bMaps.thermalClusters.size(), 1);
    BOOST_CHECK_EQUAL(bMaps.thermalClustersByIndex.size(), 1);

    // Verify reverse mapping consistency for area A
    for (const auto& [key, idx]: aMaps.thermalClusters)
    {
        BOOST_CHECK(aMaps.thermalClustersByIndex.contains(idx));
        auto revKey = aMaps.thermalClustersByIndex.at(idx);
        BOOST_CHECK_EQUAL(revKey.first, key.first);
        BOOST_CHECK_EQUAL(revKey.second, key.second);
    }

    // Verify reverse mapping consistency for area B
    for (const auto& [key, idx]: bMaps.thermalClusters)
    {
        BOOST_CHECK(bMaps.thermalClustersByIndex.contains(idx));
        auto revKey = bMaps.thermalClustersByIndex.at(idx);
        BOOST_CHECK_EQUAL(revKey.first, key.first);
        BOOST_CHECK_EQUAL(revKey.second, key.second);
    }
}

// ============================================================================
// Test: initializeReservesIndexMaps with ST storage participations
// Manually populate PROBLEME_HEBDO since importSTStorageReserves is not exported.
// ============================================================================
BOOST_FIXTURE_TEST_CASE(initializeReservesIndexMaps_with_sts_participations,
                        ReservesIndexMapsFixture)
{
    // Import capacity reservations into PROBLEME_HEBDO
    importCapacityReservations(study->areas, *problemeHebdo);

    // Manually populate AllSTStorageReservesParticipation in PROBLEME_HEBDO
    int indexA = study->areas.find("a")->index;
    {
        auto& areaReserves = problemeHebdo->allReserves.value()[indexA];
        BOOST_CHECK_EQUAL(areaReserves.areaCapacityReservations.size(), 2);

        // Note: importCapacityReservations iterates over a std::map sorted by reserveID,
        // so "reservedown" (index 0) comes before "reserveup" (index 1)
        // ReserveDown (index 0) - STStorage1 participates
        RESERVE_PARTICIPATION_STSTORAGE partDown;
        partDown.participationCost = 6.6;
        partDown.maxStore = 5.5;
        partDown.maxRelease = 4.4;
        partDown.clusterName = "STStorage1";
        partDown.clusterIdInArea = 0;
        partDown.clusterId = 0;
        partDown.globalIndexClusterParticipation = 0;
        partDown.areaIndexClusterParticipation = 0;
        areaReserves.areaCapacityReservations[0]
          .AllSTStorageReservesParticipation.emplace(0, partDown);

        // ReserveUp (index 1) - STStorage1 participates
        RESERVE_PARTICIPATION_STSTORAGE partUp;
        partUp.participationCost = 9.9;
        partUp.maxStore = 8.8;
        partUp.maxRelease = 7.7;
        partUp.clusterName = "STStorage1";
        partUp.clusterIdInArea = 0;
        partUp.clusterId = 0;
        partUp.globalIndexClusterParticipation = 1;
        partUp.areaIndexClusterParticipation = 1;
        areaReserves.areaCapacityReservations[1]
          .AllSTStorageReservesParticipation.emplace(0, partUp);
    }

    // Initialize runtime infos from study
    StudyRuntimeInfos runtimeInfos;

    // Initialize reserve index maps
    runtimeInfos.initializeReservesIndexMaps(*study, *problemeHebdo);

    // Verify that the maps are initialized
    BOOST_CHECK(runtimeInfos.reserveParticipationIndexMaps.has_value());

    // Verify ST storage maps
    auto& aMaps = runtimeInfos.reserveParticipationIndexMaps.value().at("a");
    BOOST_CHECK_EQUAL(aMaps.STStorageClusters.size(), 2);
    BOOST_CHECK_EQUAL(aMaps.STStorageClustersByIndex.size(), 2);

    // Verify reverse mapping consistency
    for (const auto& [key, idx]: aMaps.STStorageClusters)
    {
        BOOST_CHECK(aMaps.STStorageClustersByIndex.contains(idx));
        auto revKey = aMaps.STStorageClustersByIndex.at(idx);
        BOOST_CHECK_EQUAL(revKey.first, key.first);
        BOOST_CHECK_EQUAL(revKey.second, key.second);
    }
}

// ============================================================================
// Test: initializeReservesIndexMaps with hydro participations
// Uses the exported importHydroReserves function.
// ============================================================================
BOOST_FIXTURE_TEST_CASE(initializeReservesIndexMaps_with_hydro_participations,
                        ReservesIndexMapsFixture)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    // Create reserve participation YAML for hydro
    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
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
    file.close();

    // Load reserve participations
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");

    // Import capacity reservations into PROBLEME_HEBDO
    importCapacityReservations(study->areas, *problemeHebdo);

    // Import hydro reserves
    importHydroReserves(study->areas, *problemeHebdo);

    // Initialize runtime infos from study
    StudyRuntimeInfos runtimeInfos;

    // Initialize reserve index maps
    runtimeInfos.initializeReservesIndexMaps(*study, *problemeHebdo);

    // Verify that the maps are initialized
    BOOST_CHECK(runtimeInfos.reserveParticipationIndexMaps.has_value());

    // Verify hydro maps
    auto& aMaps = runtimeInfos.reserveParticipationIndexMaps.value().at("a");
    BOOST_CHECK_EQUAL(aMaps.Hydro.size(), 2);
    BOOST_CHECK_EQUAL(aMaps.HydroByIndex.size(), 2);

    // Verify reverse mapping consistency
    for (const auto& [reserveID, idx]: aMaps.Hydro)
    {
        BOOST_CHECK(aMaps.HydroByIndex.contains(idx));
        BOOST_CHECK_EQUAL(aMaps.HydroByIndex.at(idx), reserveID);
    }
}

// ============================================================================
// Test: initializeReservesIndexMaps with mixed participations (thermal + STS + hydro)
// ============================================================================
BOOST_FIXTURE_TEST_CASE(initializeReservesIndexMaps_mixed_participations,
                        ReservesIndexMapsFixture)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    // Add a thermal cluster
    addThermalCluster(areaA, "Thermal1");

    // Add a short-term storage cluster
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

    // Import capacity reservations into PROBLEME_HEBDO
    importCapacityReservations(study->areas, *problemeHebdo);

    // Import hydro reserves
    importHydroReserves(study->areas, *problemeHebdo);

    // Manually populate thermal and STS participations
    int indexA = study->areas.find("a")->index;
    {
        auto& areaReserves = problemeHebdo->allReserves.value()[indexA];
        BOOST_CHECK_EQUAL(areaReserves.areaCapacityReservations.size(), 2);

        // Note: importCapacityReservations iterates over a std::map sorted by reserveID,
        // so "reservedowna" (index 0) comes before "reserveupa" (index 1)
        // Thermal1 participates in ReserveDown (index 0)
        RESERVE_PARTICIPATION_THERMAL thermalPart;
        thermalPart.participationCost = 5.0;
        thermalPart.maxPower = 10.0;
        thermalPart.maxPowerOff = 0.0;
        thermalPart.participationCostOff = 0.0;
        thermalPart.clusterName = "Thermal1";
        thermalPart.clusterIdInArea = 0;
        thermalPart.clusterId = 0;
        thermalPart.globalIndexClusterParticipation = 0;
        thermalPart.areaIndexClusterParticipation = 0;
        areaReserves.areaCapacityReservations[0]
          .AllThermalReservesParticipation.emplace(0, thermalPart);

        // STStorage1 participates in ReserveUp (index 1)
        RESERVE_PARTICIPATION_STSTORAGE stsPart;
        stsPart.participationCost = 6.6;
        stsPart.maxStore = 5.5;
        stsPart.maxRelease = 4.4;
        stsPart.clusterName = "STStorage1";
        stsPart.clusterIdInArea = 0;
        stsPart.clusterId = 0;
        stsPart.globalIndexClusterParticipation = 1;
        stsPart.areaIndexClusterParticipation = 1;
        areaReserves.areaCapacityReservations[1]
          .AllSTStorageReservesParticipation.emplace(0, stsPart);
    }

    // Initialize runtime infos from study
    StudyRuntimeInfos runtimeInfos;

    // Initialize reserve index maps
    runtimeInfos.initializeReservesIndexMaps(*study, *problemeHebdo);

    // Verify that the maps are initialized
    BOOST_CHECK(runtimeInfos.reserveParticipationIndexMaps.has_value());

    // Verify all maps are populated
    auto& aMaps = runtimeInfos.reserveParticipationIndexMaps.value().at("a");
    BOOST_CHECK_EQUAL(aMaps.thermalClusters.size(), 1);
    BOOST_CHECK_EQUAL(aMaps.STStorageClusters.size(), 1);
    BOOST_CHECK_EQUAL(aMaps.Hydro.size(), 2);

    // Verify reverse mapping consistency for all map types
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
// ============================================================================
BOOST_FIXTURE_TEST_CASE(initializeReservesIndexMaps_two_areas_different_reserves,
                        ReservesIndexMapsTwoAreasFixture)
{
    // Import capacity reservations into PROBLEME_HEBDO
    importCapacityReservations(study->areas, *problemeHebdo);

    // Manually populate thermal participations
    int indexA = study->areas.find("a")->index;
    int indexB = study->areas.find("b")->index;

    // Note: importCapacityReservations iterates over a std::map sorted by reserveID,
    // so "reservedowna" (index 0) comes before "reserveupa" (index 1)
    // Area A: ThermalA1 participates in ReserveUpA (index 1)
    {
        auto& areaReserves = problemeHebdo->allReserves.value()[indexA];
        BOOST_CHECK_EQUAL(areaReserves.areaCapacityReservations.size(), 2);

        RESERVE_PARTICIPATION_THERMAL partA1;
        partA1.participationCost = 5.0;
        partA1.maxPower = 10.0;
        partA1.maxPowerOff = 0.0;
        partA1.participationCostOff = 0.0;
        partA1.clusterName = "ThermalA1";
        partA1.clusterIdInArea = 0;
        partA1.clusterId = 0;
        partA1.globalIndexClusterParticipation = 0;
        partA1.areaIndexClusterParticipation = 0;
        areaReserves.areaCapacityReservations[1]
          .AllThermalReservesParticipation.emplace(0, partA1);
    }

    // Area B: ThermalB1 participates in ReserveUpB
    {
        auto& areaReserves = problemeHebdo->allReserves.value()[indexB];
        BOOST_CHECK_EQUAL(areaReserves.areaCapacityReservations.size(), 1);

        RESERVE_PARTICIPATION_THERMAL partB1;
        partB1.participationCost = 7.0;
        partB1.maxPower = 15.0;
        partB1.maxPowerOff = 0.0;
        partB1.participationCostOff = 0.0;
        partB1.clusterName = "ThermalB1";
        partB1.clusterIdInArea = 0;
        partB1.clusterId = 0;
        partB1.globalIndexClusterParticipation = 1;
        partB1.areaIndexClusterParticipation = 0;
        areaReserves.areaCapacityReservations[0]
          .AllThermalReservesParticipation.emplace(0, partB1);
    }

    // Initialize runtime infos from study
    StudyRuntimeInfos runtimeInfos;

    // Initialize reserve index maps
    runtimeInfos.initializeReservesIndexMaps(*study, *problemeHebdo);

    // Verify that both areas have entries
    BOOST_CHECK(runtimeInfos.reserveParticipationIndexMaps.has_value());
    BOOST_CHECK(runtimeInfos.reserveParticipationIndexMaps.value().contains("a"));
    BOOST_CHECK(runtimeInfos.reserveParticipationIndexMaps.value().contains("b"));

    // Area A should have ReserveUpA thermal participation
    auto& aMaps = runtimeInfos.reserveParticipationIndexMaps.value().at("a");
    BOOST_CHECK_EQUAL(aMaps.thermalClusters.size(), 1);

    // Area B should have ReserveUpB thermal participation
    auto& bMaps = runtimeInfos.reserveParticipationIndexMaps.value().at("b");
    BOOST_CHECK_EQUAL(bMaps.thermalClusters.size(), 1);

    // Verify the keys are correct
    BOOST_CHECK(aMaps.thermalClusters.find({"reserveupa", "ThermalA1"}) != aMaps.thermalClusters.end());
    BOOST_CHECK(bMaps.thermalClusters.find({"reserveupb", "ThermalB1"}) != bMaps.thermalClusters.end());
}

// ============================================================================
// Test: reserveIDToName mapping correctness
// ============================================================================
BOOST_FIXTURE_TEST_CASE(initializeReservesIndexMaps_reserveID_to_name,
                        ReservesIndexMapsTwoAreasFixture)
{
    // Import capacity reservations into PROBLEME_HEBDO
    importCapacityReservations(study->areas, *problemeHebdo);

    // Initialize runtime infos from study
    StudyRuntimeInfos runtimeInfos;

    // Initialize reserve index maps
    runtimeInfos.initializeReservesIndexMaps(*study, *problemeHebdo);

    // Verify reserve ID to name mapping
    BOOST_CHECK_EQUAL(runtimeInfos.reserveIDToName.value().size(), 3);
    BOOST_CHECK_EQUAL(runtimeInfos.reserveIDToName.value().at("reserveupa"), "ReserveUpA");
    BOOST_CHECK_EQUAL(runtimeInfos.reserveIDToName.value().at("reservedowna"), "ReserveDownA");
    BOOST_CHECK_EQUAL(runtimeInfos.reserveIDToName.value().at("reserveupb"), "ReserveUpB");
}

// ============================================================================
// Test: initializeReservesIndexMaps with empty area list (edge case)
// ============================================================================
BOOST_AUTO_TEST_CASE(initializeReservesIndexMaps_empty_study)
{
    // Create a study with no areas
    Study study;
    study.parameters.include.reserves = true;
    study.parameters.simulationDays.first = 0;
    study.parameters.simulationDays.end = 7;

    PROBLEME_HEBDO problemeHebdo;
    problemeHebdo.allReserves = std::vector<::AREA_RESERVES_VECTOR>();

    StudyRuntimeInfos runtimeInfos;
    runtimeInfos.initializeReservesIndexMaps(study, problemeHebdo);

    // Maps should be initialized but empty
    BOOST_CHECK(runtimeInfos.reserveParticipationIndexMaps.has_value());
    BOOST_CHECK(runtimeInfos.reserveIDToName.has_value());
    BOOST_CHECK_EQUAL(runtimeInfos.reserveParticipationIndexMaps.value().size(), 0);
    BOOST_CHECK_EQUAL(runtimeInfos.reserveIDToName.value().size(), 0);
}

// ============================================================================
// Test: initializeReservesIndexMaps when reserves are disabled
// ============================================================================
BOOST_FIXTURE_TEST_CASE(initializeReservesIndexMaps_reserves_disabled,
                        ReservesIndexMapsFixture)
{
    // Disable reserves
    study->parameters.include.reserves = false;

    // Import capacity reservations into PROBLEME_HEBDO
    importCapacityReservations(study->areas, *problemeHebdo);

    // Initialize runtime infos from study
    StudyRuntimeInfos runtimeInfos;

    // Initialize reserve index maps - should still work but produce empty maps
    runtimeInfos.initializeReservesIndexMaps(*study, *problemeHebdo);

    // Maps should be initialized but empty
    BOOST_CHECK(runtimeInfos.reserveParticipationIndexMaps.has_value());
    BOOST_CHECK(runtimeInfos.reserveIDToName.has_value());
    BOOST_CHECK_EQUAL(runtimeInfos.reserveParticipationIndexMaps.value().size(), 1);
}

BOOST_AUTO_TEST_SUITE_END()
