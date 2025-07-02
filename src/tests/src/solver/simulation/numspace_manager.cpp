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
#define BOOST_TEST_MODULE numspace manager

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/solver/simulation/numspace_manager.h>

using namespace Antares::Solver::Simulation;

BOOST_AUTO_TEST_CASE(singleThread_allGood)
{
    NumSpaceManager manager(1);
    BOOST_CHECK_EQUAL(manager.getAvailableNumSpace(), 0);
    manager.freeNumSpace(0);
    BOOST_CHECK_EQUAL(manager.getAvailableNumSpace(), 0);
}

BOOST_AUTO_TEST_CASE(singleThread_depletion)
{
    NumSpaceManager manager(1);
    BOOST_CHECK_EQUAL(manager.getAvailableNumSpace(), 0);
    BOOST_CHECK_THROW(manager.getAvailableNumSpace(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(twoThreads_allGood)
{
    NumSpaceManager manager(2);
    BOOST_CHECK_EQUAL(manager.getAvailableNumSpace(), 0);
    BOOST_CHECK_EQUAL(manager.getAvailableNumSpace(), 1);
}

BOOST_AUTO_TEST_CASE(twoThreads_depletion)
{
    NumSpaceManager manager(2);
    BOOST_CHECK_EQUAL(manager.getAvailableNumSpace(), 0);
    BOOST_CHECK_EQUAL(manager.getAvailableNumSpace(), 1);
    BOOST_CHECK_THROW(manager.getAvailableNumSpace(), std::runtime_error);
}
