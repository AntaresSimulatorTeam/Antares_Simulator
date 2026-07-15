// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE study
#include <algorithm>
#include <files-system.h>
#include <filesystem>
#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <antares/logs/logs.h>
#include <antares/solver/simulation/sim_structure_probleme_economique.h>
#include <antares/solver/simulation/simulation.h>
#include "antares/study/area/forTestsOnlyList.h"
#include "antares/study/study.h"

using Antares::UnitTests::CaptureAntaresLogs;
using namespace Antares::Data;

void addThermalCluster(Area* area, const std::string& name)
{
    auto c = std::make_shared<ThermalCluster>(area);
    c->setName(name);
    area->thermal.list.addToCompleteList(c);
}

void addShortTermStorage(Area* area, const std::string& name)
{
    ShortTermStorage::STStorageCluster cluster;
    cluster.properties.name = name;
    cluster.id = name;
    area->shortTermStorage.storagesByIndex.push_back(cluster);
}

/*!
 * Study with one area named "A"
 */
class OneProblemWithoutReservesOneArea
{
public:
    OneProblemWithoutReservesOneArea()
    {
        study = std::make_unique<Study>();
        areaA = addAreaToListOfAreas(study->areas, "A");
    }

    std::unique_ptr<Study> study;
    Area* areaA;
};

class OneProblemWithoutReservesOneAreaWithLogger: public OneProblemWithoutReservesOneArea,
                                                  public CaptureAntaresLogs
{
};

/*!
 * Study with one area named "A" and reserves
 */
class OneProblemWithReservesOneArea
{
public:
    OneProblemWithReservesOneArea()
    {
        study = std::make_unique<Study>();
        areaA = addAreaToListOfAreas(study->areas, "A");
        CAPACITY_RESERVATION areaCapacityReservations;
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;
        tmpCapacityReservationUp.type = ReserveType::UP;
        tmpCapacityReservationUp.unsuppliedCost = 1;
        tmpCapacityReservationUp.referenceActivationDuration = 2;
        tmpCapacityReservationUp.powerActivationRatio = 3;
        tmpCapacityReservationUp.energyActivationRatio = 4;

        tmpCapacityReservationUpTwo.type = ReserveType::UP;
        tmpCapacityReservationUpTwo.unsuppliedCost = 11;
        tmpCapacityReservationUpTwo.referenceActivationDuration = 12;
        tmpCapacityReservationUpTwo.powerActivationRatio = 13;
        tmpCapacityReservationUpTwo.energyActivationRatio = 14;

        tmpCapacityReservationUpThree.type = ReserveType::UP;
        tmpCapacityReservationUpThree.unsuppliedCost = 21;
        tmpCapacityReservationUpThree.referenceActivationDuration = 22;
        tmpCapacityReservationUpThree.powerActivationRatio = 23;
        tmpCapacityReservationUpThree.energyActivationRatio = 24;

        tmpCapacityReservationDown.type = ReserveType::DOWN;
        tmpCapacityReservationDown.unsuppliedCost = 5;
        tmpCapacityReservationDown.referenceActivationDuration = 6;
        tmpCapacityReservationDown.powerActivationRatio = 7;
        tmpCapacityReservationDown.energyActivationRatio = 8;

        tmpCapacityReservationDownTwo.type = ReserveType::DOWN;
        tmpCapacityReservationDownTwo.unsuppliedCost = 15;
        tmpCapacityReservationDownTwo.referenceActivationDuration = 16;
        tmpCapacityReservationDownTwo.powerActivationRatio = 17;
        tmpCapacityReservationDownTwo.energyActivationRatio = 18;

        areaA->allCapacityReservations = AllCapacityReservations();
        areaA->allCapacityReservations->areaCapacityReservations.emplace("reserveup",
                                                                         tmpCapacityReservationUp);
        areaA->allCapacityReservations->areaCapacityReservations
          .emplace("reserveuptwo", tmpCapacityReservationUpTwo);
        areaA->allCapacityReservations->areaCapacityReservations
          .emplace("reserveupthree", tmpCapacityReservationUpThree);

        areaA->allCapacityReservations->areaCapacityReservations
          .emplace("reservedown", tmpCapacityReservationDown);
        areaA->allCapacityReservations->areaCapacityReservations
          .emplace("reservedowntwo", tmpCapacityReservationDownTwo);
    }

    std::unique_ptr<Study> study;
    Area* areaA;
    CapacityReservation tmpCapacityReservationUp;
    CapacityReservation tmpCapacityReservationUpTwo;
    CapacityReservation tmpCapacityReservationUpThree;
    CapacityReservation tmpCapacityReservationDown;
    CapacityReservation tmpCapacityReservationDownTwo;
};

