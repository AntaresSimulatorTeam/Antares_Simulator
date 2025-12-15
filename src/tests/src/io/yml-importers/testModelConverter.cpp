/* * Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include "antares/exception/RuntimeError.hpp"
#include "antares/io/inputs/model-converter/modelConverter.h"
#include "antares/io/inputs/yml-model/Library.h"
#include "antares/study/system-model/library.h"

#include "enum_operators.h"

using namespace Antares::IO::Inputs;
using namespace Antares::IO::Inputs::YmlModel;
using namespace Antares::ModelerStudy;
using namespace Antares::Modeler::Config;

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
    YmlModel::PortType portType1{"port1", "flow port", {"field1", "field2"}, ""};
    YmlModel::PortType portType2{"port2", "impedance port", {"field3", "field4"}, ""};
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
    YmlModel::PortType portType1{"port1", "empty port", {}, ""};
    library.port_types = {portType1};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library),
                          ModelConverter::PortTypeDoesntContainsFields,
                          emptyFields);

    // same name
    YmlModel::PortType portType2{"port2", "flow port", {"field1", "field2"}, ""};
    YmlModel::PortType portType3{"port2", "impedance port", {"field3", "field4"}, ""};
    library.port_types = {portType2, portType3};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library),
                          ModelConverter::PortTypeWithThisIdAlreadyExists,
                          portTypeAlreadyExists);
}

// Test library with models
BOOST_FIXTURE_TEST_CASE(model_with_one_objective_properly_translated, Fixture)
{
    YmlModel::Model model1{.id = "model1",
                           .description = "description",
                           .parameters = {{"param1", true, false}},
                           .variables = {},
                           .ports = {},
                           .port_field_definitions = {},
                           .constraints = {},
                           .binding_constraints = {},
                           .objectives = {{"objective-id", "param1", "subproblems"}},
                           .extra_outputs = {}};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    BOOST_REQUIRE_EQUAL(lib.Models().size(), 1);
    BOOST_CHECK_EQUAL(lib.Models().at("model1").Id(), "model1");
    BOOST_CHECK_EQUAL(lib.Models().at("model1").Objectives()[0].expression().Value(), "param1");
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
                           .objectives = {},
                           .extra_outputs = {}};
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
      .variables = {{"var1", "7", "pmax", ValueType::BOOL, true, true, "master"},
                    {"var2", "999.99", "var1", ValueType::INTEGER, true, true, "subproblems"}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {{"objective-id", "var1", "subproblems"}},
      .extra_outputs = {}};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    auto& model = lib.Models().at("model1");
    const auto& vars = model.Variables();
    BOOST_REQUIRE_EQUAL(vars.size(), 2);
    const auto var_1 = std::ranges::find_if(vars, [](const auto& v) { return v.Id() == "var1"; });
    const auto var_2 = std::ranges::find_if(vars, [](const auto& v) { return v.Id() == "var2"; });
    BOOST_CHECK(var_1 != vars.cend());
    BOOST_CHECK_EQUAL(var_1->LowerBound().Value(), "7");
    BOOST_CHECK_EQUAL(var_1->UpperBound().Value(), "pmax");
    BOOST_CHECK_EQUAL(var_1->Type(), SystemModel::ValueType::BOOL);
    BOOST_CHECK(var_1->location() == Location::MASTER);
    BOOST_CHECK(var_2 != vars.cend());
    BOOST_CHECK_EQUAL(var_2->LowerBound().Value(), "999.99");
    BOOST_CHECK_EQUAL(var_2->UpperBound().Value(), "var1");
    BOOST_CHECK_EQUAL(var_2->Type(), SystemModel::ValueType::INTEGER);
    BOOST_CHECK(var_2->location() == Location::SUBPROBLEMS);
}

// wrong variable ValueType
BOOST_FIXTURE_TEST_CASE(wrong_value_type, Fixture)
{
    YmlModel::Model model1{
      .id = "model1",
      .description = "description",
      .parameters = {{"param1", true, false}, {"param2", false, false}},
      .variables = {{"varP", "7", "param2", static_cast<ValueType>(5), true, true, "master"}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {},
      .extra_outputs = {}};
    library.models = {model1};
    BOOST_CHECK_THROW(ModelConverter::convert(library), std::runtime_error);
}

// Test library with models and ports
BOOST_FIXTURE_TEST_CASE(model_ports_properly_translated, Fixture)
{
    YmlModel::PortType portType1{"flow", "description", {"abc"}, ""};
    library.port_types = {portType1};

    YmlModel::Model model1{.id = "model1",
                           .description = "description",
                           .parameters = {},
                           .variables = {},
                           .ports = {{"port1", "flow"}, {"port2", "flow"}},
                           .port_field_definitions = {},
                           .constraints = {},
                           .binding_constraints = {},
                           .objectives = {},
                           .extra_outputs = {}};
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
    YmlModel::PortType portType1{"flow", "description", {"abc"}, ""};
    library.port_types = {portType1};

    // test port type not found
    YmlModel::Model model{.id = "model2",
                          .description = "description",
                          .parameters = {},
                          .variables = {},
                          .ports = {{"port2", "not flow"}},
                          .port_field_definitions = {},
                          .constraints = {},
                          .binding_constraints = {},
                          .objectives = {},
                          .extra_outputs = {}};
    library.models = {model};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library),
                          ModelConverter::PortTypeNotFound,
                          typeNotFound);
}

// Test library with models and constraints
BOOST_FIXTURE_TEST_CASE(model_constraints_properly_translated, Fixture)
{
    YmlModel::Model model1{
      .id = "model1",
      .description = "description",
      .parameters = {{"expression1", true, false},
                     {"expression2", true, false},
                     {"expression3", true, false}},
      .variables = {},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {{"constraint1", "expression1", "subproblems"},
                      {"constraint2", "expression2", "master-and-subproblems"}},
      .binding_constraints = {{"constraint3", "expression3", "master"}},
      .objectives = {},
      .extra_outputs = {}};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    auto& model = lib.Models().at("model1");
    BOOST_REQUIRE_EQUAL(model.Constraints().size(), 3);
    auto& constraint1 = model.Constraints().at(0);
    auto& constraint2 = model.Constraints().at(1);
    auto& constraint3 = model.Constraints().at(2);
    BOOST_CHECK_EQUAL(constraint1.Id(), "constraint1");
    BOOST_CHECK_EQUAL(constraint1.expression().Value(), "expression1");
    BOOST_CHECK_EQUAL(constraint2.Id(), "constraint2");
    BOOST_CHECK_EQUAL(constraint2.expression().Value(), "expression2");
    BOOST_CHECK_EQUAL(constraint3.Id(), "constraint3");
    BOOST_CHECK_EQUAL(constraint3.expression().Value(), "expression3");
    BOOST_CHECK(constraint1.location() == Location::SUBPROBLEMS);
    BOOST_CHECK(constraint2.location() == Location::MASTER_AND_SUBPROBLEMS);
    BOOST_CHECK(constraint3.location() == Location::MASTER);
}

bool constraintAlreadyExists(const ModelConverter::ConstraintWithThisIdAlreadyExists& ex)
{
    BOOST_CHECK_EQUAL(ex.what(), "Constraint with this id already exists: constraint1");
    return true;
}

// Test with 2 models
BOOST_FIXTURE_TEST_CASE(multiple_models_properly_translated, Fixture)
{
    YmlModel::Model ymlModel_1{.id = "model1",
                               .description = "description",
                               .parameters = {{"param1", true, false}, {"param2", false, false}},
                               .variables = {{"varP",
                                              "7",
                                              "param2",
                                              ValueType::CONTINUOUS,
                                              true,
                                              true,
                                              "master-and-subproblems"}},
                               .ports = {},
                               .port_field_definitions = {},
                               .constraints = {},
                               .binding_constraints = {},
                               .objectives = {},
                               .extra_outputs = {}};

    YmlModel::Model ymlModel_2{
      .id = "model2",
      .description = "description",
      .parameters = {},
      .variables = {{"var1", "7", "8", ValueType::BOOL, true, true, "master"},
                    {"var2", "9999.9", "var1", ValueType::INTEGER, true, true, "subproblems"}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {},
      .extra_outputs = {}};

    library.models = {ymlModel_1, ymlModel_2};

    SystemModel::Library lib = ModelConverter::convert(library);

    BOOST_REQUIRE_EQUAL(lib.Models().size(), 2);

    auto& model1 = lib.Models().at("model1");
    BOOST_REQUIRE_EQUAL(model1.Parameters().size(), 2);
    BOOST_REQUIRE_EQUAL(model1.Variables().size(), 1);

    auto& model2 = lib.Models().at("model2");
    BOOST_REQUIRE_EQUAL(model2.Parameters().size(), 0);
    BOOST_REQUIRE_EQUAL(model2.Variables().size(), 2);

    auto& var11 = model1.Variables()[0];
    auto& var21 = model2.Variables()[0];
    auto& var22 = model2.Variables()[1];
    BOOST_CHECK(var11.location() == Location::MASTER_AND_SUBPROBLEMS);
    BOOST_CHECK(var21.location() == Location::MASTER);
    BOOST_CHECK(var22.location() == Location::SUBPROBLEMS);
}

// Test library with ports field definitions
BOOST_FIXTURE_TEST_CASE(model_port_field_definitions_properly_translated, Fixture)
{
    YmlModel::PortType portType1{"flow", "description", {"field1"}, ""};
    library.port_types = {portType1};

    YmlModel::Model model1{.id = "model1",
                           .description = "description",
                           .parameters = {{"param1", true, false}},
                           .variables = {},
                           .ports = {{"port1", "flow"}},
                           .port_field_definitions = {{"port1", "field1", "param1"}},
                           .constraints = {},
                           .binding_constraints = {},
                           .objectives = {},
                           .extra_outputs = {}};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    auto& model = lib.Models().at("model1");
    auto& pfd1 = model.PortFieldDefinitions().at(
      SystemModel::PortFieldKey{.portId = "port1", .fieldId = "field1"});
    BOOST_CHECK_EQUAL(pfd1.port().Id(), "port1");
    BOOST_CHECK_EQUAL(pfd1.field().Id(), "field1");
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
    YmlModel::PortType portType1{"flow", "description", {"field1"}, ""};
    library.port_types = {portType1};

    YmlModel::Model model1{.id = "model1",
                           .description = "description",
                           .parameters = {{"param1", true, false}},
                           .variables = {},
                           .ports = {{"port1", "flow"}},
                           .port_field_definitions = {{"port2", "field1", "param1"}},
                           .constraints = {},
                           .binding_constraints = {},
                           .objectives = {},
                           .extra_outputs = {}};
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
                           .objectives = {},
                           .extra_outputs = {}};
    library.models = {model2};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library),
                          ModelConverter::FieldNotFoundForDefinition,
                          fieldNotFoundForDef);

    YmlModel::Model model3{.id = "model3",
                           .description = "description",
                           .parameters = {{"param3", true, false}},
                           .variables = {},
                           .ports = {{"port3", "flow"}, {"port4", "flow"}},
                           .port_field_definitions = {{"port4", "field1", "port3.field1"},
                                                      {"port3", "field1", ""}},
                           .constraints = {},
                           .binding_constraints = {},
                           .objectives = {},
                           .extra_outputs = {}};
    library.models = {model3};
    BOOST_CHECK_EXCEPTION(ModelConverter::convert(library),
                          ModelConverter::PortInDefinition,
                          portInDef);
}

// Test one model with extra outputs
BOOST_FIXTURE_TEST_CASE(model_extra_outputs_properly_translated, Fixture)
{
    YmlModel::Model model1{
      .id = "model1",
      .description = "description",
      .parameters = {{"param1", true, false}},
      .variables = {{"var1", "7", "8", ValueType::CONTINUOUS, true, true, "subproblems"}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .binding_constraints = {},
      .objectives = {},
      .extra_outputs = {{"output1", "5 * param1"}, {"output2", "param1 / var1 * 95.4"}}};

    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);

    auto& model = lib.Models().at("model1");
    BOOST_REQUIRE_EQUAL(model.ExtraOutputs().size(), 2);
    auto& output1 = model.ExtraOutputs().at("output1");
    auto& output2 = model.ExtraOutputs().at("output2");
    BOOST_CHECK_EQUAL(output1.Id(), "output1");
    BOOST_CHECK_EQUAL(output1.expression().Value(), "5 * param1");
    BOOST_CHECK_EQUAL(output2.Id(), "output2");
    BOOST_CHECK_EQUAL(output2.expression().Value(), "param1 / var1 * 95.4");
}
