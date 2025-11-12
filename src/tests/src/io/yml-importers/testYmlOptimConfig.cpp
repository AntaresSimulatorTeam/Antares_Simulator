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

#include <yaml-cpp/exceptions.h>

#include <boost/test/unit_test.hpp>

#include "antares/io/inputs/yml-optim-config/parser.h"
using namespace Antares::IO::Inputs::YmlOptimConfig;
using namespace std::string_literals;

using namespace Antares::IO::Inputs::YmlOptimConfig;

BOOST_AUTO_TEST_SUITE(ParserTests)

BOOST_AUTO_TEST_CASE(parse_complete_valid_yaml)
{
    std::string yaml_content = R"(
models:
  - id: lib_thermal_invest.thermal_candidate
    model-decomposition:
      variables:
        - id: nb_units
          location: master
        - id: pmax_cluster
          location: master-and-subproblems
      objective-contributions:
        - id: invest_objective
          location: master
        - id: operational_objective
          location: subproblems
  - id: lib_thermal_invest.my_other_model
    model-decomposition:
      variables:
        - id: var1
          location: subproblems
      objective-contributions:
        - id: obj1
          location: master
)";

    Parser parser;
    OptimConfig config = parser.parse(yaml_content);

    BOOST_REQUIRE_EQUAL(config.size(), 2);

    // First model
    BOOST_CHECK_EQUAL(config[0].id, "lib_thermal_invest.thermal_candidate");
    BOOST_REQUIRE_EQUAL(config[0].variables.size(), 2);
    BOOST_CHECK_EQUAL(config[0].variables[0].id, "nb_units");
    BOOST_CHECK(config[0].variables[0].location == "master");
    BOOST_CHECK_EQUAL(config[0].variables[1].id, "pmax_cluster");
    BOOST_CHECK(config[0].variables[1].location == "master-and-subproblems");

    BOOST_REQUIRE_EQUAL(config[0].objectives.size(), 2);
    BOOST_CHECK_EQUAL(config[0].objectives[0].id, "invest_objective");
    BOOST_CHECK(config[0].objectives[0].location == "master");
    BOOST_CHECK_EQUAL(config[0].objectives[1].id, "operational_objective");
    BOOST_CHECK(config[0].objectives[1].location == "subproblems");

    // Second model
    BOOST_CHECK_EQUAL(config[1].id, "lib_thermal_invest.my_other_model");
    BOOST_REQUIRE_EQUAL(config[1].variables.size(), 1);
    BOOST_CHECK_EQUAL(config[1].variables[0].id, "var1");
    BOOST_CHECK(config[1].variables[0].location == "subproblems");

    BOOST_REQUIRE_EQUAL(config[1].objectives.size(), 1);
    BOOST_CHECK_EQUAL(config[1].objectives[0].id, "obj1");
    BOOST_CHECK(config[1].objectives[0].location == "master");
}

BOOST_AUTO_TEST_CASE(parse_single_model)
{
    std::string yaml_content = R"(
models:
  - id: single_model
    model-decomposition:
      variables:
        - id: single_var
          location: master
      objective-contributions:
        - id: single_obj
          location: subproblems
)";

    Parser parser;
    OptimConfig config = parser.parse(yaml_content);

    BOOST_REQUIRE_EQUAL(config.size(), 1);
    BOOST_CHECK_EQUAL(config[0].id, "single_model");
    BOOST_REQUIRE_EQUAL(config[0].variables.size(), 1);
    BOOST_CHECK_EQUAL(config[0].variables[0].id, "single_var");
    BOOST_REQUIRE_EQUAL(config[0].objectives.size(), 1);
    BOOST_CHECK_EQUAL(config[0].objectives[0].id, "single_obj");
}

BOOST_AUTO_TEST_CASE(parse_empty_models_list)
{
    std::string yaml_content = R"(
models: []
)";

    Parser parser;
    OptimConfig config = parser.parse(yaml_content);

    BOOST_CHECK_EQUAL(config.size(), 0);
}

BOOST_AUTO_TEST_CASE(parse_model_with_empty_variables)
{
    std::string yaml_content = R"(
models:
  - id: model_no_vars
    model-decomposition:
      variables: []
      objective-contributions:
        - id: obj1
          location: master
)";

    Parser parser;
    OptimConfig config = parser.parse(yaml_content);

    BOOST_REQUIRE_EQUAL(config.size(), 1);
    BOOST_CHECK_EQUAL(config[0].variables.size(), 0);
    BOOST_REQUIRE_EQUAL(config[0].objectives.size(), 1);
}

BOOST_AUTO_TEST_CASE(parse_model_with_empty_objectives)
{
    std::string yaml_content = R"(
models:
  - id: model_no_objs
    model-decomposition:
      variables:
        - id: var1
          location: master
      objective-contributions: []
)";

    Parser parser;
    OptimConfig config = parser.parse(yaml_content);

    BOOST_REQUIRE_EQUAL(config.size(), 1);
    BOOST_REQUIRE_EQUAL(config[0].variables.size(), 1);
    BOOST_CHECK_EQUAL(config[0].objectives.size(), 0);
}