class OneProblemWithReservesOneAreaWithLogger: public OneProblemWithReservesOneArea,
                                               public CaptureAntaresLogs
{
};

/*!
 * Study with two areas named "A" and "B"
 */
struct OneProblemWithReservesTwoAreas
{
    OneProblemWithReservesTwoAreas()
    {
        problemeHebdo = std::make_unique<PROBLEME_HEBDO>();
        study = std::make_unique<Study>();
        areaA = addAreaToListOfAreas(study->areas, "A");
        areaB = addAreaToListOfAreas(study->areas, "B");
        CAPACITY_RESERVATION areaCapacityReservations;
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;
        study->parameters.include.reserves = true;

        tmpCapacityReservationUp.type = ReserveType::UP;
        tmpCapacityReservationUp.unsuppliedCost = 1;
        tmpCapacityReservationUp.referenceActivationDuration = 2;
        tmpCapacityReservationUp.powerActivationRatio = 3;
        tmpCapacityReservationUp.energyActivationRatio = 4;

        tmpCapacityReservationDown.type = ReserveType::DOWN;
        tmpCapacityReservationDown.unsuppliedCost = 5;
        tmpCapacityReservationDown.referenceActivationDuration = 6;
        tmpCapacityReservationDown.powerActivationRatio = 7;
        tmpCapacityReservationDown.energyActivationRatio = 8;

        tmpCapacityReservationUpB.type = ReserveType::UP;
        tmpCapacityReservationUpB.unsuppliedCost = 11;
        tmpCapacityReservationUpB.referenceActivationDuration = 12;
        tmpCapacityReservationUpB.powerActivationRatio = 13;
        tmpCapacityReservationUpB.energyActivationRatio = 14;

        tmpCapacityReservationDownB.type = ReserveType::DOWN;
        tmpCapacityReservationDownB.unsuppliedCost = 15;
        tmpCapacityReservationDownB.referenceActivationDuration = 16;
        tmpCapacityReservationDownB.powerActivationRatio = 17;
        tmpCapacityReservationDownB.energyActivationRatio = 18;

        areaA->allCapacityReservations = AllCapacityReservations();
        areaA->allCapacityReservations->areaCapacityReservations.emplace("reserveup",
                                                                         tmpCapacityReservationUp);
        areaA->allCapacityReservations->areaCapacityReservations
          .emplace("reservedown", tmpCapacityReservationDown);

        areaB->allCapacityReservations = AllCapacityReservations();
        areaB->allCapacityReservations->areaCapacityReservations.emplace("reserveup",
                                                                         tmpCapacityReservationUpB);
        areaB->allCapacityReservations->areaCapacityReservations
          .emplace("reservedown", tmpCapacityReservationDownB);
    }

    std::unique_ptr<PROBLEME_HEBDO> problemeHebdo;
    std::unique_ptr<Study> study;
    Area* areaA;
    Area* areaB;
    CapacityReservation tmpCapacityReservationUp;
    CapacityReservation tmpCapacityReservationDown;

    CapacityReservation tmpCapacityReservationUpB;
    CapacityReservation tmpCapacityReservationDownB;
};

BOOST_AUTO_TEST_SUITE(reserves_operations_load)

BOOST_AUTO_TEST_CASE(reserve_add)
{
    auto study = std::make_unique<Study>();
    const auto areaA = addAreaToListOfAreas(study->areas, "A");
    CapacityReservation tmpCapacityReservationUp;
    tmpCapacityReservationUp.type = ReserveType::UP;
    tmpCapacityReservationUp.unsuppliedCost = 0;
    areaA->allCapacityReservations = AllCapacityReservations();
    areaA->allCapacityReservations->areaCapacityReservations.emplace("ReserveUp",
                                                                     tmpCapacityReservationUp);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->size(), 1);
    BOOST_CHECK(areaA->allCapacityReservations->getReserveByID("ReserveUp") != nullptr);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->getReserveByID("ReserveNULL"), nullptr);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->contains("ReserveUp"), true);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->areaCapacityReservations.at("ReserveUp").unsuppliedCost,
      0);
}

