// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE study
#include <algorithm>
#include <files-system.h>
#include <filesystem>

#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <antares/solver/simulation/sim_structure_probleme_economique.h>
#include <antares/solver/simulation/simulation.h>
#include "antares/study/study.h"

using namespace Antares::Data;

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
        tmpCapacityReservationUp.setName("ReserveUp");

        tmpCapacityReservationDown.type = ReserveType::DOWN;
        tmpCapacityReservationDown.unsuppliedCost = 5;
        tmpCapacityReservationDown.referenceActivationDuration = 6;
        tmpCapacityReservationDown.powerActivationRatio = 7;
        tmpCapacityReservationDown.energyActivationRatio = 8;
        tmpCapacityReservationDown.setName("ReserveDown");

        tmpCapacityReservationUpB.type = ReserveType::UP;
        tmpCapacityReservationUpB.unsuppliedCost = 11;
        tmpCapacityReservationUpB.referenceActivationDuration = 12;
        tmpCapacityReservationUpB.powerActivationRatio = 13;
        tmpCapacityReservationUpB.energyActivationRatio = 14;
        tmpCapacityReservationUpB.setName("ReserveUpB");

        tmpCapacityReservationDownB.type = ReserveType::DOWN;
        tmpCapacityReservationDownB.unsuppliedCost = 15;
        tmpCapacityReservationDownB.referenceActivationDuration = 16;
        tmpCapacityReservationDownB.powerActivationRatio = 17;
        tmpCapacityReservationDownB.energyActivationRatio = 18;
        tmpCapacityReservationDownB.setName("ReserveDownB");

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

        areaA->allCapacityReservations->areaCapacityReservations.at("reserveup").need.resize(2);
        areaA->allCapacityReservations->areaCapacityReservations.at("reserveup").need[0] = 2;
        areaA->allCapacityReservations->areaCapacityReservations.at("reserveup").need[1] = 3;
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

BOOST_AUTO_TEST_SUITE(reserves_operations_import)

BOOST_FIXTURE_TEST_CASE(test_importCapacityReservation_allGood, OneProblemWithReservesTwoAreas)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    BOOST_CHECK_EQUAL(problemeHebdo->allReserves.has_value(), false);
    importCapacityReservations(study->areas, *problemeHebdo);
    BOOST_CHECK_EQUAL(problemeHebdo->allReserves.has_value(), true);
    int indexA = study->areas.find("a")->index;
    int indexB = study->areas.find("b")->index;
    BOOST_CHECK_EQUAL(problemeHebdo->allReserves->at(indexA).areaCapacityReservations.size(), 2);
    BOOST_CHECK_EQUAL(problemeHebdo->allReserves->at(indexB).areaCapacityReservations.size(), 2);
    bool containsUp = false;
    bool containsDown = false;
    for (auto& reserve: problemeHebdo->allReserves->at(indexA).areaCapacityReservations)
    {
        if (reserve.type == Antares::Data::ReserveType::UP)
        {
            BOOST_CHECK_EQUAL(reserve.unsuppliedCost, 1);
            BOOST_CHECK_EQUAL(reserve.referenceActivationDuration, 2);
            BOOST_CHECK_EQUAL(reserve.powerActivationRatio, 3);
            BOOST_CHECK_EQUAL(reserve.energyActivationRatio, 4);
            containsUp = true;
            BOOST_CHECK_EQUAL(reserve.need.size(), 2);
            BOOST_CHECK_EQUAL(reserve.need[0], 2);
            BOOST_CHECK_EQUAL(reserve.need[1], 3);
        }
        else
        {
            BOOST_CHECK_EQUAL(reserve.unsuppliedCost, 5);
            BOOST_CHECK_EQUAL(reserve.referenceActivationDuration, 6);
            BOOST_CHECK_EQUAL(reserve.powerActivationRatio, 7);
            BOOST_CHECK_EQUAL(reserve.energyActivationRatio, 8);
            containsDown = true;
        }
    }
    BOOST_CHECK_EQUAL(containsDown, true);
    BOOST_CHECK_EQUAL(containsUp, true);
    containsUp = false;
    containsDown = false;

    for (auto& reserve: problemeHebdo->allReserves->at(indexB).areaCapacityReservations)
    {
        if (reserve.type == Antares::Data::ReserveType::UP)
        {
            BOOST_CHECK_EQUAL(reserve.unsuppliedCost, 11);
            BOOST_CHECK_EQUAL(reserve.referenceActivationDuration, 12);
            BOOST_CHECK_EQUAL(reserve.powerActivationRatio, 13);
            BOOST_CHECK_EQUAL(reserve.energyActivationRatio, 14);
            containsUp = true;
        }
        else
        {
            BOOST_CHECK_EQUAL(reserve.unsuppliedCost, 15);
            BOOST_CHECK_EQUAL(reserve.referenceActivationDuration, 16);
            BOOST_CHECK_EQUAL(reserve.powerActivationRatio, 17);
            BOOST_CHECK_EQUAL(reserve.energyActivationRatio, 18);
            containsDown = true;
        }
    }
    BOOST_CHECK_EQUAL(containsDown, true);
    BOOST_CHECK_EQUAL(containsUp, true);
}

