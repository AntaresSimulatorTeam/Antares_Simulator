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

#define BOOST_TEST_MODULE modelParser

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/io/inputs/yml-model/parser.h"

#include "enum_operators.h"

using namespace std::string_literals;

// Test empty library
BOOST_AUTO_TEST_CASE(EmpyLibrary_is_valid)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: ""
            description: ""
            port-types: []
            models: []
    )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_CHECK(libraryObj.id.empty());
    BOOST_CHECK(libraryObj.description.empty());
    BOOST_CHECK(libraryObj.port_types.empty());
    BOOST_CHECK(libraryObj.models.empty());
}

// Test library with id and description
BOOST_AUTO_TEST_CASE(library_id_and_description_parsed_properly)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "test_id"
            description: "test_description"
            port-types: []
            models: []
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_CHECK_EQUAL(libraryObj.id, "test_id");
    BOOST_CHECK_EQUAL(libraryObj.description, "test_description");
}

// Test library with port types
BOOST_AUTO_TEST_CASE(port_types_properly_parsed)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types:
                - id: "porttype_id"
                  description: "porttype_description"
                  fields:
                      - id: "port_name"
            models: []
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_REQUIRE_EQUAL(libraryObj.port_types.size(), 1);
    BOOST_CHECK_EQUAL(libraryObj.port_types[0].id, "porttype_id");
    BOOST_CHECK_EQUAL(libraryObj.port_types[0].description, "porttype_description");
    BOOST_REQUIRE_EQUAL(libraryObj.port_types[0].fields.size(), 1);
    BOOST_CHECK_EQUAL(libraryObj.port_types[0].fields[0], "port_name");
}

// Test library with multiple port types
BOOST_AUTO_TEST_CASE(library_can_contain_multiple_port_types)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types:
                - id: "porttype_id1"
                  description: "porttype_description1"
                  fields:
                      - id: "port_name1"
                - id: "porttype_id2"
                  description: "porttype_description2"
                  fields:
                      - id: "port_name2"
            models: []
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_REQUIRE_EQUAL(libraryObj.port_types.size(), 2);
    BOOST_CHECK_EQUAL(libraryObj.port_types[0].id, "porttype_id1");
    BOOST_CHECK_EQUAL(libraryObj.port_types[0].description, "porttype_description1");
    BOOST_REQUIRE_EQUAL(libraryObj.port_types[0].fields.size(), 1);
    BOOST_CHECK_EQUAL(libraryObj.port_types[0].fields[0], "port_name1");
    BOOST_CHECK_EQUAL(libraryObj.port_types[1].id, "porttype_id2");
    BOOST_CHECK_EQUAL(libraryObj.port_types[1].description, "porttype_description2");
    BOOST_REQUIRE_EQUAL(libraryObj.port_types[1].fields.size(), 1);
    BOOST_CHECK_EQUAL(libraryObj.port_types[1].fields[0], "port_name2");
}

// Test library with models
BOOST_AUTO_TEST_CASE(models_properly_parsed)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const std::string library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types: []
            models:
                - id: "model_id"
                  description: "model_description"
                  parameters: []
                  variables: []
                  ports: []
                  port-field-definitions: []
                  constraints: []
                  objective: "objective"
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_REQUIRE_EQUAL(libraryObj.models.size(), 1);
    BOOST_CHECK_EQUAL(libraryObj.models[0].id, "model_id");
    BOOST_CHECK_EQUAL(libraryObj.models[0].description, "model_description");
    auto& model = libraryObj.models[0];
    BOOST_CHECK(model.parameters.empty());
    BOOST_CHECK(model.variables.empty());
    BOOST_CHECK(model.ports.empty());
    BOOST_CHECK(model.port_field_definitions.empty());
    BOOST_CHECK(model.constraints.empty());
    BOOST_CHECK_EQUAL(libraryObj.models[0].objective, "objective");
}

