/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#define BOOST_TEST_MODULE study
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

void addThermalCluster(Data::Area* area, const std::string& name)
{
    auto c = std::make_shared<ThermalCluster>(area);
    c->setName(name);
    area->thermal.list.addToCompleteList(c);
}

void addShortTermStorage(Data::Area* area, const std::string& name)
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
        areaA = study->areaAdd("A");
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
        areaA = study->areaAdd("A");
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
        areaA->allCapacityReservations.value()
          .areaCapacityReservations.emplace("ReserveUp", tmpCapacityReservationUp);
        areaA->allCapacityReservations.value()
          .areaCapacityReservations.emplace("ReserveUpTwo", tmpCapacityReservationUpTwo);
        areaA->allCapacityReservations.value()
          .areaCapacityReservations.emplace("ReserveUpThree", tmpCapacityReservationUpThree);

        areaA->allCapacityReservations.value()
          .areaCapacityReservations.emplace("ReserveDown", tmpCapacityReservationDown);
        areaA->allCapacityReservations.value()
          .areaCapacityReservations.emplace("ReserveDownTwo", tmpCapacityReservationDownTwo);
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
        areaA = study->areaAdd("A");
        areaB = study->areaAdd("B");
        CAPACITY_RESERVATION areaCapacityReservations;
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;
        study->parameters.reservesEnabled = true;

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
        areaA->allCapacityReservations.value()
          .areaCapacityReservations.emplace("ReserveUp", tmpCapacityReservationUp);
        areaA->allCapacityReservations.value()
          .areaCapacityReservations.emplace("ReserveDown", tmpCapacityReservationDown);

        areaB->allCapacityReservations = AllCapacityReservations();
        areaB->allCapacityReservations.value()
          .areaCapacityReservations.emplace("ReserveUp", tmpCapacityReservationUpB);
        areaB->allCapacityReservations.value()
          .areaCapacityReservations.emplace("ReserveDown", tmpCapacityReservationDownB);
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
    const auto areaA = study->areaAdd("A");
    CapacityReservation tmpCapacityReservationUp;
    tmpCapacityReservationUp.type = ReserveType::UP;
    tmpCapacityReservationUp.unsuppliedCost = 0;
    areaA->allCapacityReservations = AllCapacityReservations();
    areaA->allCapacityReservations.value()
      .areaCapacityReservations.emplace("ReserveUp", tmpCapacityReservationUp);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().size(), 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().contains("ReserveUp"), true);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservations.at("ReserveUp")
                        .unsuppliedCost,
                      0);
}

BOOST_AUTO_TEST_CASE(reserve_add_double)
{
    auto study = std::make_unique<Study>();
    const auto areaA = study->areaAdd("A");
    CapacityReservation tmpCapacityReservation;

    CapacityReservation tmpCapacityReservationTwo;
    areaA->allCapacityReservations = AllCapacityReservations();
    areaA->allCapacityReservations.value().areaCapacityReservations.emplace("Reserve",
                                                                            tmpCapacityReservation);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().size(), 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().contains("Reserve"), true);
    areaA->allCapacityReservations.value()
      .areaCapacityReservations.emplace("Reserve", tmpCapacityReservationTwo);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().size(), 1);
    areaA->allCapacityReservations.value()
      .areaCapacityReservations.emplace("ReserveTwo", tmpCapacityReservationTwo);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().size(), 2);
}

BOOST_FIXTURE_TEST_CASE(reserve_one_area, OneProblemWithReservesOneArea)
{
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().size(), 5);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservations.at("ReserveUp")
                        .unsuppliedCost,
                      1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservations.at("ReserveUp")
                        .referenceActivationDuration,
                      2);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservations.at("ReserveUp")
                        .powerActivationRatio,
                      3);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservations.at("ReserveUp")
                        .energyActivationRatio,
                      4);

    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservations.at("ReserveDown")
                        .unsuppliedCost,
                      5);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservations.at("ReserveDown")
                        .referenceActivationDuration,
                      6);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservations.at("ReserveDown")
                        .powerActivationRatio,
                      7);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservations.at("ReserveDown")
                        .energyActivationRatio,
                      8);
}

