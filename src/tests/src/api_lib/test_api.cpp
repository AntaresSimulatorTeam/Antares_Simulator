// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test_api
#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/api/solver.h"

BOOST_AUTO_TEST_CASE(result_failure_when_study_path_invalid)
{
    using namespace std::string_literals;
    auto results = Antares::API::PerformSimulation("dummy"s, {}, {});
    BOOST_CHECK(results.error);
    BOOST_CHECK(!results.error->reason.empty());
}