// Test library with multiple models
BOOST_AUTO_TEST_CASE(library_can_contain_multiple_models)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types: []
            models:
                - id: "model_id1"
                  description: "model_description1"
                  parameters: []
                  variables: []
                  ports: []
                  port-field-definitions: []
                  constraints: []
                  objective: "objective1"
                - id: "model_id2"
                  description: "model_description2"
                  parameters: []
                  variables: []
                  ports: []
                  port-field-definitions: []
                  constraints: []
                  objective: "objective2"
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_REQUIRE_EQUAL(libraryObj.models.size(), 2);
    BOOST_CHECK_EQUAL(libraryObj.models[0].id, "model_id1");
    BOOST_CHECK_EQUAL(libraryObj.models[0].description, "model_description1");
    BOOST_CHECK_EQUAL(libraryObj.models[0].objective, "objective1");
    BOOST_CHECK_EQUAL(libraryObj.models[1].id, "model_id2");
    BOOST_CHECK_EQUAL(libraryObj.models[1].description, "model_description2");
    BOOST_CHECK_EQUAL(libraryObj.models[1].objective, "objective2");
}

// Test library with one model containing parameters
BOOST_AUTO_TEST_CASE(parameters_properly_parsed)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types: []
            models:
                - id: "model_id"
                  description: "model_description"
                  parameters:
                      - id: "param_name"
                        time-dependent: FALSE
                        scenario-dependent: false
                  variables: []
                  ports: []
                  port-field-definitions: []
                  constraints: []
                  objective: "objective"
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_REQUIRE_EQUAL(libraryObj.models.size(), 1);
    BOOST_REQUIRE_EQUAL(libraryObj.models[0].parameters.size(), 1);
    BOOST_CHECK_EQUAL(libraryObj.models[0].parameters[0].id, "param_name");
    BOOST_CHECK_EQUAL(libraryObj.models[0].parameters[0].time_dependent, false);
    BOOST_CHECK_EQUAL(libraryObj.models[0].parameters[0].scenario_dependent, false);
}

// Test library with one model containing multiple parameters
BOOST_AUTO_TEST_CASE(model_can_contain_multiple_parameters)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types: []
            models:
                - id: "model_id"
                  description: "model_description"
                  parameters:
                      - id: "param_name1"
                        time-dependent: FALSE
                        scenario-dependent: false
                      - id: "param_name2"
                        time-dependent: true
                        scenario-dependent: true
                  variables: []
                  ports: []
                  port-field-definitions: []
                  constraints: []
                  objective: "objective"
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_REQUIRE_EQUAL(libraryObj.models.size(), 1);
    BOOST_REQUIRE_EQUAL(libraryObj.models[0].parameters.size(), 2);
    BOOST_CHECK_EQUAL(libraryObj.models[0].parameters[0].id, "param_name1");
    BOOST_CHECK_EQUAL(libraryObj.models[0].parameters[0].time_dependent, false);
    BOOST_CHECK_EQUAL(libraryObj.models[0].parameters[0].scenario_dependent, false);
    BOOST_CHECK_EQUAL(libraryObj.models[0].parameters[1].id, "param_name2");
    BOOST_CHECK_EQUAL(libraryObj.models[0].parameters[1].time_dependent, true);
    BOOST_CHECK_EQUAL(libraryObj.models[0].parameters[1].scenario_dependent, true);
}

// Time dependent and scenario dependant default value are true
BOOST_AUTO_TEST_CASE(test_library_model_parameters_default_values)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types: []
            models:
                - id: "model_id"
                  description: "model_description"
                  parameters:
                      - id: "param_name"
                  variables: []
                  ports: []
                  port-field-definitions: []
                  constraints: []
                  objective: "objective"
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_CHECK(libraryObj.models.size() == 1);
    BOOST_CHECK(libraryObj.models[0].parameters.size() == 1);
    BOOST_CHECK(libraryObj.models[0].parameters[0].id == "param_name");
    BOOST_CHECK(libraryObj.models[0].parameters[0].time_dependent == true);
    BOOST_CHECK(libraryObj.models[0].parameters[0].scenario_dependent == true);
}