BOOST_FIXTURE_TEST_CASE(reserve_up_two_areas, OneProblemWithReservesTwoAreas)
{
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().size(), 2);
    BOOST_CHECK_EQUAL(areaB->allCapacityReservations.value().size(), 2);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservations.at("ReserveUp")
                        .unsuppliedCost,
                      1);

    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservations.at("ReserveDown")
                        .unsuppliedCost,
                      5);

    BOOST_CHECK_EQUAL(areaB->allCapacityReservations.value()
                        .areaCapacityReservations.at("ReserveUp")
                        .unsuppliedCost,
                      11);
    BOOST_CHECK_EQUAL(areaB->allCapacityReservations.value()
                        .areaCapacityReservations.at("ReserveDown")
                        .unsuppliedCost,
                      15);
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_One_No_Symmetries,
                        OneProblemWithReservesOneArea)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");
    addThermalCluster(areaA, "cluster2");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK(
      areaA->thermal.list.findInAll("cluster1")->reserveParticipationContainer.has_value());
    BOOST_CHECK(
      !areaA->thermal.list.findInAll("cluster2")->reserveParticipationContainer.has_value());
    BOOST_CHECK(areaA->thermal.list.findInAll("cluster1")
                  ->reserveParticipationContainer.value()
                  .isParticipatingInReserve("ReserveUp"));
    BOOST_CHECK(!areaA->thermal.list.findInAll("cluster1")
                   ->reserveParticipationContainer.value()
                   .isParticipatingInReserve("ReserveDown"));
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .reserveMaxPower("ReserveUp"),
                      9.9);
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .reserveCost("ReserveUp"),
                      8.8);
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .reserveMaxPowerOff("ReserveUp"),
                      7.7);
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .reserveCostOff("ReserveUp"),
                      6.6);
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_One_Bad_Parameter,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");
    addThermalCluster(areaA, "cluster2");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "bad = 9.9\n";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 1);
    BOOST_CHECK(
      getWarnings().contains("A : invalid property in thermal reserves implementation : bad"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Symmetries,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");
    addThermalCluster(areaA, "cluster2");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "cluster1 = [ReserveUpThree, ReserveDown]\n";
    file << "cluster1 = [ReserveUpTwo, ReserveDownTwo]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file << "\n";
    file << "[ReserveUpTwo]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 19.9\n";
    file << "participation-cost = 18.8\n";
    file << "max-power-off = 17.7\n";
    file << "participation-cost-off = 16.6\n";
    file << "\n";
    file << "[ReserveUpThree]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 19.9\n";
    file << "participation-cost = 18.8\n";
    file << "max-power-off = 17.7\n";
    file << "participation-cost-off = 16.6\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file << "\n";
    file << "[ReserveDownTwo]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK(
      areaA->thermal.list.findInAll("cluster1")->reserveParticipationContainer.has_value());
    BOOST_CHECK(
      !areaA->thermal.list.findInAll("cluster2")->reserveParticipationContainer.has_value());
    BOOST_CHECK(areaA->thermal.list.findInAll("cluster1")
                  ->reserveParticipationContainer.value()
                  .isParticipatingInReserve("ReserveUp"));
    BOOST_CHECK(areaA->thermal.list.findInAll("cluster1")
                  ->reserveParticipationContainer.value()
                  .isParticipatingInReserve("ReserveDown"));

    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .getNbSymGroups(),
                      3);

    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .symmetricalIndices("ReserveUp")
                        .size(),
                      1);

    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .symmetricalIndices("ReserveDown")
                        .size(),
                      2);

    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .symmetricalIndices("ReserveUpThree")
                        .size(),
                      1);

    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .symmetricalIndices("ReserveUpTwo")
                        .size(),
                      1);

    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .symmetricalIndices("ReserveDownTwo")
                        .size(),
                      1);

    std::vector<int> symDown = areaA->thermal.list.findInAll("cluster1")
                                 ->reserveParticipationContainer.value()
                                 .symmetricalIndices("ReserveDown");

    int symUp = areaA->thermal.list.findInAll("cluster1")
                  ->reserveParticipationContainer.value()
                  .symmetricalIndices("ReserveUp")
                  .at(0);
    int symUpTwo = areaA->thermal.list.findInAll("cluster1")
                     ->reserveParticipationContainer.value()
                     .symmetricalIndices("ReserveUpTwo")
                     .at(0);
    int symUpThree = areaA->thermal.list.findInAll("cluster1")
                       ->reserveParticipationContainer.value()
                       .symmetricalIndices("ReserveUpThree")
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

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster3 = [ReserveUp, ReserveDown]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK(
      getErrors().contains("Thermal cluster cluster3 not participating to reserves of A"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Bad_Reserve_Symmetry,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveNull, ReserveDown]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();
    BOOST_CHECK_EXCEPTION(
      areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini"),
      std::out_of_range,
      checkMessage("This entity is not participating to reserve ReserveNull"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Bad_Cluster_Participation,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");
    addThermalCluster(areaA, "cluster3");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "cluster-name = cluster3\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();
    BOOST_CHECK_EXCEPTION(
      areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini"),
      std::out_of_range,
      checkMessage("This entity is not participating to reserve ReserveDown"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Bad_Reserve_Load,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file << "\n";
    file << "[ReserveNull]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK(getErrors().contains(
      "A: missing reserve ReserveNull when loading thermal reserve participations"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Delete_Double_Sym_Participation,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();
    BOOST_CHECK_EXCEPTION(
      areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini"),
      std::invalid_argument,
      checkMessage("Detected duplicate in reserves symmetries"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_No_Cluster_Provided,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK(getErrors().contains(
      "A : Please provide a cluster name when declaring a capacity reservation"));
    BOOST_CHECK(
      getErrors().contains("A : missing cluster  when loading thermal reserve participations"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Double_Cluster_Participation,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK(
      getErrors().contains("A, cluster cluster1 : duplicate participation to reserve ReserveUp"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Only_One_Symmetry,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();
    BOOST_CHECK_EXCEPTION(
      areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini"),
      std::runtime_error,
      checkMessage("Must have at least two distinct reserves to participate to a symmetry"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Triple_Symmetry,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown, ReserveDownTwo]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file << "\n";
    file << "[ReserveDownTwo]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .symmetricalIndices("ReserveUp")
                        .size(),
                      1);
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .symmetricalIndices("ReserveDown")
                        .size(),
                      1);
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .symmetricalIndices("ReserveDownTwo")
                        .size(),
                      1);
    int symUp = areaA->thermal.list.findInAll("cluster1")
                  ->reserveParticipationContainer.value()
                  .symmetricalIndices("ReserveUp")
                  .at(0);
    int symDown = areaA->thermal.list.findInAll("cluster1")
                    ->reserveParticipationContainer.value()
                    .symmetricalIndices("ReserveDown")
                    .at(0);
    int symDownTwo = areaA->thermal.list.findInAll("cluster1")
                       ->reserveParticipationContainer.value()
                       .symmetricalIndices("ReserveDownTwo")
                       .at(0);
    BOOST_CHECK_EQUAL(symUp, symDown);
    BOOST_CHECK_EQUAL(symUp, symDownTwo); // all of them are participating to the same symmetry
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Double_Symmetry_Same_Line,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveUp]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();
    BOOST_CHECK_EXCEPTION(
      areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini"),
      std::runtime_error,
      checkMessage("Must have at least two distinct reserves to participate to a symmetry"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Cluster_Participation_No_Init,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file.close();
    BOOST_CHECK_EXCEPTION(
      areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini"),
      std::runtime_error,
      checkMessage(
        "Area A, cluster1 : trying to add symmetries without any reserves participations"));
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_Symmetries,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "hydro = [ReserveUp, ReserveDown]\n";
    file << "lt = [ReserveUpThree, ReserveDown]\n";
    file << "hydro = [ReserveUpTwo, ReserveDownTwo]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "participation-cost = 9.9\n";
    file << "max-store = 8.8\n";
    file << "max-release = 7.7\n";
    file << "\n";
    file << "[ReserveUpTwo]\n";
    file << "participation-cost = 9.9\n";
    file << "max-store = 8.8\n";
    file << "max-release = 7.7\n";
    file << "\n";
    file << "[ReserveUpThree]\n";
    file << "participation-cost = 9.9\n";
    file << "max-store = 8.8\n";
    file << "max-release = 7.7\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "participation-cost = 9.9\n";
    file << "max-store = 8.8\n";
    file << "max-release = 7.7\n";
    file << "\n";
    file << "[ReserveDownTwo]\n";
    file << "participation-cost = 9.9\n";
    file << "max-store = 8.8\n";
    file << "max-release = 7.7\n";
    file.close();
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");

    BOOST_CHECK(areaA->hydro.reserveParticipationContainer.has_value());

    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer.value().isParticipatingInReserve(
                        "ReserveUp"),
                      true);

    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer.value().isParticipatingInReserve(
                        "ReserveDown"),
                      true);

    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer.value().getNbSymGroups(), 3);

    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer.value().reserveCost("ReserveUp"),
                      9.9);
    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer.value().reserveMaxStore(
                        "ReserveUp"),
                      8.8);
    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer.value().reserveMaxRelease(
                        "ReserveUp"),
                      7.7);
    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_no_reserve,
                        OneProblemWithReservesOneArea)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "hydro = [ReserveUp, ReserveDown]\n";
    file.close();
    BOOST_CHECK_EXCEPTION(
      areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.ini"),
      std::runtime_error,
      checkMessage("Area A, hydro : trying to add symmetries without any reserves participations"));
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_missing_reserve,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[ReserveNull]\n";
    file.close();
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK(getErrors().contains(
      "A: missing reserve ReserveNull when loading hydro reserve participations"));
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_cluster,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "participation-cost = 9.9\n";
    file << "max-store = 8.8\n";
    file << "max-release = 7.7\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "participation-cost = 9.9\n";
    file << "max-store = 8.8\n";
    file << "max-release = 7.7\n";
    file.close();
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK(getErrors().contains(
      "A : invalid cluster name for hydro symmetry cluster1 please use 'hydro' or 'lt'"));
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_bad_property,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[ReserveUp]\n";
    file << "bad = 9.9\n";
    file << "\n";
    file.close();
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 1);
    BOOST_CHECK(
      getWarnings().contains("A : invalid property in hydro reserves implementation : bad"));
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_bad_reserve,
                        OneProblemWithReservesOneArea)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "[ReserveUp]\n";
    file << "participation-cost = 9.9\n";
    file << "max-store = 8.8\n";
    file << "max-release = 7.7\n";
    file << "\n";
    file << "[ReserveUpTwo]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 19.9\n";
    file << "participation-cost = 18.8\n";
    file << "max-power-off = 17.7\n";
    file << "participation-cost-off = 16.6\n";
    file.close();
    BOOST_CHECK_EXCEPTION(areaA->hydro.loadReserveParticipations(*areaA,
                                                                 studyPath / "myreserve.ini"),
                          std::out_of_range,
                          checkMessage("This entity is not participating to reserve ReserveDown"));
}