BOOST_AUTO_TEST_CASE(reserve_add_double)
{
    auto study = std::make_unique<Study>();
    const auto areaA = addAreaToListOfAreas(study->areas, "A");
    CapacityReservation tmpCapacityReservation;

    CapacityReservation tmpCapacityReservationTwo;
    areaA->allCapacityReservations = AllCapacityReservations();
    areaA->allCapacityReservations->areaCapacityReservations.emplace("Reserve",
                                                                     tmpCapacityReservation);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->size(), 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->contains("Reserve"), true);
    areaA->allCapacityReservations->areaCapacityReservations.emplace("Reserve",
                                                                     tmpCapacityReservationTwo);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->size(), 1);
    areaA->allCapacityReservations->areaCapacityReservations.emplace("ReserveTwo",
                                                                     tmpCapacityReservationTwo);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->size(), 2);
}

BOOST_FIXTURE_TEST_CASE(reserve_one_area, OneProblemWithReservesOneArea)
{
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->size(), 5);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->areaCapacityReservations.at("reserveup").unsuppliedCost,
      1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->areaCapacityReservations.at("reserveup")
                        .referenceActivationDuration,
                      2);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->areaCapacityReservations.at("reserveup").powerActivationRatio,
      3);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->areaCapacityReservations.at("reserveup")
                        .energyActivationRatio,
                      4);

    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->areaCapacityReservations.at("reservedown").unsuppliedCost,
      5);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->areaCapacityReservations.at("reservedown")
                        .referenceActivationDuration,
                      6);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->areaCapacityReservations.at("reservedown")
                        .powerActivationRatio,
                      7);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->areaCapacityReservations.at("reservedown")
                        .energyActivationRatio,
                      8);
}

