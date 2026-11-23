// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/modeler-optimisation-container/scenarioGroupRepo.h>
#include "antares/exception/RuntimeError.hpp"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"

using namespace Antares::LinearProblem;
using namespace Antares::LinearProblem::DataImpl;

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

    std::unique_ptr<Api::IScenario> anotherScenarioPtr = std::make_unique<Scenario>("some group");
    const auto* originalScenario = &scenarioGroupRepo.scenario("some group");
    const auto* rejectedScenario = anotherScenarioPtr.get();

    std::string expectedErrMsg = "Scenario group 'SOME GROUP' already exists in group repo.";
    BOOST_CHECK_EXCEPTION(scenarioGroupRepo.addScenario("some group",
                                                        std::move(anotherScenarioPtr)),
                          ScenarioGroupRepository::AlreadyExists,
                          containsMessage(expectedErrMsg));
    BOOST_CHECK(anotherScenarioPtr.get() == rejectedScenario);
    BOOST_CHECK(&scenarioGroupRepo.scenario("some group") == originalScenario);
}

BOOST_AUTO_TEST_CASE(ask_an_empty_repo_a_rank_exception_raised)
{
    ScenarioGroupRepository scenarioGroupRepo;

    std::string expectedErrMsg = "Scenario group 'SOME GROUP' does not exist in group repo.";
    BOOST_CHECK_EXCEPTION((void)scenarioGroupRepo.scenario("some group"),
                          ScenarioGroupRepository::DoesNotExist,
                          containsMessage(expectedErrMsg));
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
                          containsMessage(expectedErrMsg));
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
    BOOST_CHECK_EXCEPTION(scenarioPtr->setTimeSerieNumber(scenario, dataRank + 1),
                          Antares::Error::RuntimeError,
                          containsMessage("Time serie number for year 10 already exists."));
    BOOST_CHECK_EQUAL(scenarioPtr->getData(scenario), dataRank);
}