BOOST_FIXTURE_TEST_CASE(test_STS_loadReserveParticipations_Symmetries,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    addShortTermStorage(areaA, "cluster1");
    std::ofstream file(studyPath / "myreserve.ini");

    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "cluster1 = [ReserveUpThree, ReserveDown]\n";
    file << "cluster1 = [ReserveUpTwo, ReserveDownTwo]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "participation-cost = 9.9\n";
    file << "max-store = 8.8\n";
    file << "max-release = 7.7\n";
    file << "\n";
    file << "[ReserveUpTwo]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 19.9\n";
    file << "max-power-off = 17.7\n";
    file << "participation-cost-off = 16.6\n";
    file << "\n";
    file << "[ReserveUpThree]\n";
    file << "cluster-name = cluster1\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "cluster-name = cluster1\n";
    file << "\n";
    file << "[ReserveDownTwo]\n";
    file << "cluster-name = cluster1\n";
    file.close();

    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    auto* resContainer = &areaA->shortTermStorage.findInAll("cluster1")
                            ->reserveParticipationContainer;

    BOOST_CHECK(resContainer->has_value());
    BOOST_CHECK_EQUAL(resContainer->value().isParticipatingInReserve("ReserveUp"), true);
    BOOST_CHECK_EQUAL(resContainer->value().isParticipatingInReserve("ReserveDown"), true);
    BOOST_CHECK_EQUAL(resContainer->value().getNbSymGroups(), 3);
    BOOST_CHECK_EQUAL(resContainer->value().reserveCost("ReserveUp"), 9.9);
    BOOST_CHECK_EQUAL(resContainer->value().reserveMaxStore("ReserveUp"), 8.8);
    BOOST_CHECK_EQUAL(resContainer->value().reserveMaxRelease("ReserveUp"), 7.7);
    BOOST_CHECK_EQUAL(getWarnings().size(), 3);
    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK(
      getWarnings().contains("A : invalid property in STS reserves implementation : max-power"));
    BOOST_CHECK(getWarnings().contains(
      "A : invalid property in STS reserves implementation : max-power-off"));
    BOOST_CHECK(getWarnings().contains(
      "A : invalid property in STS reserves implementation : participation-cost-off"));
}

