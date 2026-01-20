// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#define BOOST_TEST_MODULE test_client_api

#include <boost/test/unit_test.hpp>

#include "API_client.h"

BOOST_AUTO_TEST_CASE(test_run)
{
    auto results = solve("dummy_study_test_client_api", {});
    BOOST_CHECK(results.error);
    BOOST_CHECK(!results.error->reason.empty());
    auto c = results.error->reason;
    std::cout << c << std::endl;
    BOOST_CHECK(results.error->reason.find("Study") != std::string::npos);
    BOOST_CHECK(results.error->reason.find("folder") != std::string::npos);
    BOOST_CHECK(results.error->reason.find("not") != std::string::npos);
    BOOST_CHECK(results.error->reason.find("exist") != std::string::npos);
}
