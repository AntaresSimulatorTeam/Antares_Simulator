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
#define BOOST_TEST_MODULE toybox

#define WIN32_LEAN_AND_MEAN

#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>

#include <boost/test/unit_test.hpp>

// Simple test case
BOOST_AUTO_TEST_CASE(test_case1)
{
    std::string model = R"(
library:
format-version: "1.0"
id: basic
description: Basic library

port-types:
  - id: flow
    description: A port which transfers power flow
    fields:
      - name: flow

models:
  - id: generator
    description: A basic generator model
    parameters:
      - name: cost
        time-dependent: false
        scenario-dependent: false
      - name: value
        time-dependent: false
        scenario-dependent: false
    objective: "cost * value"
)";

    std::string component1 = R"(
"study:
nodes:
- id: small_generator
  model: generator
  parameters:
    cost: 10
    value: 100
")";

    YAML::Node modely = YAML::Load(model);
    YAML::Node componenty = YAML::Load(component1);
}
