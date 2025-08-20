/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#define WIN32_LEAN_AND_MEAN

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/solver/optim-model-filler/scenarioGroupRepo.h>
#include "antares/exception/RuntimeError.hpp"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"

using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemDataImpl;

BOOST_AUTO_TEST_CASE(ask_repo_a_question_it_can_answer_repo_answers_correctly)
{
    ScenarioGroupRepository scenarioGroupRepo;
    unsigned scenario = 10;
    unsigned dataRank = 15;
    auto scenarioPtr = std::make_unique<Scenario>("some group");
    scenarioPtr->setTimeSerieNumber(scenario, dataRank);
    scenarioGroupRepo.addScenario("some group", std::move(scenarioPtr));

    BOOST_CHECK_EQUAL(scenarioGroupRepo.scenario("some group").getData(scenario), dataRank);
}

BOOST_AUTO_TEST_CASE(add_to_repo_a_group_it_already_contains_exception_raised)
{
    ScenarioGroupRepository scenarioGroupRepo;
    auto scenarioPtr = std::make_unique<Scenario>("some group");
    scenarioGroupRepo.addScenario("some group", std::move(scenarioPtr));

    auto anotherScenarioPtr = std::make_unique<Scenario>("some group");

    std::string expectedErrMsg = "Scenario group 'SOME GROUP' already exists in group repo.";
    BOOST_CHECK_EXCEPTION(scenarioGroupRepo.addScenario("some group",
                                                        std::move(anotherScenarioPtr)),
                          ScenarioGroupRepository::AlreadyExists,
                          checkMessage(expectedErrMsg));
}

BOOST_AUTO_TEST_CASE(ask_an_empty_repo_a_rank_exception_raised)
{
    ScenarioGroupRepository scenarioGroupRepo;

    std::string expectedErrMsg = "Scenario group 'SOME GROUP' does not exist in group repo.";
    BOOST_CHECK_EXCEPTION((void)scenarioGroupRepo.scenario("some group"),
                          ScenarioGroupRepository::DoesNotExist,
                          checkMessage(expectedErrMsg));
}

BOOST_AUTO_TEST_CASE(ask_a_repo_a_rank_it_cannot_find_exception_raised)
{
    ScenarioGroupRepository scenarioGroupRepo;
    auto scenarioPtr = std::make_unique<Scenario>("some group");
    scenarioGroupRepo.addScenario("some group", std::move(scenarioPtr));

    std::string expectedErrMsg = "In scenario group 'SOME GROUP', time serie for year 0 does not "
                                 "exist.";
    BOOST_CHECK_EXCEPTION((void)scenarioGroupRepo.scenario("some group").getData(0),
                          Antares::Error::RuntimeError,
                          checkMessage(expectedErrMsg));
}

BOOST_AUTO_TEST_CASE(empty_group_id_returns_default_rank)
{
    ScenarioGroupRepository scenarioGroupRepo;
    unsigned scenario = 10;
    unsigned dataRank = 15;
    auto scenarioPtr = std::make_unique<Scenario>("some group");
    scenarioPtr->setTimeSerieNumber(scenario, dataRank);
    scenarioGroupRepo.addScenario("some group", std::move(scenarioPtr));

    BOOST_CHECK_EQUAL(scenarioGroupRepo.scenario("").getData(scenario), 1);
}

BOOST_AUTO_TEST_CASE(set_should_fail_if_timeserie_exists)
{
    ScenarioGroupRepository scenarioGroupRepo;
    unsigned scenario = 10;
    unsigned dataRank = 15;
    auto scenarioPtr = std::make_unique<Scenario>("some group");
    scenarioPtr->setTimeSerieNumber(scenario, dataRank);
    BOOST_CHECK_EXCEPTION(scenarioPtr->setTimeSerieNumber(scenario, dataRank),
                          Antares::Error::RuntimeError,
                          checkMessage("Time serie number for year 10 already exists."));
}
