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

#define WIN32_LEAN_AND_MEAN

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>

#include "antares/solver/optim-model-filler/ComponentFiller.h"

using Dimensions = Antares::Optimization::Dimensions;

BOOST_AUTO_TEST_SUITE(DimensionsSuite)

BOOST_AUTO_TEST_CASE(no_scenarios)
{
    Dimensions dim({}, Dimensions::TimeInterval(0, 2));
    BOOST_CHECK_EQUAL(dim.getNumberOfTimesteps(), 3);
    const std::vector<int> expected_ts{0, 1, 2};
    const std::vector<int> ts = dim.getTimesteps();
    BOOST_CHECK_EQUAL_COLLECTIONS(expected_ts.begin(), expected_ts.end(), ts.begin(), ts.end());

    BOOST_CHECK(dim.getScenarioIndices() == std::vector<int>{0});
}
BOOST_AUTO_TEST_SUITE_END()
