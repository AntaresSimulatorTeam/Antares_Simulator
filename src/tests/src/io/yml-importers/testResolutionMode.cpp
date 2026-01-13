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

#include <boost/test/unit_test.hpp>

#include "antares/io/inputs/yml-optim-config/parser.h"

using namespace Antares::IO::Inputs::YmlOptimConfig;

BOOST_AUTO_TEST_SUITE(ResolutionModeIntegrationTests)

BOOST_AUTO_TEST_CASE(investment_study_with_benders_allows_scenario_independent_vars)
{
    // This test validates the integration:
    // When resolution-mode is set to benders-decomposition,
    // scenario-independent variables (like investment variables) are allowed
    std::string yaml_content = R"(
resolution-mode: benders-decomposition
models:
  - id: lib_thermal_invest.thermal_candidate
    model-decomposition:
      variables:
        - id: p_max
          location: master
        - id: nb_units
          location: master
        - id: power_output
          location: master-and-subproblems
      objective-contributions:
        - id: capex
          location: master
        - id: opex
          location: subproblems
      constraints:
        - id: capacity_limit
          location: master-and-subproblems
)";

    Parser parser;
    OptimConfig config = parser.parse(yaml_content);

    // Verify resolution mode is set correctly
    BOOST_CHECK_EQUAL(config.resolution_mode, ResolutionMode::BendersDecomposition);

    // Verify models are loaded
    BOOST_REQUIRE_EQUAL(config.models.size(), 1);
    BOOST_CHECK_EQUAL(config.models[0].id, "lib_thermal_invest.thermal_candidate");

    // Verify variables are loaded (including scenario-independent ones)
    BOOST_REQUIRE_EQUAL(config.models[0].variables.size(), 3);
    BOOST_CHECK_EQUAL(config.models[0].variables[0].id, "p_max");
    BOOST_CHECK_EQUAL(config.models[0].variables[0].location, "master");
    BOOST_CHECK_EQUAL(config.models[0].variables[1].id, "nb_units");
    BOOST_CHECK_EQUAL(config.models[0].variables[1].location, "master");
}

BOOST_AUTO_TEST_CASE(simulation_study_default_mode_is_sequential_subproblems)
{
    // Default mode should be sequential-subproblems (for backward compatibility)
    std::string yaml_content = R"(
models:
  - id: lib_model.model1
    model-decomposition:
      variables:
        - id: var1
          location: master
      objective-contributions:
        - id: obj1
          location: master
      constraints: []
)";

    Parser parser;
    OptimConfig config = parser.parse(yaml_content);

    // Verify default resolution mode
    BOOST_CHECK_EQUAL(config.resolution_mode, ResolutionMode::SequentialSubproblems);
    BOOST_REQUIRE_EQUAL(config.models.size(), 1);
}

BOOST_AUTO_TEST_CASE(explicit_sequential_subproblems_mode)
{
    std::string yaml_content = R"(
resolution-mode: sequential-subproblems
models:
  - id: lib_model.model1
    model-decomposition:
      variables: []
      objective-contributions: []
      constraints: []
)";

    Parser parser;
    OptimConfig config = parser.parse(yaml_content);

    BOOST_CHECK_EQUAL(config.resolution_mode, ResolutionMode::SequentialSubproblems);
}

BOOST_AUTO_TEST_CASE(multiple_models_with_benders_mode)
{
    // Test realistic scenario with multiple investment models
    std::string yaml_content = R"(
resolution-mode: benders-decomposition
models:
  - id: lib_thermal.coal_investment
    model-decomposition:
      variables:
        - id: capacity
          location: master
        - id: power
          location: master-and-subproblems
      objective-contributions:
        - id: capex
          location: master
        - id: opex
          location: subproblems
      constraints:
        - id: gen_constraint
          location: master-and-subproblems

  - id: lib_renewable.wind_investment
    model-decomposition:
      variables:
        - id: installed_capacity
          location: master
      objective-contributions:
        - id: investment_cost
          location: master
      constraints: []
)";

    Parser parser;
    OptimConfig config = parser.parse(yaml_content);

    BOOST_CHECK_EQUAL(config.resolution_mode, ResolutionMode::BendersDecomposition);
    BOOST_REQUIRE_EQUAL(config.models.size(), 2);

    // First model
    BOOST_CHECK_EQUAL(config.models[0].id, "lib_thermal.coal_investment");
    BOOST_REQUIRE_EQUAL(config.models[0].variables.size(), 2);
    BOOST_CHECK_EQUAL(config.models[0].variables[0].id, "capacity");
    BOOST_CHECK_EQUAL(config.models[0].variables[0].location, "master");

    // Second model
    BOOST_CHECK_EQUAL(config.models[1].id, "lib_renewable.wind_investment");
    BOOST_REQUIRE_EQUAL(config.models[1].variables.size(), 1);
    BOOST_CHECK_EQUAL(config.models[1].variables[0].id, "installed_capacity");
}

BOOST_AUTO_TEST_SUITE_END()

