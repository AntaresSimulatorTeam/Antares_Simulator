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
#include <algorithm>
#include <files-system.h>
#include <filesystem>

#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <antares/solver/simulation/forTestsOnlySimCalculEco.h>
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

BOOST_AUTO_TEST_SUITE(reserves_operations_import)

BOOST_FIXTURE_TEST_CASE(test_importCapacityReservation_allGood, OneProblemWithReservesTwoAreas)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    BOOST_CHECK_EQUAL(problemeHebdo->allReserves.has_value(), false);
    importCapacityReservations(study->areas, *problemeHebdo);
    BOOST_CHECK_EQUAL(problemeHebdo->allReserves.has_value(), true);
    int indexA = study->areas.find("a")->index;
    int indexB = study->areas.find("b")->index;
    BOOST_CHECK_EQUAL(problemeHebdo->allReserves.value()[indexA].areaCapacityReservations.size(),
                      2);
    BOOST_CHECK_EQUAL(problemeHebdo->allReserves.value()[indexB].areaCapacityReservations.size(),
                      2);
    bool containsUp = false;
    bool containsDown = false;
    for (auto& reserve: problemeHebdo->allReserves.value()[indexA].areaCapacityReservations)
    {
        if (reserve.type == Antares::Data::ReserveType::UP)
        {
            BOOST_CHECK_EQUAL(reserve.unsuppliedCost, 1);
            BOOST_CHECK_EQUAL(reserve.referenceActivationDuration, 2);
            BOOST_CHECK_EQUAL(reserve.powerActivationRatio, 3);
            BOOST_CHECK_EQUAL(reserve.energyActivationRatio, 4);
            containsUp = true;
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

    for (auto& reserve: problemeHebdo->allReserves.value()[indexB].areaCapacityReservations)
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

    std::ofstream file(studyPath / "myreserveA.ini");
    file << "[ReserveUp]\n";
    file << "participation-cost = 1.1\n";
    file << "max-store = 2.2\n";
    file << "max-release = 3.3\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "participation-cost = 4.4\n";
    file << "max-store = 5.5\n";
    file << "max-release = 6.6\n";
    file.close();
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserveA.ini");

    std::ofstream fileB(studyPath / "myreserveB.ini");
    fileB << "[ReserveUp]\n";
    fileB << "participation-cost = 9.9\n";
    fileB << "max-store = 8.8\n";
    fileB << "max-release = 7.7\n";
    fileB.close();
    areaA->hydro.loadReserveParticipations(*areaB, studyPath / "myreserveB.ini");

    importCapacityReservations(study->areas, *problemeHebdo);
    importHydroReserves(study->areas, *problemeHebdo);
    int indexA = study->areas.find("a")->index;
    int indexB = study->areas.find("b")->index;
    BOOST_CHECK_EQUAL(problemeHebdo->allReserves.value()[indexA].areaCapacityReservations.size(),
                      2);
    BOOST_CHECK_EQUAL(problemeHebdo->allReserves.value()[indexB].areaCapacityReservations.size(),
                      2);
    int maxGlobalHydroParticipationIndex;
    bool containsUp = false;
    bool containsDown = false;
    for (auto& reserve: problemeHebdo->allReserves.value()[indexA].areaCapacityReservations)
    {
        BOOST_CHECK_EQUAL(reserve.AllHydroReservesParticipation.size(), 1);
        auto& part = reserve.AllHydroReservesParticipation[0];
        if (reserve.type == Antares::Data::ReserveType::UP)
        {
            BOOST_CHECK_EQUAL(part.participationCost, 1.1);
            BOOST_CHECK_EQUAL(part.maxStore, 2.2);
            BOOST_CHECK_EQUAL(part.maxRelease, 3.3);
            maxGlobalHydroParticipationIndex = std::max(maxGlobalHydroParticipationIndex,
                                                        part.globalIndexClusterParticipation);
            containsUp = true;
        }
        else
        {
            BOOST_CHECK_EQUAL(part.participationCost, 4.4);
            BOOST_CHECK_EQUAL(part.maxStore, 5.5);
            BOOST_CHECK_EQUAL(part.maxRelease, 6.6);
            maxGlobalHydroParticipationIndex = std::max(maxGlobalHydroParticipationIndex,
                                                        part.globalIndexClusterParticipation);
            containsDown = true;
        }
    }
    BOOST_CHECK_EQUAL(containsDown, true);
    BOOST_CHECK_EQUAL(containsUp, true);
    containsUp = false;
    containsDown = false;
    for (auto& reserve: problemeHebdo->allReserves.value()[indexB].areaCapacityReservations)
    {
        if (reserve.AllHydroReservesParticipation.size() == 1)
        {
            auto& part = reserve.AllHydroReservesParticipation[0];
            if (reserve.type == Antares::Data::ReserveType::UP)
            {
                BOOST_CHECK_EQUAL(part.participationCost, 9.9);
                BOOST_CHECK_EQUAL(part.maxStore, 8.8);
                BOOST_CHECK_EQUAL(part.maxRelease, 7.7);
                maxGlobalHydroParticipationIndex = std::max(maxGlobalHydroParticipationIndex,
                                                            part.globalIndexClusterParticipation);
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
