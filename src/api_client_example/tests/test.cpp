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
