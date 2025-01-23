#define WIN32_LEAN_AND_MEAN

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/dataSeries/scenarioGroupRepo.h>
#include "antares/solver/modeler/dataSeries/scenarioGroupRepoExceptions.h"

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

    std::string expectedErrMsg = "Scenario group 'some group' does not exist in group repo.";
    BOOST_CHECK_EXCEPTION(scenarioGroupRepo.getDataRank("some group", 0),
                          ScGroup_DoesNotExist,
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
                          ScGroup_ScenarioNotExist,
                          checkMessage(expectedErrMsg));
}
