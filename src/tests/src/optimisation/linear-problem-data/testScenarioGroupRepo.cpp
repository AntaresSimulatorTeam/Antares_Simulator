#define WIN32_LEAN_AND_MEAN

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/optimisation/linear-problem-data-impl/scenarioGroupRepo.h>

using namespace Antares::Optimisation::LinearProblemDataImpl;

BOOST_AUTO_TEST_CASE(ask_repo_a_question_it_can_answer___repo_answers_correctly)
{
    ScenarioGroupRepository scenarioGroupRepo;
    unsigned scenario = 10;
    unsigned dataRank = 15;
    scenarioGroupRepo.addPairScenarioRankToGroup("some group", {scenario, dataRank});

    BOOST_CHECK_EQUAL(scenarioGroupRepo.getDataRank("some group", scenario), dataRank);
}

BOOST_AUTO_TEST_CASE(add_to_repo_a_group_it_already_contains___exception_raised)
{
    ScenarioGroupRepository scenarioGroupRepo;
    scenarioGroupRepo.addPairScenarioRankToGroup("some group", {0, 0});

    std::string expectedErrMsg = "Scenario group 'some group' already exists in group repo.";
    BOOST_CHECK_EXCEPTION(scenarioGroupRepo.addPairScenarioRankToGroup("some group", {1, 1}),
                          ScenarioGroupRepository::AlreadyExists,
                          checkMessage(expectedErrMsg));
}

BOOST_AUTO_TEST_CASE(ask_an_empty_repo_a_rank___exception_raised)
{
    ScenarioGroupRepository scenarioGroupRepo;

    std::string expectedErrMsg = "Scenario group 'some group' does not exist in group repo.";
    BOOST_CHECK_EXCEPTION(scenarioGroupRepo.getDataRank("some group", 0),
                          ScenarioGroupRepository::DoesNotExist,
                          checkMessage(expectedErrMsg));
}

BOOST_AUTO_TEST_CASE(ask_a_repo_a_rank_it_cannot_find___exception_raised)
{
    ScenarioGroupRepository scenarioGroupRepo;
    unsigned scenario = 10;
    unsigned dataRank = 15;
    scenarioGroupRepo.addPairScenarioRankToGroup("some group", {scenario, dataRank});

    std::string expectedErrMsg = "In scenario group 'some group', scenario '0' does not exist.";
    BOOST_CHECK_EXCEPTION(scenarioGroupRepo.getDataRank("some group", 0),
                          ScenarioGroupRepository::ScenarioNotExist,
                          checkMessage(expectedErrMsg));
}
