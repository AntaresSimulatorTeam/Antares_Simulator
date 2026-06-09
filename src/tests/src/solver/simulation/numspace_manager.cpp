// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