BOOST_FIXTURE_TEST_CASE(test_STS_loadReserveParticipations_bad_cluster_symmetry,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "\n";
    file << "[ReserveNull]\n";
    file.close();

    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK(getErrors().contains(
      "Area A, cluster1 : trying to add symmetries to a non existing cluster or participation"));
}

BOOST_FIXTURE_TEST_CASE(test_STS_loadReserveParticipations_no_reserves,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addShortTermStorage(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file.close();
    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK(getErrors().contains(
      "Area A, cluster1 : trying to add symmetries to a non existing cluster or participation"));
}

BOOST_FIXTURE_TEST_CASE(test_STS_loadReserveParticipations_bad_reserve,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addShortTermStorage(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "\n";
    file << "[ReserveDownTwo]\n";
    file << "cluster-name = cluster1\n";
    file.close();
    BOOST_CHECK_EXCEPTION(
      areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.ini"),
      std::out_of_range,
      checkMessage("This entity is not participating to reserve ReserveDown"));
}

BOOST_FIXTURE_TEST_CASE(test_sts_loadReserveParticipations_No_Cluster_Provided,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file.close();
    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK(getErrors().contains(
      "A : Please provide a cluster name when declaring a capacity reservation"));
    BOOST_CHECK(
      getErrors().contains("A : missing cluster  when loading STS reserve participations"));
}

BOOST_FIXTURE_TEST_CASE(test_sts_loadReserveParticipations_Invalid_Cluster,
                        OneProblemWithReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addShortTermStorage(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "cluster-name = cluster4\n";
    file.close();
    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK(
      getErrors().contains("A : missing cluster cluster4 when loading STS reserve participations"));
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_ok_file_missing_needs,
                        OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    std::ofstream file(studyPath / "reserves" / "a" / "reserves.ini");
    file << "[ReserveUp]\n";
    file.close();
    BOOST_CHECK_EXCEPTION(
      accessForTests::loadReservesParameters(studyPath, *areaA),
      std::runtime_error,
      checkMessage("Could not open " + (studyPath / "reserves" / "a" / "ReserveUp.txt").string()));
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_ok_minimal, OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    std::ofstream file(studyPath / "reserves" / "a" / "reserves.ini");
    file << "[ReserveUp]\n";
    file.close();

    std::ofstream fileNeeds(studyPath / "reserves" / "a" / "ReserveUp.txt");
    fileNeeds << "\n";
    fileNeeds.close();
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.has_value(), false);
    accessForTests::loadReservesParameters(studyPath, *areaA);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.has_value(), true);
    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().referenceGlobalActivationDuration.up,
                      1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().referenceGlobalActivationDuration.down,
                      1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().maxGlobalEnergyActivationRatio.up, 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().maxGlobalEnergyActivationRatio.down,
                      1);
    BOOST_CHECK(areaA->allCapacityReservations.value().getReserveByName("ReserveUp") != nullptr);
    BOOST_CHECK(areaA->allCapacityReservations.value().getReserveByName("ReserveUp")->type
                == ReserveType::DOWN);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations.value().getReserveByName("ReserveUp")->energyActivationRatio,
      1);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations.value().getReserveByName("ReserveUp")->powerActivationRatio,
      0);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .getReserveByName("ReserveUp")
                        ->referenceActivationDuration,
                      1);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations.value().getReserveByName("ReserveUp")->spillageCost,
      0);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations.value().getReserveByName("ReserveUp")->unsuppliedCost,
      0);

    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations.value().getReserveByName("ReserveUp")->need.size(),
      0);
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_bad_ini, OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    std::ofstream file(studyPath / "reserves" / "a" / "reserves.ini");
    file << "[ReserveUp]\n";
    file << "a\n ";
    file << "\n ";
    file << "[globalparameters]\n";
    file << "a\n ";
    file.close();

    std::ofstream fileNeeds(studyPath / "reserves" / "a" / "ReserveUp.txt");
    fileNeeds << "\n";
    fileNeeds.close();
    accessForTests::loadReservesParameters(studyPath, *areaA);

    BOOST_CHECK_EQUAL(getErrors().size(), 2);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK(getErrors().contains("INI content : unknown format for line \'a\'"));
    BOOST_CHECK(getErrors().contains("Invalid INI file : "
                                     + (studyPath / "reserves" / "a" / "reserves.ini").string()));
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_bad_parameters, OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    std::ofstream file(studyPath / "reserves" / "a" / "reserves.ini");
    file << "[ReserveUp]\n";
    file << "a = a\n ";
    file << "\n ";
    file << "[globalparameters]\n";
    file << "b = 2.1\n ";
    file.close();

    std::ofstream fileNeeds(studyPath / "reserves" / "a" / "ReserveUp.txt");
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
    std::ofstream file(studyPath / "reserves" / "a" / "reserves.ini");
    file << "[ReserveUp]\n";
    file << "type = up\n ";
    file << "reference-activation-duration = 2\n ";
    file << "energy-activation-ratio = 2.2\n ";
    file << "power-activation-ratio = 3.3\n ";
    file << "spillage-cost = 4.4\n ";
    file << "failure-cost = 5.5\n ";
    file << "\n ";
    file << "[globalparameters]\n";
    file << "energy-activation-ratio-up = 6.6\n ";
    file << "energy-activation-ratio-down = 7.7\n ";
    file << "reference-activation-duration-up = 8\n ";
    file << "reference-activation-duration-down = 9\n ";
    file << "[ReserveDown]\n";
    file << "type = down\n ";
    file.close();

    std::ofstream fileNeedsUp(studyPath / "reserves" / "a" / "ReserveUp.txt");
    fileNeedsUp << "2\n";
    fileNeedsUp << "3\n";
    fileNeedsUp.close();

    std::ofstream fileNeedsDown(studyPath / "reserves" / "a" / "ReserveDown.txt");
    fileNeedsDown << "4\n";
    fileNeedsDown << "5\n";
    fileNeedsDown << "6\n";
    fileNeedsDown.close();
    accessForTests::loadReservesParameters(studyPath, *areaA);

    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().referenceGlobalActivationDuration.up,
                      8);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().referenceGlobalActivationDuration.down,
                      9);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().maxGlobalEnergyActivationRatio.up,
                      6.6);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().maxGlobalEnergyActivationRatio.down,
                      7.7);
    BOOST_CHECK(areaA->allCapacityReservations.value().getReserveByName("ReserveUp") != nullptr);
    BOOST_CHECK(areaA->allCapacityReservations.value().getReserveByName("ReserveDown") != nullptr);
    BOOST_CHECK(areaA->allCapacityReservations.value().getReserveByName("ReserveUp")->type
                == ReserveType::UP);
    BOOST_CHECK(areaA->allCapacityReservations.value().getReserveByName("ReserveDown")->type
                == ReserveType::DOWN);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations.value().getReserveByName("ReserveUp")->energyActivationRatio,
      2.2);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations.value().getReserveByName("ReserveUp")->powerActivationRatio,
      3.3);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .getReserveByName("ReserveUp")
                        ->referenceActivationDuration,
                      2);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations.value().getReserveByName("ReserveUp")->spillageCost,
      4.4);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations.value().getReserveByName("ReserveUp")->unsuppliedCost,
      5.5);

    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations.value().getReserveByName("ReserveUp")->need.size(),
      2);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations.value().getReserveByName("ReserveDown")->need.size(),
      3);

    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations.value().getReserveByName("ReserveUp")->need.at(0),
      2);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations.value().getReserveByName("ReserveUp")->need.at(1),
      3);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations.value().getReserveByName("ReserveDown")->need.at(0),
      4);
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_bad_parameters_values,
                        OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    std::ofstream file(studyPath / "reserves" / "a" / "reserves.ini");
    file << "[ReserveUp]\n";
    file << "type = aa\n ";
    file << "reference-activation-duration = 1.1\n ";
    file << "energy-activation-ratio = a\n ";
    file << "power-activation-ratio = a\n ";
    file << "spillage-cost = a\n ";
    file << "failure-cost = a\n ";
    file << "\n ";
    file << "[globalparameters]\n";
    file << "energy-activation-ratio-up = a\n ";
    file << "energy-activation-ratio-down = a\n ";
    file << "reference-activation-duration-up = 1.1\n ";
    file << "reference-activation-duration-down = 1.1\n ";
    file.close();

    std::ofstream fileNeedsUp(studyPath / "reserves" / "a" / "ReserveUp.txt");
    fileNeedsUp.close();

    std::ofstream fileNeedsDown(studyPath / "reserves" / "a" / "ReserveDown.txt");
    fileNeedsDown.close();
    accessForTests::loadReservesParameters(studyPath, *areaA);
    BOOST_CHECK_EQUAL(getErrors().size(), 0);
    BOOST_CHECK_EQUAL(getWarnings().size(), 10);
    BOOST_CHECK(getWarnings().contains("A: invalid type for reserve ReserveUp"));
    BOOST_CHECK(
      getWarnings().contains("A: invalid reference activation duration for reserve ReserveUp"));
    BOOST_CHECK(getWarnings().contains("A: invalid energy activation ratio for reserve ReserveUp"));
    BOOST_CHECK(
      getWarnings().contains("A: invalid maximum activation ratio for reserve ReserveUp"));
    BOOST_CHECK(getWarnings().contains("A: invalid spillage cost for reserve ReserveUp"));
    BOOST_CHECK(getWarnings().contains("A: invalid failure cost for reserve ReserveUp"));
    BOOST_CHECK(
      getWarnings().contains("A: invalid maximum energy activation ratio for UP reserves"));
    BOOST_CHECK(
      getWarnings().contains("A: invalid maximum energy activation ratio for DOWN reserves"));
    BOOST_CHECK(
      getWarnings().contains("A: invalid reference energy activation duration for UP reserves"));
    BOOST_CHECK(
      getWarnings().contains("A: invalid reference energy activation duration for DOWN reserves"));
}

BOOST_FIXTURE_TEST_CASE(test_readReserve_duplicated, OneProblemWithoutReservesOneAreaWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "reserves" / "a");
    std::ofstream file(studyPath / "reserves" / "a" / "reserves.ini");
    file << "[ReserveUp]\n";
    file << "\n ";
    file << "[ReserveUp]\n";
    file << "\n ";
    file << "[globalparameters]\n";
    file.close();

    std::ofstream fileNeedsUp(studyPath / "reserves" / "a" / "ReserveUp.txt");
    fileNeedsUp.close();
    accessForTests::loadReservesParameters(studyPath, *areaA);
    BOOST_CHECK_EQUAL(getErrors().size(), 1);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK(getErrors().contains("A : reserve name already exists for reserve ReserveUp"));
}

BOOST_AUTO_TEST_SUITE_END() // version