// Test library with one model containing variables
BOOST_AUTO_TEST_CASE(variables_properly_parsed)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types: []
            models:
                - id: "model_id"
                  description: "model_description"
                  parameters: []
                  variables:
                      - id: "var_name"
                        lower-bound: 0
                        upper-bound: 1
                  ports: []
                  port-field-definitions: []
                  constraints: []
                  objective: "objective"
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_REQUIRE_EQUAL(libraryObj.models.size(), 1);
    BOOST_REQUIRE_EQUAL(libraryObj.models[0].variables.size(), 1);
    BOOST_CHECK_EQUAL(libraryObj.models[0].variables[0].id, "var_name");
    BOOST_CHECK_EQUAL(libraryObj.models[0].variables[0].lower_bound, "0");
    BOOST_CHECK_EQUAL(libraryObj.models[0].variables[0].upper_bound, "1");
}

// Test library with one model containing multiple variables
BOOST_AUTO_TEST_CASE(model_can_contain_multiple_variables)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types: []
            models:
                - id: "model_id"
                  description: "model_description"
                  parameters: []
                  variables:
                      - id: "var_name1"
                        lower-bound: 0
                        upper-bound: 1
                      - id: "var_name2"
                        lower-bound: -1
                        upper-bound: 2
                  ports: []
                  port-field-definitions: []
                  constraints: []
                  objective: "objective"
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_REQUIRE_EQUAL(libraryObj.models.size(), 1);
    BOOST_REQUIRE_EQUAL(libraryObj.models[0].variables.size(), 2);
    BOOST_CHECK_EQUAL(libraryObj.models[0].variables[0].id, "var_name1");
    BOOST_CHECK_EQUAL(libraryObj.models[0].variables[0].lower_bound, "0");
    BOOST_CHECK_EQUAL(libraryObj.models[0].variables[0].upper_bound, "1");
    BOOST_CHECK_EQUAL(libraryObj.models[0].variables[1].id, "var_name2");
    BOOST_CHECK_EQUAL(libraryObj.models[0].variables[1].lower_bound, "-1");
    BOOST_CHECK_EQUAL(libraryObj.models[0].variables[1].upper_bound, "2");
}

// variable bounds are strings expressions
BOOST_AUTO_TEST_CASE(variables_bounds_are_literals)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types: []
            models:
                - id: "model_id"
                  description: "model_description"
                  parameters: []
                  variables:
                      - id: "var_name"
                        lower-bound: "near-zero"
                        upper-bound: "pmax"
                  ports: []
                  port-field-definitions: []
                  constraints: []
                  objective: "objective"
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_CHECK_EQUAL(libraryObj.models[0].variables[0].id, "var_name");
    BOOST_CHECK_EQUAL(libraryObj.models[0].variables[0].lower_bound, "near-zero");
    BOOST_CHECK_EQUAL(libraryObj.models[0].variables[0].upper_bound, "pmax");
}

// variable variable-type
BOOST_AUTO_TEST_CASE(variable_types_can_be_integer_bool_float_default_to_float)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types: []
            models:
                - id: "model_id"
                  description: "model_description"
                  parameters: []
                  variables:
                    - id: "var1"
                      lower-bound: 0
                      upper-bound: 1
                      variable-type: "boolean"
                    - id: "var2"
                      lower-bound: 0
                      upper-bound: 1
                      variable-type: "integer"
                    - id: "var3"
                      lower-bound: 0
                      upper-bound: 1
                      variable-type: "continuous"
                    - id: "var4"
                      lower-bound: 0
                      upper-bound: 1
                  ports: []
                  port-field-definitions: []
                  constraints: []
                  objective: "objective"
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    auto& model = libraryObj.models[0];
    auto& var1 = model.variables[0];
    auto& var2 = model.variables[1];
    auto& var3 = model.variables[2];
    auto& var4 = model.variables[3];
    BOOST_CHECK_EQUAL(var1.variable_type, Antares::IO::Inputs::YmlModel::ValueType::BOOL);
    BOOST_CHECK_EQUAL(var2.variable_type, Antares::IO::Inputs::YmlModel::ValueType::INTEGER);
    BOOST_CHECK_EQUAL(var3.variable_type, Antares::IO::Inputs::YmlModel::ValueType::CONTINUOUS);
    BOOST_CHECK_EQUAL(var4.variable_type, Antares::IO::Inputs::YmlModel::ValueType::CONTINUOUS);
}