BOOST_AUTO_TEST_CASE(parse_all_location_types)
{
    std::string yaml_content = R"(
models:
  - id: test_locations
    model-decomposition:
      variables:
        - id: var_master
          location: master
        - id: var_sub
          location: subproblems
        - id: var_both
          location: master-and-subproblems
      objective-contributions:
        - id: obj_master
          location: master
        - id: obj_sub
          location: subproblems
        - id: obj_both
          location: master-and-subproblems
)";

    Parser parser;
    OptimConfig config = parser.parse(yaml_content);

    BOOST_REQUIRE_EQUAL(config.size(), 1);
    BOOST_REQUIRE_EQUAL(config[0].variables.size(), 3);
    BOOST_CHECK(config[0].variables[0].location == "master");
    BOOST_CHECK(config[0].variables[1].location == "subproblems");
    BOOST_CHECK(config[0].variables[2].location == "master-and-subproblems");

    BOOST_REQUIRE_EQUAL(config[0].objectives.size(), 3);
    BOOST_CHECK(config[0].objectives[0].location == "master");
    BOOST_CHECK(config[0].objectives[1].location == "subproblems");
    BOOST_CHECK(config[0].objectives[2].location == "master-and-subproblems");
}

BOOST_AUTO_TEST_CASE(parse_missing_models_key_throws)
{
    std::string yaml_content = R"(
other_key:
  - id: model1
)";

    Parser parser;
    BOOST_CHECK_THROW(parser.parse(yaml_content), YAML::Exception);
}

BOOST_AUTO_TEST_CASE(parse_missing_id_throws)
{
    std::string yaml_content = R"(
models:
  - model-decomposition:
      variables: []
      objective-contributions: []
)";

    Parser parser;
    BOOST_CHECK_THROW(parser.parse(yaml_content), YAML::Exception);
}

BOOST_AUTO_TEST_CASE(parse_missing_model_decomposition_throws)
{
    std::string yaml_content = R"(
models:
  - id: model1
)";

    Parser parser;
    BOOST_CHECK_THROW(parser.parse(yaml_content), YAML::Exception);
}

BOOST_AUTO_TEST_CASE(parse_missing_variable_id_throws)
{
    std::string yaml_content = R"(
models:
  - id: model1
    model-decomposition:
      variables:
        - location: master
      objective-contributions: []
)";

    Parser parser;
    BOOST_CHECK_THROW(parser.parse(yaml_content), YAML::Exception);
}

BOOST_AUTO_TEST_CASE(parse_missing_variable_location_throws)
{
    std::string yaml_content = R"(
models:
  - id: model1
    model-decomposition:
      variables:
        - id: var1
      objective-contributions: []
)";

    Parser parser;
    BOOST_CHECK_THROW(parser.parse(yaml_content), YAML::Exception);
}

BOOST_AUTO_TEST_CASE(parse_missing_objective_id_throws)
{
    std::string yaml_content = R"(
models:
  - id: model1
    model-decomposition:
      variables: []
      objective-contributions:
        - location: master
)";

    Parser parser;
    BOOST_CHECK_THROW(parser.parse(yaml_content), YAML::Exception);
}

BOOST_AUTO_TEST_CASE(parse_missing_objective_location_throws)
{
    std::string yaml_content = R"(
models:
  - id: model1
    model-decomposition:
      variables: []
      objective-contributions:
        - id: obj1
)";

    Parser parser;
    BOOST_CHECK_THROW(parser.parse(yaml_content), YAML::Exception);
}

BOOST_AUTO_TEST_CASE(parse_invalid_yaml_syntax_throws)
{
    std::string yaml_content = R"(
models:
  - id: model1
    model-decomposition:
      variables:
        - id: var1
          location: master
      objective-contributions: [
)";

    Parser parser;
    BOOST_CHECK_THROW(parser.parse(yaml_content), YAML::Exception);
}

BOOST_AUTO_TEST_CASE(parse_empty_string_throws)
{
    std::string yaml_content = "";

    Parser parser;
    BOOST_CHECK_THROW(parser.parse(yaml_content), YAML::Exception);
}

BOOST_AUTO_TEST_CASE(parse_multiple_models_different_structures)
{
    std::string yaml_content = R"(
models:
  - id: model1
    model-decomposition:
      variables:
        - id: v1
          location: master
        - id: v2
          location: subproblems
      objective-contributions:
        - id: o1
          location: master
  - id: model2
    model-decomposition:
      variables:
        - id: v3
          location: master-and-subproblems
      objective-contributions:
        - id: o2
          location: subproblems
        - id: o3
          location: master
  - id: model3
    model-decomposition:
      variables: []
      objective-contributions: []
)";

    Parser parser;
    OptimConfig config = parser.parse(yaml_content);

    BOOST_REQUIRE_EQUAL(config.size(), 3);

    BOOST_CHECK_EQUAL(config[0].id, "model1");
    BOOST_CHECK_EQUAL(config[0].variables.size(), 2);
    BOOST_CHECK_EQUAL(config[0].objectives.size(), 1);

    BOOST_CHECK_EQUAL(config[1].id, "model2");
    BOOST_CHECK_EQUAL(config[1].variables.size(), 1);
    BOOST_CHECK_EQUAL(config[1].objectives.size(), 2);
    BOOST_CHECK_EQUAL(config[2].id, "model3");
    BOOST_CHECK_EQUAL(config[2].variables.size(), 0);
    BOOST_CHECK_EQUAL(config[2].objectives.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
