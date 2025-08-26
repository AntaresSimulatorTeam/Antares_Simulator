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

#include <antares/optimisation/linear-problem-data-impl/timeSeriesSet.h>

using namespace Antares::Optimisation::LinearProblemDataImpl;

BOOST_AUTO_TEST_CASE(adding_to_a_TS_set_a_TS_with_the_wrong_size___exception_raised)
{
    TimeSeriesSet timeSeriesSet("my-TS-set", 5);

    std::string expected_err_msg = "TS set 'my-TS-set' : add a TS of size 3 in a set of height 5";
    BOOST_CHECK_EXCEPTION(timeSeriesSet.add({1., 2., 3.}),
                          TimeSeriesSet::AddTSofWrongSize,
                          checkMessage(expected_err_msg));
}

BOOST_AUTO_TEST_CASE(ask_a_value_to_empty_TS_set___exception_raised)
{
    TimeSeriesSet timeSeriesSet("my-TS-set", 5);

    std::string expected_err_msg = "TS set 'my-TS-set' : empty, requesting a value makes no sense";
    BOOST_CHECK_EXCEPTION(timeSeriesSet.getData(1, 0),
                          TimeSeriesSet::Empty,
                          checkMessage(expected_err_msg));
}

BOOST_AUTO_TEST_CASE(ask_to_a_TS_set_for_an_out_of_range_TS_rank___exception_raised)
{
    TimeSeriesSet timeSeriesSet("my-TS-set", 3);
    timeSeriesSet.add({1., 2., 3.});

    std::string expected_err_msg = "TS set 'my-TS-set' : TS number 2 exceeds TS set's number of "
                                   "columns (1)";
    BOOST_CHECK_EXCEPTION(timeSeriesSet.getData(2, 0),
                          TimeSeriesSet::RankTooBig,
                          checkMessage(expected_err_msg));
}

BOOST_AUTO_TEST_CASE(ask_a_TS_set_for_an_out_of_range_hour___exception_raised)
{
    TimeSeriesSet timeSeriesSet("my-TS-set", 3);
    timeSeriesSet.add({1., 2., 3.});

    std::string expected_err_msg = "TS set 'my-TS-set' : hour 4 exceeds TS set's height";
    BOOST_CHECK_EXCEPTION(timeSeriesSet.getData(1, 4),
                          TimeSeriesSet::HourTooBig,
                          checkMessage(expected_err_msg));
}

BOOST_AUTO_TEST_CASE(ask_an_admissible_value_to_a_very_simple_TS_set__answer_is_correct)
{
    TimeSeriesSet timeSeriesSet("my-time-series-set", 5);
    timeSeriesSet.add({1., 2., 3., 4., 5});
    unsigned rank = 1;
    unsigned hour = 4;
    BOOST_CHECK_EQUAL(timeSeriesSet.getData(rank, hour), 5.);
}

BOOST_AUTO_TEST_CASE(ask_an_admissible_value_to_a_more_complex_TS_set__answer_is_correct)
{
    TimeSeriesSet timeSeriesSet("my-time-series-set", 5);
    timeSeriesSet.add({1., 2., 3., 4., 5});
    timeSeriesSet.add({11., 12., 13., 14., 15});
    timeSeriesSet.add({21., 22., 23., 24., 25});
    unsigned rank = 2;
    unsigned hour = 3;
    BOOST_CHECK_EQUAL(timeSeriesSet.getData(rank, hour), 14.);
}
