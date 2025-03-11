/* * Copyright 2007-2024, RTE (https://www.rte-france.com)
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

#include <iostream>

#include <boost/test/unit_test.hpp>

#include "antares/expressions/nodes/Node.h"
#include "antares/io/inputs/model-converter/modelConverter.h"
#include "antares/io/inputs/yml-model/Library.h"
#include "antares/study/system-model/library.h"

#include "enum_operators.h"

using namespace Antares::IO::Inputs;
using namespace Antares::Study;

struct Fixture
{
    YmlModel::Library library;
};

// Test empty library
BOOST_FIXTURE_TEST_CASE(Empty_library_is_valid, Fixture)
{
    SystemModel::Library lib = ModelConverter::convert(library);
    BOOST_CHECK(lib.Id().empty());
    BOOST_CHECK(lib.Description().empty());
    BOOST_CHECK(lib.PortTypes().empty());
    BOOST_CHECK(lib.Models().empty());
}

// Test library with id and description
BOOST_FIXTURE_TEST_CASE(library_id_description_properly_translated, Fixture)

{
    library.id = "test_id";
    library.description = "test_description";
    SystemModel::Library lib = ModelConverter::convert(library);
    BOOST_CHECK_EQUAL(lib.Id(), "test_id");
    BOOST_CHECK_EQUAL(lib.Description(), "test_description");
}

// Test library with port types
BOOST_FIXTURE_TEST_CASE(port_type_with_empty_fields_exception, Fixture)
{
}

// Test library with port types and fields
BOOST_FIXTURE_TEST_CASE(portType_with_fields_properly_translated, Fixture)
{
    YmlModel::PortType portType1{"port1", "flow port", {"field1", "field2"}};
    YmlModel::PortType portType2{"port2", "impedance port", {"field3", "field4"}};
    library.port_types = {portType1, portType2};
    SystemModel::Library lib = ModelConverter::convert(library);
    BOOST_REQUIRE_EQUAL(lib.PortTypes().at("port1").Fields().size(), 2);
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port1").Fields()[0].Id(), "field1");
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port1").Fields()[1].Id(), "field2");
    BOOST_REQUIRE_EQUAL(lib.PortTypes().at("port2").Fields().size(), 2);
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port2").Fields()[0].Id(), "field3");
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port2").Fields()[1].Id(), "field4");
}

bool emptyFields(const ModelConverter::PortTypeDoesntContainsFields& ex)
{
    BOOST_CHECK_EQUAL(ex.what(), "This port type doesn't contains fields: port1");
    return true;
}

bool portTypeAlreadyExists(const ModelConverter::PortTypeWithThisIdAlreadyExists& ex)
{
    BOOST_CHECK_EQUAL(ex.what(), "Port type with this id already exists: port2");
    return true;
}

// Test port types for exceptions
BOOST_FIXTURE_TEST_CASE(port_type_error_cases, Fixture)
{
    YmlModel::PortType portType1{"port1", "empty port", {}};
    library.port_types = {portType1};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library),
                          ModelConverter::PortTypeDoesntContainsFields,
                          emptyFields);

    // same name
    YmlModel::PortType portType2{"port2", "flow port", {"field1", "field2"}};
    YmlModel::PortType portType3{"port2", "impedance port", {"field3", "field4"}};
    library.port_types = {portType2, portType3};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library),
                          ModelConverter::PortTypeWithThisIdAlreadyExists,
                          portTypeAlreadyExists);
}

// Test library with models
BOOST_FIXTURE_TEST_CASE(empty_model_properly_translated, Fixture)
{
    YmlModel::Model model1{.id = "model1",
                           .description = "description",
                           .parameters = {{"param1", true, false}},
                           .variables = {},
                           .ports = {},
                           .port_field_definitions = {},
                           .constraints = {},
                           .binding_constraints = {},
                           .objective = "param1"};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    BOOST_REQUIRE_EQUAL(lib.Models().size(), 1);
    BOOST_CHECK_EQUAL(lib.Models().at("model1").Id(), "model1");
    BOOST_CHECK_EQUAL(lib.Models().at("model1").Objective().Value(), "param1");
}

// Test library with models and parameters
BOOST_FIXTURE_TEST_CASE(model_parameters_properly_translated, Fixture)
{
    YmlModel::Model model1{.id = "model1",
                           .description = "description",
                           .parameters = {{"param1", true, false}, {"param2", false, false}},
                           .variables = {},
                           .ports = {},
                           .port_field_definitions{},
                           .constraints{},
                           .binding_constraints = {},
                           .objective = ""};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    auto& model = lib.Models().at("model1");
    BOOST_REQUIRE_EQUAL(model.Parameters().size(), 2);
    auto& parameter1 = model.Parameters().at("param1");
    auto& parameter2 = model.Parameters().at("param2");
    BOOST_CHECK_EQUAL(parameter1.Id(), "param1");
    BOOST_CHECK(parameter1.isTimeDependent());
    BOOST_CHECK(!parameter1.isScenarioDependent());
    BOOST_CHECK_EQUAL(parameter2.Id(), "param2");
    BOOST_CHECK(!parameter2.isTimeDependent());
    BOOST_CHECK(!parameter2.isScenarioDependent());
}

// Test library with models and variables
BOOST_FIXTURE_TEST_CASE(model_variables_properly_translated, Fixture)
{
    YmlModel::Model model1{
      .id = "model1",
      .description = "description",
      .parameters = {{"pmax", true, false}},
      .variables = {{"var1", "7", "pmax", YmlModel::ValueType::BOOL, true, true},
                    {"var2", "99999999.9999999", "var1", YmlModel::ValueType::INTEGER, true, true}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objective = "var1"};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    auto& model = lib.Models().at("model1");
    BOOST_REQUIRE_EQUAL(model.Variables().size(), 2);
    auto& variable1 = model.Variables().at("var1");
    auto& variable2 = model.Variables().at("var2");
    BOOST_CHECK_EQUAL(variable1.Id(), "var1");
    BOOST_CHECK_EQUAL(variable1.LowerBound().Value(), "7");
    BOOST_CHECK_EQUAL(variable1.UpperBound().Value(), "pmax");
    BOOST_CHECK_EQUAL(variable1.Type(), SystemModel::ValueType::BOOL);
    BOOST_CHECK_EQUAL(variable2.Id(), "var2");
    BOOST_CHECK_EQUAL(variable2.LowerBound().Value(), "99999999.9999999");
    BOOST_CHECK_EQUAL(variable2.UpperBound().Value(), "var1");
    BOOST_CHECK_EQUAL(variable2.Type(), SystemModel::ValueType::INTEGER);
}

// wrong variable ValueType
BOOST_FIXTURE_TEST_CASE(wrong_value_type, Fixture)
{
    YmlModel::Model model1{
      .id = "model1",
      .description = "description",
      .parameters = {{"param1", true, false}, {"param2", false, false}},
      .variables = {{"varP", "7", "param2", static_cast<YmlModel::ValueType>(5), true, true}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objective = ""};
    library.models = {model1};
    BOOST_CHECK_THROW(ModelConverter::convert(library), std::runtime_error);
}

// Test library with models and ports
BOOST_FIXTURE_TEST_CASE(model_ports_properly_translated, Fixture)
{
    YmlModel::PortType portType1{"flow", "description", {"abc"}};
    library.port_types = {portType1};

    YmlModel::Model model1{.id = "model1",
                           .description = "description",
                           .parameters = {},
                           .variables = {},
                           .ports = {{"port1", "flow"}, {"port2", "flow"}},
                           .port_field_definitions = {},
                           .constraints = {},
                           .binding_constraints = {},
                           .objective = ""};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    [[maybe_unused]] auto& model = lib.Models().at("model1");
    BOOST_REQUIRE_EQUAL(model.Ports().size(), 2);
    auto& port1 = model.Ports().at("port1");
    auto& port2 = model.Ports().at("port2");
    BOOST_CHECK_EQUAL(port1.Id(), "port1");
    BOOST_CHECK_EQUAL(port1.Type().Id(), lib.PortTypes().at(port1.Type().Id()).Id());
    BOOST_CHECK_EQUAL(port2.Id(), "port2");
}

bool portAlreadyExists(const ModelConverter::PortWithThisIdAlreadyExists& ex)
{
    BOOST_CHECK_EQUAL(ex.what(), "Port with this id already exists: port1");
    return true;
}

bool typeNotFound(const ModelConverter::PortTypeNotFound& ex)
{
    BOOST_CHECK_EQUAL(ex.what(), "For the port: port2 , port type not found: not flow");
    return true;
}

// Test ports errors
BOOST_FIXTURE_TEST_CASE(ports_errors_cases, Fixture)
{
    // test port already exists
    YmlModel::PortType portType1{"flow", "description", {"abc"}};
    library.port_types = {portType1};

    YmlModel::Model model1{.id = "model1",
                           .description = "description",
                           .parameters = {},
                           .variables = {},
                           .ports = {{"port1", "flow"}, {"port1", "flow"}},
                           .port_field_definitions = {},
                           .constraints = {},
                           .binding_constraints = {},
                           .objective = ""};
    library.models = {model1};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library),
                          ModelConverter::PortWithThisIdAlreadyExists,
                          portAlreadyExists);

    // test port type not found
    YmlModel::Model model2{.id = "model2",
                           .description = "description",
                           .parameters = {},
                           .variables = {},
                           .ports = {{"port2", "not flow"}},
                           .port_field_definitions = {},
                           .constraints = {},
                           .binding_constraints = {},
                           .objective = ""};
    library.models = {model2};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library),
                          ModelConverter::PortTypeNotFound,
                          typeNotFound);
}

// Test library with models and constraints
BOOST_FIXTURE_TEST_CASE(model_constraints_properly_translated, Fixture)
{
    YmlModel::Model model1{.id = "model1",
                           .description = "description",
                           .parameters = {{"expression1", true, false},
                                          {"expression2", true, false},
                                          {"expression3", true, false}},
                           .variables = {},
                           .ports = {},
                           .port_field_definitions = {},
                           .constraints = {{"constraint1", "expression1"},
                                           {"constraint2", "expression2"}},
                           .binding_constraints = {{"constraint3", "expression3"}},
                           .objective = ""};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    auto& model = lib.Models().at("model1");
    BOOST_REQUIRE_EQUAL(model.getConstraints().size(), 3);
    auto& constraint1 = model.getConstraints().at("constraint1");
    auto& constraint2 = model.getConstraints().at("constraint2");
    auto& constraint3 = model.getConstraints().at("constraint3");
    BOOST_CHECK_EQUAL(constraint1.Id(), "constraint1");
    BOOST_CHECK_EQUAL(constraint1.expression().Value(), "expression1");
    BOOST_CHECK_EQUAL(constraint2.Id(), "constraint2");
    BOOST_CHECK_EQUAL(constraint2.expression().Value(), "expression2");
    BOOST_CHECK_EQUAL(constraint3.Id(), "constraint3");
    BOOST_CHECK_EQUAL(constraint3.expression().Value(), "expression3");
}

bool constraintAlreadyExists(const ModelConverter::ConstraintWithThisIdAlreadyExists& ex)
{
    BOOST_CHECK_EQUAL(ex.what(), "Constraint with this id already exists: constraint1");
    return true;
}

// Test constraints errors
BOOST_FIXTURE_TEST_CASE(constraints_error_cases, Fixture)
{
    YmlModel::Model model1{.id = "model1",
                           .description = "description",
                           .parameters = {{"expression1", true, false},
                                          {"expression2", true, false}},
                           .variables = {},
                           .ports = {},
                           .port_field_definitions = {},
                           .constraints = {{"constraint1", "expression1"},
                                           {"constraint1", "expression2"}},
                           .binding_constraints = {},
                           .objective = ""};
    library.models = {model1};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library),
                          ModelConverter::ConstraintWithThisIdAlreadyExists,
                          constraintAlreadyExists);
}

// Test with 2 models
BOOST_FIXTURE_TEST_CASE(multiple_models_properly_translated, Fixture)
{
    YmlModel::Model model1{
      .id = "model1",
      .description = "description",
      .parameters = {{"param1", true, false}, {"param2", false, false}},
      .variables = {{"varP", "7", "param2", YmlModel::ValueType::CONTINUOUS, true, true}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objective = ""};
    YmlModel::Model model2{
      .id = "model2",
      .description = "description",
      .parameters = {},
      .variables = {{"var1", "7", "8", YmlModel::ValueType::BOOL, true, true},
                    {"var2", "99999999.9999999", "var1", YmlModel::ValueType::INTEGER, true, true}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objective = ""};
    library.models = {model1, model2};
    SystemModel::Library lib = ModelConverter::convert(library);
    BOOST_REQUIRE_EQUAL(lib.Models().size(), 2);
    auto& modelo1 = lib.Models().at("model1");
    BOOST_REQUIRE_EQUAL(modelo1.Parameters().size(), 2);
    BOOST_REQUIRE_EQUAL(modelo1.Variables().size(), 1);
    auto& modelo2 = lib.Models().at("model2");
    BOOST_REQUIRE_EQUAL(modelo2.Parameters().size(), 0);
    BOOST_REQUIRE_EQUAL(modelo2.Variables().size(), 2);
}

// Test library with ports field definitions
BOOST_FIXTURE_TEST_CASE(model_port_field_definitions_properly_translated, Fixture)
{
    YmlModel::PortType portType1{"flow", "description", {"field1"}};
    library.port_types = {portType1};

    YmlModel::Model model1{.id = "model1",
                           .description = "description",
                           .parameters = {{"param1", true, false}},
                           .variables = {},
                           .ports = {{"port1", "flow"}},
                           .port_field_definitions = {{"port1", "field1", "param1"}},
                           .constraints = {},
                           .binding_constraints = {},
                           .objective = ""};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    auto& model = lib.Models().at("model1");
    auto& pfd1 = model.PortFieldDefinitions().at("port1");
    BOOST_CHECK_EQUAL(pfd1.getPort().Id(), "port1");
    BOOST_CHECK_EQUAL(pfd1.Field().Id(), "field1");
    BOOST_CHECK_EQUAL(pfd1.Definition().Value(), "param1");
}

bool portNotFoundForDef(const ModelConverter::PortNotFoundForDefinition& ex)
{
    BOOST_CHECK_EQUAL(ex.what(), "In port-field-definitions, port not found: port2");
    return true;
}

bool fieldNotFoundForDef(const ModelConverter::FieldNotFoundForDefinition& ex)
{
    BOOST_CHECK_EQUAL(ex.what(),
                      "In port-field-definitions, for port: port2 , field not found: field2");
    return true;
}

bool portInDef(const ModelConverter::PortInDefinition& ex)
{
    BOOST_CHECK_EQUAL(
      ex.what(),
      "In port-field-definitions, for port: port4 , found another port in the definition: port3");
    return true;
}

// Test library ports field definitions errors
BOOST_FIXTURE_TEST_CASE(port_field_definition_error_cases, Fixture)
{
    YmlModel::PortType portType1{"flow", "description", {"field1"}};
    library.port_types = {portType1};

    YmlModel::Model model1{.id = "model1",
                           .description = "description",
                           .parameters = {{"param1", true, false}},
                           .variables = {},
                           .ports = {{"port1", "flow"}},
                           .port_field_definitions = {{"port2", "field1", "param1"}},
                           .constraints = {},
                           .binding_constraints = {},
                           .objective = ""};
    library.models = {model1};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library),
                          ModelConverter::PortNotFoundForDefinition,
                          portNotFoundForDef);

    YmlModel::Model model2{.id = "model2",
                           .description = "description",
                           .parameters = {{"param2", true, false}},
                           .variables = {},
                           .ports = {{"port2", "flow"}},
                           .port_field_definitions = {{"port2", "field2", "param2"}},
                           .constraints = {},
                           .binding_constraints = {},
                           .objective = ""};
    library.models = {model2};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library),
                          ModelConverter::FieldNotFoundForDefinition,
                          fieldNotFoundForDef);

    YmlModel::Model model3{.id = "model3",
                           .description = "description",
                           .parameters = {{"param3", true, false}},
                           .variables = {},
                           .ports = {{"port3", "flow"}, {"port4", "flow"}},
                           .port_field_definitions = {{"port3", "field1", ""},
                                                      {"port4", "field1", "port3.field1"}},
                           .constraints = {},
                           .binding_constraints = {},
                           .objective = ""};
    library.models = {model3};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library),
                          ModelConverter::PortInDefinition,
                          portInDef);
}
