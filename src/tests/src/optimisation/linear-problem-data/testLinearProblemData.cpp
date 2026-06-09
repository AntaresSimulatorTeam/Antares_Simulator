// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/optimisation/linear-problem-data-impl/linearProblemData.h>
#include <antares/optimisation/linear-problem-data-impl/timeSeriesSet.h>

using namespace Antares::Optimisation::LinearProblemDataImpl;

BOOST_AUTO_TEST_CASE(PbData_is_empty_asking_it_a_value_leads_to_exception)
{
    LinearProblemData linearProblemData;
    std::string expected_err_msg = "Data series repo is empty, and somebody requests data from it";
    BOOST_CHECK_EXCEPTION((void)linearProblemData.getData("data set name", 0, 0),
                          std::invalid_argument,
                          checkMessage(expected_err_msg));
}

BOOST_AUTO_TEST_CASE(ask_to_a_simple_linearProblemData_data_it_contains_answer_ok)
{
    LinearProblemData linearProblemData;

    // Adding a scenario group to the linear problem data
    const unsigned timeSriesNumber = 1;
    const std::string groupName = "group 1";

    // Adding a data set to the linear problem data
    const std::string dataSetName = "my-TS-set";
    const unsigned height = 5;
    auto timeSeriesSet = std::make_unique<TimeSeriesSet>(dataSetName, height);
    timeSeriesSet->add({10., 20., 30., 40., 50.});
    linearProblemData.addDataSeries(std::move(timeSeriesSet));

    const unsigned hour = 3;
    BOOST_CHECK_EQUAL(linearProblemData.getData(dataSetName, timeSriesNumber, hour), 40.);
}
