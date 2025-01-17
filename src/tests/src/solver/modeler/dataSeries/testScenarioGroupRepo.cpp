#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <unit_test_utils.h>
#include <antares/solver/modeler/dataSeries/scenarioGroupRepo.h>

using namespace Antares::Solver::Modeler::DataSeries;

BOOST_AUTO_TEST_CASE(ask_repo_a_question_it_can_answer___repo_answers_correctly)
{
    ScenarioGroupRepository scenarioGroupRepo;
    unsigned scenario = 10;
    unsigned dataRank = 15;
    scenarioGroupRepo.addPairScenarioRankToGroup("some group", {scenario, dataRank});

    BOOST_CHECK_EQUAL(scenarioGroupRepo.getDataRank("some group", scenario), dataRank);
}

BOOST_AUTO_TEST_CASE(ask_an_empty_repo_a_rank___exception_raised)
{
    ScenarioGroupRepository scenarioGroupRepo;

    BOOST_CHECK_EXCEPTION(scenarioGroupRepo.getDataRank("some group", 0),
                          std::invalid_argument,
                          checkMessage("Group 'some group' does not exist in group repo."));
}

BOOST_AUTO_TEST_CASE(ask_a_repo_a_rank_it_cannot_find___exception_raised)
{
    ScenarioGroupRepository scenarioGroupRepo;
    unsigned scenario = 10;
    unsigned dataRank = 15;
    scenarioGroupRepo.addPairScenarioRankToGroup("some group", {scenario, dataRank});

    BOOST_CHECK_EXCEPTION(scenarioGroupRepo.getDataRank("some group", 0),
                          std::invalid_argument,
                          checkMessage("In scenario group 'some group', scenario '0' does not exist."));
}
