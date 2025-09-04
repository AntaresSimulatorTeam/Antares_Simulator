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
#include <memory>
#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/optimisation/linear-problem-data-impl/dataSeriesRepo.h>
#include <antares/optimisation/linear-problem-data-impl/timeSeriesSet.h>

using namespace Antares::Optimisation::LinearProblemDataImpl;

BOOST_AUTO_TEST_CASE(repo_is_empty__asking_any_data_series_raises_exception)
{
    DataSeriesRepository dataSeriesRepository;

    std::string expected_err_msg = "Data series repo is empty, and somebody requests data from it";
    BOOST_CHECK_EXCEPTION((void)dataSeriesRepository.getDataSeries("dummy name"),
                          DataSeriesRepository::Empty,
                          checkMessage(expected_err_msg));
}

BOOST_AUTO_TEST_CASE(adding_to_repo_a_data_it_already_contains___exception_raised)
{
    DataSeriesRepository dataSeriesRepository;
    auto some_TS_set = std::make_unique<TimeSeriesSet>("some TS set", 5);
    dataSeriesRepository.addDataSeries(std::move(some_TS_set));

    // This TS set wears the same name as the previous one
    auto some_other_TS_set = std::make_unique<TimeSeriesSet>("some TS set", 7);

    std::string expected_err_msg = "Data series repo : data series 'some TS set' already exists";
    BOOST_CHECK_EXCEPTION(dataSeriesRepository.addDataSeries(std::move(some_other_TS_set)),
                          DataSeriesRepository::DataSeriesAlreadyExists,
                          checkMessage(expected_err_msg));
}

BOOST_AUTO_TEST_CASE(repo_not_empty__asking_nonexistent_data_raises_exception)
{
    DataSeriesRepository dataSeriesRepository;
    auto some_TS_set = std::make_unique<TimeSeriesSet>("some TS set", 5);
    dataSeriesRepository.addDataSeries(std::move(some_TS_set));

    std::string expected_err_msg = "Data series repo : data series 'dummy name' does not exist";
    BOOST_CHECK_EXCEPTION((void)dataSeriesRepository.getDataSeries("dummy name"),
                          DataSeriesRepository::DataSeriesNotExist,
                          checkMessage(expected_err_msg));
}

BOOST_AUTO_TEST_CASE(ask_a_simple_data_repo_some_data_it_contains___answer_is_correct)
{
    DataSeriesRepository dataSeriesRepository;

    auto some_TS_set = std::make_unique<TimeSeriesSet>("some TS set", 5);
    some_TS_set->add({1., 2., 3., 4., 5.});
    some_TS_set->add({11., 12., 13., 14., 15.});

    dataSeriesRepository.addDataSeries(std::move(some_TS_set));

    Antares::Optimisation::LinearProblemApi::IScenario::TimeSeriesNumber tsNumber = 2;
    unsigned hour = 3;
    BOOST_CHECK_EQUAL(dataSeriesRepository.getDataSeries("some TS set").getData(tsNumber, hour),
                      14.);
}

BOOST_AUTO_TEST_CASE(asking_repo_data_for_a_too_big_hour___exception_from_data_series_is_caught)
{
    DataSeriesRepository dataSeriesRepository;
    auto some_TS_set = std::make_unique<TimeSeriesSet>("some TS set", 5);
    some_TS_set->add({1., 2., 3., 4., 5.});
    dataSeriesRepository.addDataSeries(std::move(some_TS_set));

    Antares::Optimisation::LinearProblemApi::IScenario::TimeSeriesNumber tsNumber = 1;
    unsigned hour = 100; // Hour too big
    std::string expected_err_msg = "TS set 'some TS set' : hour 100 exceeds TS set's height";
    BOOST_CHECK_EXCEPTION(
      (void)dataSeriesRepository.getDataSeries("some TS set").getData(tsNumber, hour),
      TimeSeriesSet::HourTooBig,
      checkMessage(expected_err_msg));
}

BOOST_AUTO_TEST_CASE(ask_a_more_complex_data_repo_some_data_it_contains___answer_is_correct)
{
    DataSeriesRepository dataSeriesRepository;

    auto TS_set_1 = std::make_unique<TimeSeriesSet>("TS set 1", 5);
    TS_set_1->add({1., 2., 3., 4., 5.});
    TS_set_1->add({11., 12., 13., 14., 15.});

    auto TS_set_2 = std::make_unique<TimeSeriesSet>("TS set 2", 5);
    TS_set_2->add({21., 22., 23., 24., 25.});
    TS_set_2->add({31., 32., 33., 34., 35.});
    TS_set_2->add({41., 42., 43., 44., 45.});

    dataSeriesRepository.addDataSeries(std::move(TS_set_1));
    dataSeriesRepository.addDataSeries(std::move(TS_set_2));

    Antares::Optimisation::LinearProblemApi::IScenario::TimeSeriesNumber tsNumber = 2;
    unsigned hour = 3;
    BOOST_CHECK_EQUAL(dataSeriesRepository.getDataSeries("TS set 1").getData(tsNumber, hour), 14.);

    tsNumber = 3;
    hour = 1;
    BOOST_CHECK_EQUAL(dataSeriesRepository.getDataSeries("TS set 2").getData(tsNumber, hour), 42.);
}
