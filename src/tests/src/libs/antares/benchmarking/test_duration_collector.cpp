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

#define BOOST_TEST_MODULE test - benchmarking
#define WIN32_LEAN_AND_MEAN
#include <thread>

#include <boost/test/unit_test.hpp>

#include <antares/benchmarking/DurationCollector.h>
#include <antares/benchmarking/timer.h>

BOOST_AUTO_TEST_SUITE(durationCollector)

using namespace std::literals::chrono_literals;

constexpr double threshold = 30;

BOOST_AUTO_TEST_CASE(lambda)
{
    Benchmarking::DurationCollector d;

    d("test1") << [] { [[maybe_unused]] int a; };
    BOOST_CHECK_CLOSE((double)d.getTime("test1"), 0., threshold);

    d("test2") << [] { std::this_thread::sleep_for(200ms); };
    BOOST_CHECK_CLOSE((double)d.getTime("test2"), 200., threshold);

    d("test3") << [&d]
    {
        d("test4") << [] { std::this_thread::sleep_for(100ms); };
        std::this_thread::sleep_for(100ms);
    };

    BOOST_CHECK_CLOSE((double)d.getTime("test3"), 200., threshold);
    BOOST_CHECK_CLOSE((double)d.getTime("test4"), 100., threshold);
}

BOOST_AUTO_TEST_CASE(exceptionHandling)
{
    Benchmarking::DurationCollector d;

    auto IShouldThrowAnInt = [&d]() { d("test1") << [] { throw int(2); }; };

    BOOST_CHECK_THROW(IShouldThrowAnInt(), int);
}

BOOST_AUTO_TEST_CASE(addDuration)
{
    Benchmarking::DurationCollector d;
    Benchmarking::Timer t;

    std::this_thread::sleep_for(100ms);
    t.stop();
    d.addDuration("test1", t.get_duration());

    BOOST_CHECK_CLOSE((double)d.getTime("test1"), 100., threshold);
}

BOOST_AUTO_TEST_SUITE_END() // DurationCollector