BOOST_FIXTURE_TEST_CASE(reserve_up_two_areas, OneProblemWithReservesTwoAreas)
{
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->size(), 2);
    BOOST_CHECK_EQUAL(areaB->allCapacityReservations->size(), 2);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->areaCapacityReservations.at("reserveup").unsuppliedCost,
      1);

    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->areaCapacityReservations.at("reservedown").unsuppliedCost,
      5);

    BOOST_CHECK_EQUAL(
      areaB->allCapacityReservations->areaCapacityReservations.at("reserveup").unsuppliedCost,
      11);
    BOOST_CHECK_EQUAL(
      areaB->allCapacityReservations->areaCapacityReservations.at("reservedown").unsuppliedCost,
      15);
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_One_No_Symmetries,
                        OneProblemWithReservesOneArea)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");
    addThermalCluster(areaA, "cluster2");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
)";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(
      areaA->thermal.list.findInAll("cluster1")->reserveParticipationContainer.has_value());
    BOOST_CHECK(
      !areaA->thermal.list.findInAll("cluster2")->reserveParticipationContainer.has_value());
    BOOST_CHECK(areaA->thermal.list.findInAll("cluster1")
                  ->reserveParticipationContainer->isParticipatingInReserve("reserveup"));
    BOOST_CHECK(!areaA->thermal.list.findInAll("cluster1")
                   ->reserveParticipationContainer->isParticipatingInReserve("reservedown"));
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer->reserveMaxPower("reserveup"),
                      9.9);
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer->reserveCost("reserveup"),
                      8.8);
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer->reserveMaxPowerOff("reserveup"),
                      7.7);
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer->reserveCostOff("reserveup"),
                      6.6);
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_One_Bad_Parameter,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");
    addThermalCluster(areaA, "cluster2");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        bad: 9.9
)";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK_EQUAL(getErrors().size(), 1);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK(getErrors().contains("invalid thermal reserve property bad"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Symmetries,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");
    addThermalCluster(areaA, "cluster2");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
      - reserve: ReserveUpTwo
        max-power: 19.9
        participation-cost: 18.8
        max-power-off: 17.7
        participation-cost-off: 16.6
      - reserve: ReserveUpThree
        max-power: 19.9
        participation-cost: 18.8
        max-power-off: 17.7
        participation-cost-off: 16.6
      - reserve: ReserveDown
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
      - reserve: ReserveDownTwo
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
      - reserves: [ReserveUpThree, ReserveDown]
      - reserves: [ReserveUpTwo, ReserveDownTwo]
)";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(
      areaA->thermal.list.findInAll("cluster1")->reserveParticipationContainer.has_value());
    BOOST_CHECK(
      !areaA->thermal.list.findInAll("cluster2")->reserveParticipationContainer.has_value());
    BOOST_CHECK(areaA->thermal.list.findInAll("cluster1")
                  ->reserveParticipationContainer->isParticipatingInReserve("reserveup"));
    BOOST_CHECK(areaA->thermal.list.findInAll("cluster1")
                  ->reserveParticipationContainer->isParticipatingInReserve("reservedown"));

    BOOST_CHECK_EQUAL(
      areaA->thermal.list.findInAll("cluster1")->reserveParticipationContainer->getNbSymGroups(),
      3);

    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer->symmetricalIndices("reserveup")
                        .size(),
                      1);

    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer->symmetricalIndices("reservedown")
                        .size(),
                      2);

    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer->symmetricalIndices("reserveupthree")
                        .size(),
                      1);

    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer->symmetricalIndices("reserveuptwo")
                        .size(),
                      1);

    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer->symmetricalIndices("reservedowntwo")
                        .size(),
                      1);

    std::vector<int> symDown = areaA->thermal.list.findInAll("cluster1")
                                 ->reserveParticipationContainer->symmetricalIndices("reservedown");

    int symUp = areaA->thermal.list.findInAll("cluster1")
                  ->reserveParticipationContainer->symmetricalIndices("reserveup")
                  .at(0);
    int symUpTwo = areaA->thermal.list.findInAll("cluster1")
                     ->reserveParticipationContainer->symmetricalIndices("reserveuptwo")
                     .at(0);
    int symUpThree = areaA->thermal.list.findInAll("cluster1")
                       ->reserveParticipationContainer->symmetricalIndices("reserveupthree")
                       .at(0);
    BOOST_CHECK_EQUAL(
      count(symDown.begin(), symDown.end(), symUp),
      1); // Value in symmetricalIndices("ReserveUp") is also in symmetricalIndices("ReserveDown")
    BOOST_CHECK_EQUAL(
      count(symDown.begin(), symDown.end(), symUpTwo),
      0); // Value in symmetricalIndices("ReserveUpTwo") is not in symmetricalIndices("ReserveDown")
    BOOST_CHECK_EQUAL(count(symDown.begin(), symDown.end(), symUpThree),
                      1);             // Value in symmetricalIndices("ReserveUpThree") is also in
                                      // symmetricalIndices("ReserveDown")
    BOOST_CHECK(symUp != symUpThree); // Value in symmetricalIndices("ReserveUp") is not in
                                      // symmetricalIndices("ReserveUpThree")

    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Bad_Cluster_Symmetry,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
      - reserve: ReserveDown
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
  - cluster: cluster3
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
)";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("Thermal cluster cluster3 does not exist in area A"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Bad_Reserve_Symmetry,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
      - reserve: ReserveDown
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
    symmetries:
      - reserves: [ReserveNull, ReserveDown]
)";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("This entity is not participating to reserve reservenull"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Bad_Cluster_Participation,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");
    addThermalCluster(areaA, "cluster3");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
  - cluster: cluster3
    certifications:
      - reserve: ReserveDown
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
)";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("This entity is not participating to reserve reservedown"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Bad_Reserve_Load,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
      - reserve: ReserveNull
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
)";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains(
      "A : missing reserve reservenull when loading thermal reserve participations"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Delete_Double_Sym_Participation,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
      - reserve: ReserveDown
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
      - reserves: [ReserveUp, ReserveDown]
)";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("Detected duplicate in reserves symmetries"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_No_Cluster_Provided,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
  - cluster: ""
    certifications:
      - reserve: ReserveDown
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
)";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(
      getErrors().contains("A : cluster/storage name is missing in a participation entry"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Double_Cluster_Participation,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
      - reserve: ReserveDown
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
)";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(
      getErrors().contains("A, cluster cluster1 : duplicate participation to reserve reserveup"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Only_One_Symmetry,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
      - reserve: ReserveDown
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
    symmetries:
      - reserves: [ReserveUp]
)";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("A : symmetry group must have at least two reserves"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Triple_Symmetry,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
      - reserve: ReserveDownTwo
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
      - reserve: ReserveDown
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
    symmetries:
      - reserves: [ReserveUp, ReserveDown, ReserveDownTwo]
)";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer->symmetricalIndices("reserveup")
                        .size(),
                      1);
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer->symmetricalIndices("reservedown")
                        .size(),
                      1);
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer->symmetricalIndices("reservedowntwo")
                        .size(),
                      1);
    int symUp = areaA->thermal.list.findInAll("cluster1")
                  ->reserveParticipationContainer->symmetricalIndices("reserveup")
                  .at(0);
    int symDown = areaA->thermal.list.findInAll("cluster1")
                    ->reserveParticipationContainer->symmetricalIndices("reservedown")
                    .at(0);
    int symDownTwo = areaA->thermal.list.findInAll("cluster1")
                       ->reserveParticipationContainer->symmetricalIndices("reservedowntwo")
                       .at(0);
    BOOST_CHECK_EQUAL(symUp, symDown);
    BOOST_CHECK_EQUAL(symUp, symDownTwo); // all of them are participating to the same symmetry
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Double_Symmetry_Same_Line,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - cluster: cluster1
    certifications:
      - reserve: ReserveUp
        max-power: 9.9
        participation-cost: 8.8
        max-power-off: 7.7
        participation-cost-off: 6.6
      - reserve: ReserveDown
        max-power: 1.1
        participation-cost: 2.2
        max-power-off: 3.3
        participation-cost-off: 4.4
    symmetries:
      - reserves: [ReserveUp, ReserveUp]
      - reserves: [ReserveUp, ReserveDown]
)";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("A : symmetry group must have at least two reserves"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Cluster_Participation_No_Init,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - cluster: cluster1
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
)";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains(
      "Area A, cluster1 : trying to add symmetries without any reserve participation"));
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_Symmetries,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  certifications:
    - reserve: ReserveUp
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
    - reserve: ReserveUpTwo
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
    - reserve: ReserveUpThree
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
    - reserve: ReserveDown
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
    - reserve: ReserveDownTwo
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
  symmetries:
    - reserves: [ReserveUp, ReserveDown]
    - reserves: [ReserveUpThree, ReserveDown]
    - reserves: [ReserveUpTwo, ReserveDownTwo]
)";
    file.close();
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");

    BOOST_CHECK(areaA->hydro.reserveParticipationContainer.has_value());

    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer->isParticipatingInReserve(
                        "reserveup"),
                      true);

    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer->isParticipatingInReserve(
                        "reservedown"),
                      true);

    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer->getNbSymGroups(), 3);

    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer->reserveCost("reserveup"), 9.9);
    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer->reserveMaxStore("reserveup"),
                      8.8);
    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer->reserveMaxRelease("reserveup"),
                      7.7);
    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_no_reserve,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  symmetries:
    - reserves: [ReserveUp, ReserveDown]
)";
    file.close();
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains(
      "Area A, hydro : trying to add symmetries without any reserve participation"));
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_missing_reserve,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  certifications:
    - reserve: reservenull
)";
    file.close();
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains(
      "A : missing reserve reservenull when loading hydro reserve participations"));
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_cluster,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  certifications:
    - reserve: ReserveUp
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
    - reserve: ReserveDown
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
  symmetries:
    - reserves: [ReserveUp, ReserveDown]
)";
    file.close();
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK_EQUAL(getErrors().size(), 0);
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_bad_property,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  certifications:
    - reserve: ReserveUp
      bad: 9.9
)";
    file.close();
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK_EQUAL(getErrors().size(), 1);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK(getErrors().contains("invalid hydro reserve property bad"));
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_bad_reserve,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  certifications:
    - reserve: ReserveUp
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
    - reserve: ReserveUpTwo
      cluster: cluster1
      max-power: 19.9
      participation-cost: 18.8
      max-power-off: 17.7
      participation-cost-off: 16.6
  symmetries:
    - reserves: [ReserveUp, ReserveDown]
)";
    file.close();
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("This entity is not participating to reserve reservedown"));
}

