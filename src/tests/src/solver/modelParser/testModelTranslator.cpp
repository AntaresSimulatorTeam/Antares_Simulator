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

#include "antares/solver/expressions/nodes/Node.h"
#include "antares/solver/modelConverter/modelConverter.h"
#include "antares/solver/modelParser/Library.h"
#include "antares/study/system-model/library.h"

#include "enum_operators.h"

using namespace Antares::Solver;
using namespace Antares::Study;

struct Fixture
{
    ModelParser::Library library;
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
BOOST_FIXTURE_TEST_CASE(port_type_with_empty_fileds_properly_translated, Fixture)
{
    ModelParser::PortType portType1{"port1", "flow port", {}};
    ModelParser::PortType portType2{"port2", "impedance port", {}};
    library.port_types = {portType1, portType2};
    SystemModel::Library lib = ModelConverter::convert(library);
    BOOST_REQUIRE_EQUAL(lib.PortTypes().size(), 2);
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port1").Id(), "port1");
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port1").Description(), "flow port");
    BOOST_CHECK(lib.PortTypes().at("port1").Fields().empty());
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port2").Id(), "port2");
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port2").Description(), "impedance port");
    BOOST_CHECK(lib.PortTypes().at("port2").Fields().empty());
    BOOST_REQUIRE_EQUAL(lib.PortTypes().size(), 2);
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port1").Id(), "port1");
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port1").Description(), "flow port");
    BOOST_CHECK(lib.PortTypes().at("port1").Fields().empty());
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port2").Id(), "port2");
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port2").Description(), "impedance port");
    BOOST_CHECK(lib.PortTypes().at("port2").Fields().empty());
}

// Test library with port types and fields
BOOST_FIXTURE_TEST_CASE(portType_with_fields_properly_translated, Fixture)
{
    ModelParser::PortType portType1{"port1", "flow port", {"field1", "field2"}};
    ModelParser::PortType portType2{"port2", "impedance port", {"field3", "field4"}};
    library.port_types = {portType1, portType2};
    SystemModel::Library lib = ModelConverter::convert(library);
    BOOST_REQUIRE_EQUAL(lib.PortTypes().at("port1").Fields().size(), 2);
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port1").Fields()[0].Id(), "field1");
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port1").Fields()[1].Id(), "field2");
    BOOST_REQUIRE_EQUAL(lib.PortTypes().at("port2").Fields().size(), 2);
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port2").Fields()[0].Id(), "field3");
    BOOST_CHECK_EQUAL(lib.PortTypes().at("port2").Fields()[1].Id(), "field4");
}

// Test library with models
BOOST_FIXTURE_TEST_CASE(empty_model_properly_translated, Fixture)
{
    ModelParser::Model model1{.id = "model1",
                              .description = "description",
                              .parameters = {{"param1", true, false}},
                              .variables = {},
                              .ports = {},
                              .port_field_definitions = {},
                              .constraints = {},
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
    ModelParser::Model model1{.id = "model1",
                              .description = "description",
                              .parameters = {{"param1", true, false}, {"param2", false, false}},
                              .variables = {},
                              .ports = {},
                              .port_field_definitions{},
                              .constraints{},
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
    ModelParser::Model model1{
      .id = "model1",
      .description = "description",
      .parameters = {{"pmax", true, false}},
      .variables = {{"var1", "7", "pmax", ModelParser::ValueType::BOOL},
                    {"var2", "99999999.9999999", "var1", ModelParser::ValueType::INTEGER}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
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

// Test library with models and ports
BOOST_AUTO_TEST_CASE(model_ports_properly_translated, *boost::unit_test::disabled())
{
    ModelParser::Library library;
    Registry<Nodes::Node> registry;
    ModelParser::Model model1{.id = "model1",
                              .description = "description",
                              .parameters = {},
                              .variables = {},
                              .ports = {{"port1", "flow"}, {"port2", "impedance"}},
                              .port_field_definitions = {},
                              .constraints = {},
                              .objective = ""};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    [[maybe_unused]] auto& model = lib.Models().at("model1");
    // BOOST_REQUIRE_EQUAL(model.Ports().size(), 2);
    // auto& port1 = model.Ports().at("port1");
    // auto& port2 = model.Ports().at("port2");
    // BOOST_CHECK_EQUAL(port1.Name(), "port1");
    //  BOOST_CHECK_EQUALS port1.Type()
    // BOOST_CHECK_EQUAL(port2.Name(), "port2");
    // BOOST_CHECK_EQUALS port2.Type()
}

// Test library with models and constraints
BOOST_FIXTURE_TEST_CASE(model_constraints_properly_translated, Fixture)
{
    ModelParser::Model model1{.id = "model1",
                              .description = "description",
                              .parameters = {{"expression1", true, false},
                                             {"expression2", true, false}},
                              .variables = {},
                              .ports = {},
                              .port_field_definitions = {},
                              .constraints = {{"constraint1", "expression1"},
                                              {"constraint2", "expression2"}},
                              .objective = ""};
    library.models = {model1};
    SystemModel::Library lib = ModelConverter::convert(library);
    auto& model = lib.Models().at("model1");
    BOOST_REQUIRE_EQUAL(model.getConstraints().size(), 2);
    auto& constraint1 = model.getConstraints().at("constraint1");
    auto& constraint2 = model.getConstraints().at("constraint2");
    BOOST_CHECK_EQUAL(constraint1.Id(), "constraint1");
    BOOST_CHECK_EQUAL(constraint1.expression().Value(), "expression1");
    BOOST_CHECK_EQUAL(constraint2.Id(), "constraint2");
    BOOST_CHECK_EQUAL(constraint2.expression().Value(), "expression2");
}

// Test with 2 models
BOOST_FIXTURE_TEST_CASE(multiple_models_properly_translated, Fixture)
{
    ModelParser::Model model1{
      .id = "model1",
      .description = "description",
      .parameters = {{"param1", true, false}, {"param2", false, false}},
      .variables = {{"varP", "7", "param2", ModelParser::ValueType::CONTINUOUS}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .objective = ""};
    ModelParser::Model model2{
      .id = "model2",
      .description = "description",
      .parameters = {},
      .variables = {{"var1", "7", "8", ModelParser::ValueType::BOOL},
                    {"var2", "99999999.9999999", "var1", ModelParser::ValueType::INTEGER}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
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