// Test library with one model containing ports
BOOST_AUTO_TEST_CASE(ports_are_properly_parsed)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types: []
            models:
                - id: "model_id"
                  description: "model_description"
                  parameters: []
                  variables: []
                  ports:
                      - id: "port_name"
                        type: "port_type"
                  port-field-definitions: []
                  constraints: []
                  objective: "objective"
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_REQUIRE_EQUAL(libraryObj.models.size(), 1);
    BOOST_REQUIRE_EQUAL(libraryObj.models[0].ports.size(), 1);
    BOOST_CHECK_EQUAL(libraryObj.models[0].ports[0].id, "port_name");
    BOOST_CHECK_EQUAL(libraryObj.models[0].ports[0].type, "port_type");
}

// Test library with one model containing multiple ports
BOOST_AUTO_TEST_CASE(model_can_conatin_multiple_ports)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types: []
            models:
                - id: "model_id"
                  description: "model_description"
                  parameters: []
                  variables: []
                  ports:
                      - id: "port_name1"
                        type: "port_type1"
                      - id: "port_name2"
                        type: "port_type2"
                  port-field-definitions: []
                  constraints: []
                  objective: "objective"
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_REQUIRE_EQUAL(libraryObj.models.size(), 1);
    BOOST_REQUIRE_EQUAL(libraryObj.models[0].ports.size(), 2);
    BOOST_CHECK_EQUAL(libraryObj.models[0].ports[0].id, "port_name1");
    BOOST_CHECK_EQUAL(libraryObj.models[0].ports[0].type, "port_type1");
    BOOST_CHECK_EQUAL(libraryObj.models[0].ports[1].id, "port_name2");
    BOOST_CHECK_EQUAL(libraryObj.models[0].ports[1].type, "port_type2");
}

// Test library with one model containing port field definitions
BOOST_AUTO_TEST_CASE(model_port_fileds_properly_parsed)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types: []
            models:
                - id: "model_id"
                  description: "model_description"
                  parameters: []
                  variables: []
                  ports: []
                  port-field-definitions:
                      - port: "port_name"
                        field: "field_name"
                        definition: "definition"
                  constraints: []
                  objective: "objective"
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_REQUIRE_EQUAL(libraryObj.models.size(), 1);
    BOOST_REQUIRE_EQUAL(libraryObj.models[0].port_field_definitions.size(), 1);
    BOOST_CHECK_EQUAL(libraryObj.models[0].port_field_definitions[0].port, "port_name");
    BOOST_CHECK_EQUAL(libraryObj.models[0].port_field_definitions[0].field, "field_name");
    BOOST_CHECK_EQUAL(libraryObj.models[0].port_field_definitions[0].definition, "definition");
}