BOOST_FIXTURE_TEST_CASE(test_STS_loadReserveParticipations_Symmetries,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    addShortTermStorage(areaA, "cluster1");
    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - storage: cluster1
    certifications:
      - reserve: ReserveUp
        participation-cost: 9.9
        max-store: 8.8
        max-release: 7.7
      - reserve: ReserveUpTwo
        max-power: 19.9
        max-power-off: 17.7
        participation-cost-off: 16.6
      - reserve: ReserveUpThree
      - reserve: ReserveDown
      - reserve: ReserveDownTwo
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
      - reserves: [ReserveUpThree, ReserveDown]
      - reserves: [ReserveUpTwo, ReserveDownTwo]
)";
    file.close();

    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    auto* resContainer = &areaA->shortTermStorage.findInAll("cluster1")
                            ->reserveParticipationContainer;

    BOOST_CHECK(resContainer->has_value());
    BOOST_CHECK_EQUAL(resContainer->value().isParticipatingInReserve("reserveup"), true);
    BOOST_CHECK_EQUAL(resContainer->value().isParticipatingInReserve("reservedown"), true);
    BOOST_CHECK_EQUAL(resContainer->value().getNbSymGroups(), 3);
    BOOST_CHECK_EQUAL(resContainer->value().reserveCost("reserveup"), 9.9);
    BOOST_CHECK_EQUAL(resContainer->value().reserveMaxStore("reserveup"), 8.8);
    BOOST_CHECK_EQUAL(resContainer->value().reserveMaxRelease("reserveup"), 7.7);
    BOOST_CHECK_EQUAL(getErrors().size(), 3);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK(getErrors().contains("invalid STS reserve property max-power"));
    BOOST_CHECK(getErrors().contains("invalid STS reserve property max-power-off"));
    BOOST_CHECK(getErrors().contains("invalid STS reserve property participation-cost-off"));
}