BOOST_FIXTURE_TEST_CASE(test_importHydroReserves, OneProblemWithReservesTwoAreas)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream file(studyPath / "myreserveA.yml");
    file << R"(participations:
  certifications:
    - reserve: ReserveUp
      participation-cost: 1.1
      max-store: 2.2
      max-release: 3.3
    - reserve: ReserveDown
      participation-cost: 4.4
      max-store: 5.5
      max-release: 6.6
)";
    file.close();

    std::ofstream fileB(studyPath / "myreserveB.yml");
    fileB << R"(participations:
  certifications:
    - reserve: ReserveUp
      participation-cost: 9.9
      max-store: 8.8
      max-release: 7.7
)";
    fileB.close();

    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserveA.yml");
    areaA->hydro.loadReserveParticipations(*areaB, studyPath / "myreserveB.yml");

    areaA->hydro.reserveParticipationContainer->addReserveParticipationSymmetry(
      {"reserveup", "reservedown"});

    importCapacityReservations(study->areas, *problemeHebdo);
    importHydroReserves(study->areas, *problemeHebdo);

    int indexA = study->areas.find("a")->index;
    int indexB = study->areas.find("b")->index;
    BOOST_CHECK_EQUAL(problemeHebdo->allReserves->at(indexA).areaCapacityReservations.size(), 2);
    BOOST_CHECK_EQUAL(problemeHebdo->allReserves->at(indexB).areaCapacityReservations.size(), 2);
    int maxGlobalHydroParticipationIndex;
    bool containsUp = false;
    bool containsDown = false;

    BOOST_CHECK_EQUAL(
      problemeHebdo->allReserves->at(indexA).HydroReservesParticipationSymmetries.size(),
      1);

    for (auto& reserve: problemeHebdo->allReserves->at(indexA).areaCapacityReservations)
    {
        BOOST_CHECK_EQUAL(reserve.AllHydroReservesParticipation.size(), 1);

        auto& part = reserve.AllHydroReservesParticipation[0];
        BOOST_CHECK_EQUAL(part.clusterName, "Hydro");
        if (reserve.type == Antares::Data::ReserveType::UP)
        {
            BOOST_CHECK_EQUAL(part.participationCost, 1.1);
            BOOST_CHECK_EQUAL(part.maxStore, 2.2);
            BOOST_CHECK_EQUAL(part.maxRelease, 3.3);
            maxGlobalHydroParticipationIndex = std::max(maxGlobalHydroParticipationIndex,
                                                        part.globalIndexClusterParticipation);
            BOOST_CHECK_EQUAL(reserve.reserveName, "ReserveUp");
            BOOST_CHECK_EQUAL(reserve.need.size(), 2);
            BOOST_CHECK_EQUAL(reserve.need[0], 2);
            BOOST_CHECK_EQUAL(reserve.need[1], 3);
            containsUp = true;
        }
        else
        {
            BOOST_CHECK_EQUAL(part.participationCost, 4.4);
            BOOST_CHECK_EQUAL(part.maxStore, 5.5);
            BOOST_CHECK_EQUAL(part.maxRelease, 6.6);
            maxGlobalHydroParticipationIndex = std::max(maxGlobalHydroParticipationIndex,
                                                        part.globalIndexClusterParticipation);
            BOOST_CHECK_EQUAL(reserve.reserveName, "ReserveDown");
            containsDown = true;
        }
    }
    BOOST_CHECK_EQUAL(containsDown, true);
    BOOST_CHECK_EQUAL(containsUp, true);
    containsUp = false;
    containsDown = false;
    for (auto& reserve: problemeHebdo->allReserves->at(indexB).areaCapacityReservations)
    {
        if (reserve.AllHydroReservesParticipation.size() == 1)
        {
            auto& part = reserve.AllHydroReservesParticipation[0];
            BOOST_CHECK_EQUAL(part.clusterName, "Hydro");
            if (reserve.type == Antares::Data::ReserveType::UP)
            {
                BOOST_CHECK_EQUAL(part.participationCost, 9.9);
                BOOST_CHECK_EQUAL(part.maxStore, 8.8);
                BOOST_CHECK_EQUAL(part.maxRelease, 7.7);
                maxGlobalHydroParticipationIndex = std::max(maxGlobalHydroParticipationIndex,
                                                            part.globalIndexClusterParticipation);
                BOOST_CHECK_EQUAL(reserve.reserveName, "ReserveUpB");
                containsUp = true;
            }
            else if (reserve.type == Antares::Data::ReserveType::DOWN)
            {
                containsDown = true;
            }
        }
        else
        {
            BOOST_CHECK_EQUAL(reserve.AllHydroReservesParticipation.size(), 0);
        }
    }
    BOOST_CHECK_EQUAL(containsDown, false);
    BOOST_CHECK_EQUAL(containsUp, true);
}

BOOST_AUTO_TEST_SUITE_END() // version