// Test library with one model containing multiple port field definitions
BOOST_AUTO_TEST_CASE(model_can_contain_multiple_portfields)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types: []
            models:
                - id: "model_id"
                  description: "model_description"
                  parameters: []
                  variables: []
                  ports: []
                  port-field-definitions:
                      - port: "port_name1"
                        field: "field_name1"
                        definition: "definition1"
                      - port: "port_name2"
                        field: "field_name2"
                        definition: "definition2"
                  constraints: []
                  objective: "objective"
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_REQUIRE_EQUAL(libraryObj.models.size(), 1);
    BOOST_REQUIRE_EQUAL(libraryObj.models[0].port_field_definitions.size(), 2);
    BOOST_CHECK_EQUAL(libraryObj.models[0].port_field_definitions[0].port, "port_name1");
    BOOST_CHECK_EQUAL(libraryObj.models[0].port_field_definitions[0].field, "field_name1");
    BOOST_CHECK_EQUAL(libraryObj.models[0].port_field_definitions[0].definition, "definition1");
    BOOST_CHECK_EQUAL(libraryObj.models[0].port_field_definitions[1].port, "port_name2");
    BOOST_CHECK_EQUAL(libraryObj.models[0].port_field_definitions[1].field, "field_name2");
    BOOST_CHECK_EQUAL(libraryObj.models[0].port_field_definitions[1].definition, "definition2");
}

// Test library with one model containing constraints
BOOST_AUTO_TEST_CASE(constraints_properly_parsed)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types: []
            models:
                - id: "model_id"
                  description: "model_description"
                  parameters: []
                  variables: []
                  ports: []
                  port-field-definitions: []
                  constraints:
                      - id: "constraint_name"
                        expression: "expression"
                  objective: "objective"
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_REQUIRE_EQUAL(libraryObj.models.size(), 1);
    BOOST_REQUIRE_EQUAL(libraryObj.models[0].constraints.size(), 1);
    BOOST_CHECK_EQUAL(libraryObj.models[0].constraints[0].id, "constraint_name");
    BOOST_CHECK_EQUAL(libraryObj.models[0].constraints[0].expression, "expression");
}

// Test library with one model containing multiple constraints
BOOST_AUTO_TEST_CASE(model_can_contain_multiple_constraints)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types: []
            models:
                - id: "model_id"
                  description: "model_description"
                  parameters: []
                  variables: []
                  ports: []
                  port-field-definitions: []
                  constraints:
                      - id: "constraint_name1"
                        expression: "expression1"
                      - id: "constraint_name2"
                        expression: "expression2"
                  objective: "objective"
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_REQUIRE_EQUAL(libraryObj.models.size(), 1);
    BOOST_REQUIRE_EQUAL(libraryObj.models[0].constraints.size(), 2);
    BOOST_CHECK_EQUAL(libraryObj.models[0].constraints[0].id, "constraint_name1");
    BOOST_CHECK_EQUAL(libraryObj.models[0].constraints[0].expression, "expression1");
    BOOST_CHECK_EQUAL(libraryObj.models[0].constraints[1].id, "constraint_name2");
    BOOST_CHECK_EQUAL(libraryObj.models[0].constraints[1].expression, "expression2");
}

// Test error when model is not a map
BOOST_AUTO_TEST_CASE(model_is_not_scalar)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types: []
            models: "not a map"
        )"s;
    BOOST_CHECK_THROW(parser.parse(library), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(model_attributs_can_be_ommited)
{
    Antares::IO::Inputs::YmlModel::Parser parser;
    const auto library = R"(
        library:
            id: "lib_id"
            description: "lib_description"
            port-types: []
            models:
                - id: "model_id"
        )"s;
    Antares::IO::Inputs::YmlModel::Library libraryObj = parser.parse(library);
    BOOST_REQUIRE_EQUAL(libraryObj.models.size(), 1);
    BOOST_CHECK_EQUAL(libraryObj.models[0].id, "model_id");
    BOOST_CHECK_EQUAL(libraryObj.models[0].description, "");
    BOOST_CHECK(libraryObj.models[0].parameters.empty());
    BOOST_CHECK(libraryObj.models[0].variables.empty());
    BOOST_CHECK(libraryObj.models[0].ports.empty());
    BOOST_CHECK(libraryObj.models[0].port_field_definitions.empty());
    BOOST_CHECK(libraryObj.models[0].constraints.empty());
    BOOST_CHECK_EQUAL(libraryObj.models[0].objective, "");
}