BOOST_FIXTURE_TEST_CASE(test_STS_loadReserveParticipations_bad_cluster_symmetry,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - storage: cluster1
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
)";
    file.close();

    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("Short term storage cluster1 does not exist in area A"));
}

BOOST_FIXTURE_TEST_CASE(test_STS_loadReserveParticipations_no_reserves,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addShortTermStorage(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - storage: cluster1
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
)";
    file.close();
    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains(
      "Area A, cluster1 : trying to add symmetries without any reserve participation"));
}

BOOST_FIXTURE_TEST_CASE(test_STS_loadReserveParticipations_bad_reserve,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addShortTermStorage(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - storage: cluster1
    certifications:
      - reserve: ReserveUp
      - reserve: ReserveDownTwo
    symmetries:
      - reserves: [ReserveUp, ReserveDown]
)";
    file.close();
    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("This entity is not participating to reserve reservedown"));
}

BOOST_FIXTURE_TEST_CASE(test_sts_loadReserveParticipations_No_Cluster_Provided,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - storage: cluster1
    certifications:
      - reserve: ReserveUp
  - certifications:
      - reserve: ReserveDown
)";
    file.close();
    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("Short term storage cluster1 does not exist in area A"));
}

BOOST_FIXTURE_TEST_CASE(test_sts_loadReserveParticipations_Invalid_Cluster,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addShortTermStorage(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.yml");
    file << R"(participations:
  - storage: cluster1
    certifications:
      - reserve: ReserveUp
  - storage: cluster4
    certifications:
      - reserve: ReserveDown
)";
    file.close();
    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.yml");
    BOOST_CHECK(getErrors().contains("Short term storage cluster4 does not exist in area A"));
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_ok_file_missing_needs,
                        OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    std::ofstream file(studyPath / "reserves" / "a" / "reserves.yml");
    file << R"(reserves:
  - name: ReserveUp
)";
    file.close();
    BOOST_CHECK_EXCEPTION(
      accessForTests::loadReservesParameters(studyPath, *areaA),
      std::runtime_error,
      checkMessage("Could not open " + (studyPath / "reserves" / "a" / "reserveup.txt").string()));
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_ok_minimal, OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    std::ofstream file(studyPath / "reserves" / "a" / "reserves.yml");
    file << R"(reserves:
  - name: ReserveUp
)";
    file.close();

    std::ofstream fileNeeds(studyPath / "reserves" / "a" / "reserveup.txt");
    fileNeeds << "\n";
    fileNeeds.close();
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.has_value(), false);
    accessForTests::loadReservesParameters(studyPath, *areaA);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.has_value(), true);
    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->referenceGlobalActivationDuration.up, 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->referenceGlobalActivationDuration.down, 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->maxGlobalEnergyActivationRatio.up, 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->maxGlobalEnergyActivationRatio.down, 1);
    BOOST_CHECK(areaA->allCapacityReservations->getReserveByID("reserveup") != nullptr);
    BOOST_CHECK(areaA->allCapacityReservations->getReserveByID("reserveup")->type
                == ReserveType::DOWN);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->getReserveByID("reserveup")->energyActivationRatio,
      1);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->getReserveByID("reserveup")->powerActivationRatio,
      0);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->getReserveByID("reserveup")->referenceActivationDuration,
      1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->getReserveByID("reserveup")->spillageCost, 0);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->getReserveByID("reserveup")->unsuppliedCost,
                      0);

    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->getReserveByID("reserveup")->need.size(), 0);
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_bad_ini, OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    std::ofstream file(studyPath / "reserves" / "a" / "reserves.yml");
    file << R"(reserves:
  - name: ReserveUp
    a: [

global-parameters:
  a: [
)";
    file.close();

    std::ofstream fileNeeds(studyPath / "reserves" / "a" / "reserveup.txt");
    fileNeeds << "\n";
    fileNeeds.close();
    accessForTests::loadReservesParameters(studyPath, *areaA);

    BOOST_CHECK_EQUAL(getErrors().size(), 1);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK(std::any_of(getErrors().begin(),
                            getErrors().end(),
                            [](const std::string& msg)
                            { return msg.find("Invalid reserves config") != std::string::npos; }));
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_bad_parameters, OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    std::ofstream file(studyPath / "reserves" / "a" / "reserves.yml");
    file << R"(reserves:
  - name: ReserveUp
    a: a

global-parameters:
  b: 2.1
)";
    file.close();

    std::ofstream fileNeeds(studyPath / "reserves" / "a" / "reserveup.txt");
    fileNeeds << "\n";
    fileNeeds.close();
    accessForTests::loadReservesParameters(studyPath, *areaA);

    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 2);
    BOOST_CHECK(
      getWarnings().contains("A : invalid key a inside reserve parameters for ReserveUp"));
    BOOST_CHECK(getWarnings().contains("A : invalid key b inside global reserve parameters"));
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_ok, OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    std::ofstream file(studyPath / "reserves" / "a" / "reserves.yml");
    file << R"(global-parameters:
  energy-activation-ratio-up: 6.6
  energy-activation-ratio-down: 7.7
  reference-activation-duration-up: 8
  reference-activation-duration-down: 9

