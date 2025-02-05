#define WIN32_LEAN_AND_MEAN

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/dataSeries/linearProblemData.h>
#include <antares/solver/modeler/dataSeries/timeSeriesSet.h>

using namespace Antares::Solver::Modeler::DataSeries;

BOOST_AUTO_TEST_CASE(PbData_is_empty__asking_it_a_value_leads_to_exception)
{
    LinearProblemData linearProblemData;
    std::string expected_err_msg = "Scenario group 'group name' does not exist in group repo.";
    BOOST_CHECK_EXCEPTION(linearProblemData.getData("data set name", "group name", 0, 0),
                          std::invalid_argument,
                          checkMessage(expected_err_msg));
}

BOOST_AUTO_TEST_CASE(ask_to_a_simple_linearProblemData_data_it_contains___answer_ok)
{
    LinearProblemData linearProblemData;

    // Adding a scenario group to the linear problem data
    const unsigned year = 0;
    const unsigned rank = 0;
    const std::string groupName = "group 1";
    linearProblemData.addScenarioGroup(groupName, {year, rank});

    // Adding a data set to the linear problem data
    const std::string dataSetName = "my-TS-set";
    const unsigned height = 5;
    auto timeSeriesSet = std::make_unique<TimeSeriesSet>(dataSetName, height);
    timeSeriesSet->add({10., 20., 30., 40., 50.});
    linearProblemData.addDataSeries(std::move(timeSeriesSet));

    const unsigned hour = 3;
    BOOST_CHECK_EQUAL(linearProblemData.getData(dataSetName, groupName, year, hour), 40.);
}