reserves:
  - name: ReserveUp
    type: up
    reference-activation-duration: 2
    energy-activation-ratio: 2.2
    power-activation-ratio: 3.3
    spillage-cost: 4.4
    failure-cost: 5.5
  - name: ReserveDown
    type: down
)";
    file.close();

    std::ofstream fileNeedsUp(studyPath / "reserves" / "a" / "reserveup.txt");
    fileNeedsUp << "2\n3\n";
    fileNeedsUp.close();

    std::ofstream fileNeedsDown(studyPath / "reserves" / "a" / "reservedown.txt");
    fileNeedsDown << "4\n5\n6\n";
    fileNeedsDown.close();
    accessForTests::loadReservesParameters(studyPath, *areaA);

    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->referenceGlobalActivationDuration.up, 8);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->referenceGlobalActivationDuration.down, 9);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->maxGlobalEnergyActivationRatio.up, 6.6);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->maxGlobalEnergyActivationRatio.down, 7.7);
    BOOST_CHECK(areaA->allCapacityReservations->getReserveByID("reserveup") != nullptr);
    BOOST_CHECK(areaA->allCapacityReservations->getReserveByID("reservedown") != nullptr);
    BOOST_CHECK(areaA->allCapacityReservations->getReserveByID("reserveup")->type
                == ReserveType::UP);
    BOOST_CHECK(areaA->allCapacityReservations->getReserveByID("reservedown")->type
                == ReserveType::DOWN);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->getReserveByID("reserveup")->energyActivationRatio,
      2.2);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->getReserveByID("reserveup")->powerActivationRatio,
      3.3);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations->getReserveByID("reserveup")->referenceActivationDuration,
      2);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->getReserveByID("reserveup")->spillageCost,
                      4.4);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->getReserveByID("reserveup")->unsuppliedCost,
                      5.5);

    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->getReserveByID("reserveup")->need.size(), 2);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->getReserveByID("reservedown")->need.size(),
                      3);

    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->getReserveByID("reserveup")->need.at(0), 2);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->getReserveByID("reserveup")->need.at(1), 3);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations->getReserveByID("reservedown")->need.at(0), 4);
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_negative_parameters_values,
                        OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    std::ofstream file(studyPath / "reserves" / "a" / "reserves.yml");
    file << R"(reserves:
  - name: ReserveUp
    type: up
    reference-activation-duration: -1
    energy-activation-ratio: -2
    power-activation-ratio: -3
    spillage-cost: 10
    failure-cost: 10

global-parameters:
  energy-activation-ratio-up: -1
  energy-activation-ratio-down: -1
  reference-activation-duration-up: -2
  reference-activation-duration-down: -2
)";
    file.close();

    std::ofstream fileNeedsUp(studyPath / "reserves" / "a" / "reserveup.txt");
    fileNeedsUp.close();

    std::ofstream fileNeedsDown(studyPath / "reserves" / "a" / "reservedown.txt");
    fileNeedsDown.close();
    accessForTests::loadReservesParameters(studyPath, *areaA);
    BOOST_CHECK_EQUAL(getErrors().size(), 7);
    BOOST_CHECK(
      getErrors().contains("A : invalid maxGlobalEnergyActivationRatio down can not be negative"));
    BOOST_CHECK(
      getErrors().contains("A : invalid maxGlobalEnergyActivationRatio up can not be negative"));
    BOOST_CHECK(getErrors().contains(
      "A : invalid referenceGlobalActivationDuration down can not be negative"));
    BOOST_CHECK(
      getErrors().contains("A : invalid referenceGlobalActivationDuration up can not be negative"));
    BOOST_CHECK(getErrors().contains(
      "A : invalid energyActivationRatio can not be negative, for reserve reserveup"));
    BOOST_CHECK(getErrors().contains(
      "A : invalid powerActivationRatio can not be negative, for reserve reserveup"));
    BOOST_CHECK(getErrors().contains(
      "A : invalid referenceActivationDuration can not be negative, for reserve reserveup"));
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_bad_parameters_values,
                        OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    std::ofstream file(studyPath / "reserves" / "a" / "reserves.yml");
    file << R"(reserves:
  - name: ReserveUp
    type: aa
    reference-activation-duration: 1.1
    energy-activation-ratio: a
    power-activation-ratio: a
    spillage-cost: a
    failure-cost: a

global-parameters:
  energy-activation-ratio-up: a
  energy-activation-ratio-down: a
  reference-activation-duration-up: 1.1
  reference-activation-duration-down: 1.1
)";
    file.close();

    std::ofstream fileNeedsUp(studyPath / "reserves" / "a" / "reserveup.txt");
    fileNeedsUp.close();

    std::ofstream fileNeedsDown(studyPath / "reserves" / "a" / "reservedown.txt");
    fileNeedsDown.close();
    accessForTests::loadReservesParameters(studyPath, *areaA);
    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 10);
    BOOST_CHECK(getWarnings().contains("A : invalid type for reserve ReserveUp"));
    BOOST_CHECK(
      getWarnings().contains("A : invalid reference activation duration for reserve ReserveUp"));
    BOOST_CHECK(
      getWarnings().contains("A : invalid energy activation ratio for reserve ReserveUp"));
    BOOST_CHECK(
      getWarnings().contains("A : invalid maximum activation ratio for reserve ReserveUp"));
    BOOST_CHECK(getWarnings().contains("A : invalid spillage cost for reserve ReserveUp"));
    BOOST_CHECK(getWarnings().contains("A : invalid failure cost for reserve ReserveUp"));
    BOOST_CHECK(
      getWarnings().contains("A : invalid maximum energy activation ratio for UP reserves"));
    BOOST_CHECK(
      getWarnings().contains("A : invalid maximum energy activation ratio for DOWN reserves"));
    BOOST_CHECK(
      getWarnings().contains("A : invalid reference energy activation duration for UP reserves"));
    BOOST_CHECK(
      getWarnings().contains("A : invalid reference energy activation duration for DOWN reserves"));
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_duplicated, OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    std::ofstream file(studyPath / "reserves" / "a" / "reserves.yml");
    file << R"(reserves:
  - name: ReserveUp
  - name: ReserveUp
global-parameters:
)";
    file.close();

    std::ofstream fileNeedsUp(studyPath / "reserves" / "a" / "reserveup.txt");
    fileNeedsUp.close();
    accessForTests::loadReservesParameters(studyPath, *areaA);
    BOOST_CHECK_EQUAL(getErrors().size(), 1);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK(getErrors().contains("A : reserve name already exists for reserve ReserveUp"));
}

BOOST_FIXTURE_TEST_CASE(test_negative_value, CaptureAntaresLogs)
{
    errorIfNegativeValue("string1", 2, "area", "cluster", "reserve");

    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);

    errorIfNegativeValue("parameter", -2, "area", "cluster", "reserve");

    BOOST_CHECK_EQUAL(getErrors().size(), 1);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK(getErrors().contains(
      "area : invalid parameter can not be negative, for cluster cluster, for reserve reserve"));
}

BOOST_FIXTURE_TEST_CASE(test_validateCapacityReservations_noNegative,
                        OneProblemWithReservesOneAreaWithLogger)
{
    accessForTests::validateCapacityReservations(*areaA);

    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
}

BOOST_FIXTURE_TEST_CASE(test_validateCapacityReservations_OneNegative,
                        OneProblemWithReservesOneAreaWithLogger)
{
    areaA->allCapacityReservations->areaCapacityReservations.at("reserveup").powerActivationRatio
      = -1;
    accessForTests::validateCapacityReservations(*areaA);

    BOOST_CHECK_EQUAL(getErrors().size(), 1);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK(getErrors().contains(
      "A : invalid powerActivationRatio can not be negative, for reserve reserveup"));
}

BOOST_AUTO_TEST_SUITE_END() // version
