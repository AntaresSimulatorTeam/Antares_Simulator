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

#include <boost/test/unit_test.hpp>

#include "antares/solver/libObjectModel/library.h"
#include "antares/solver/modelConverter/modelConverter.h"
#include "antares/solver/modelParser/model.h"

using namespace Antares::Solver;

// Test empty library
BOOST_AUTO_TEST_CASE(test_library_empty)
{
    ModelParser::Library library;
    ObjectModel::Library lib = ModelConverter::convert(library);
    BOOST_CHECK(lib.id().empty());
    BOOST_CHECK(lib.description().empty());
    BOOST_CHECK(lib.portTypes().empty());
    BOOST_CHECK(lib.models().empty());
}

// Test library with id and description
BOOST_AUTO_TEST_CASE(test_library_id_and_description)
{
    ModelParser::Library library;
    library.id = "test_id";
    library.description = "test_description";
    ObjectModel::Library lib = ModelConverter::convert(library);
    BOOST_CHECK_EQUAL(lib.id(), "test_id");
    BOOST_CHECK_EQUAL(lib.description(), "test_description");
}

// Test library with port types
BOOST_AUTO_TEST_CASE(test_library_port_types_empty_fields)
{
    ModelParser::Library library;
    ModelParser::PortType portType1{"port1", "flow port", {}};
    ModelParser::PortType portType2{"port2", "impedance port", {}};
    library.port_types = {portType1, portType2};
    ObjectModel::Library lib = ModelConverter::convert(library);
    BOOST_REQUIRE_EQUAL(lib.portTypes().size(), 2);
    BOOST_CHECK_EQUAL(lib.portTypes().at("port1").id(), "port1");
    BOOST_CHECK_EQUAL(lib.portTypes().at("port1").description(), "flow port");
    BOOST_CHECK(lib.portTypes().at("port1").fields().empty());
    BOOST_CHECK_EQUAL(lib.portTypes().at("port2").id(), "port2");
    BOOST_CHECK_EQUAL(lib.portTypes().at("port2").description(), "impedance port");
    BOOST_CHECK(lib.portTypes().at("port2").fields().empty());
}

// Test library with port types and fields
BOOST_AUTO_TEST_CASE(test_library_port_types_with_fields)
{
    ModelParser::Library library;
    ModelParser::PortType portType1{"port1", "flow port", {"field1", "field2"}};
    ModelParser::PortType portType2{"port2", "impedance port", {"field3", "field4"}};
    library.port_types = {portType1, portType2};
    ObjectModel::Library lib = ModelConverter::convert(library);
    BOOST_REQUIRE_EQUAL(lib.portTypes().at("port1").fields().size(), 2);
    BOOST_CHECK_EQUAL(lib.portTypes().at("port1").fields()[0].Name(), "field1");
    BOOST_CHECK_EQUAL(lib.portTypes().at("port1").fields()[1].Name(), "field2");
    BOOST_REQUIRE_EQUAL(lib.portTypes().at("port2").fields().size(), 2);
    BOOST_CHECK_EQUAL(lib.portTypes().at("port2").fields()[0].Name(), "field3");
    BOOST_CHECK_EQUAL(lib.portTypes().at("port2").fields()[1].Name(), "field4");
}

// Test library with models
BOOST_AUTO_TEST_CASE(test_library_empty_models)
{
    ModelParser::Library library;
    ModelParser::Model model1{"model1", "description", {}, {}, {}, {}, {}, "objectives"};
    library.models = {model1};
    ObjectModel::Library lib = ModelConverter::convert(library);
    BOOST_REQUIRE_EQUAL(lib.models().size(), 1);
    BOOST_CHECK_EQUAL(lib.models().at("model1").Id(), "model1");
    BOOST_CHECK_EQUAL(lib.models().at("model1").Objective().Value(), "objectives");
}

// Test library with models and parameters
BOOST_AUTO_TEST_CASE(test_library_models_with_parameters)
{
    ModelParser::Library library;
    ModelParser::Model model1{.id = "model1",
                              .description = "description",
                              .parameters = {{"param1", true, false}, {"param2", false, false}},
                              .variables = {},
                              .ports = {},
                              .port_field_definitions{},
                              .constraints{},
                              .objective = "objectives"};
    library.models = {model1};
    ObjectModel::Library lib = ModelConverter::convert(library);
    auto& model = lib.models().at("model1");
    BOOST_REQUIRE_EQUAL(model.Parameters().size(), 2);
    auto& parameter1 = model.Parameters().at("param1");
    auto& parameter2 = model.Parameters().at("param2");
    BOOST_CHECK_EQUAL(parameter1.Name(), "param1");
    BOOST_CHECK(parameter1.isTimeDependent());
    BOOST_CHECK(!parameter1.isScenarioDependent());
    BOOST_CHECK_EQUAL(parameter1.Type(), ObjectModel::ValueType::FLOAT);
    BOOST_CHECK_EQUAL(parameter2.Name(), "param2");
    BOOST_CHECK(!parameter2.isTimeDependent());
    BOOST_CHECK(!parameter2.isScenarioDependent());
    BOOST_CHECK_EQUAL(parameter2.Type(), ObjectModel::ValueType::FLOAT);
}

// Test library with models and variables
BOOST_AUTO_TEST_CASE(test_library_models_with_variables)
{
    ModelParser::Library library;
    ModelParser::Model model1{
      .id = "model1",
      .description = "description",
      .parameters = {},
      .variables = {{"var1", "7", "pmax", ModelParser::ValueType::BOOL},
                    {"var2", "99999999.9999999", "vcost", ModelParser::ValueType::INTEGER}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .objective = "objectives"};
    library.models = {model1};
    ObjectModel::Library lib = ModelConverter::convert(library);
    auto& model = lib.models().at("model1");
    BOOST_REQUIRE_EQUAL(model.Variables().size(), 2);
    auto& variable1 = model.Variables().at("var1");
    auto& variable2 = model.Variables().at("var2");
    BOOST_CHECK_EQUAL(variable1.Name(), "var1");
    BOOST_CHECK_EQUAL(variable1.LowerBound().Value(), "7");
    BOOST_CHECK_EQUAL(variable1.UpperBound().Value(), "pmax");
    BOOST_CHECK_EQUAL(variable1.Type(), ObjectModel::ValueType::BOOL);
    BOOST_CHECK_EQUAL(variable2.Name(), "var2");
    BOOST_CHECK_EQUAL(variable2.LowerBound().Value(), "99999999.9999999");
    BOOST_CHECK_EQUAL(variable2.UpperBound().Value(), "vcost");
    BOOST_CHECK_EQUAL(variable2.Type(), ObjectModel::ValueType::INTEGER);
}

// Test library with models and ports
BOOST_AUTO_TEST_CASE(test_library_models_with_ports, *boost::unit_test::disabled())
{
    ModelParser::Library library;
    ModelParser::Model model1{.id = "model1",
                              .description = "description",
                              .parameters = {},
                              .variables = {},
                              .ports = {{"port1", "flow"}, {"port2", "impedance"}},
                              .port_field_definitions = {},
                              .constraints = {},
                              .objective = "objectives"};
    library.models = {model1};
    ObjectModel::Library lib = ModelConverter::convert(library);
    auto& model = lib.models().at("model1");
    // BOOST_REQUIRE_EQUAL(model.Ports().size(), 2);
    // auto& port1 = model.Ports().at("port1");
    // auto& port2 = model.Ports().at("port2");
    // BOOST_CHECK_EQUAL(port1.Name(), "port1");
    //  BOOST_CHECK_EQUALS port1.Type()
    // BOOST_CHECK_EQUAL(port2.Name(), "port2");
    // BOOST_CHECK_EQUALS port2.Type()
}

// Test library with models and constraints
BOOST_AUTO_TEST_CASE(test_library_models_with_constraints)
{
    ModelParser::Library library;
    ModelParser::Model model1{.id = "model1",
                              .description = "description",
                              .parameters = {},
                              .variables = {},
                              .ports = {},
                              .port_field_definitions = {},
                              .constraints = {{"constraint1", "expression1"},
                                              {"constraint2", "expression2"}},
                              .objective = "objectives"};
    library.models = {model1};
    ObjectModel::Library lib = ModelConverter::convert(library);
    auto& model = lib.models().at("model1");
    BOOST_REQUIRE_EQUAL(model.getConstraints().size(), 2);
    auto& constraint1 = model.getConstraints().at("constraint1");
    auto& constraint2 = model.getConstraints().at("constraint2");
    BOOST_CHECK_EQUAL(constraint1.Name(), "constraint1");
    BOOST_CHECK_EQUAL(constraint1.expression().Value(), "expression1");
    BOOST_CHECK_EQUAL(constraint2.Name(), "constraint2");
    BOOST_CHECK_EQUAL(constraint2.expression().Value(), "expression2");
}

// Test with 2 models
BOOST_AUTO_TEST_CASE(test_library_two_models)
{
    ModelParser::Library library;
    ModelParser::Model model1{.id = "model1",
                              .description = "description",
                              .parameters = {{"param1", true, false}, {"param2", false, false}},
                              .variables = {{"varP", "7", "pmin", ModelParser::ValueType::FLOAT}},
                              .ports = {},
                              .port_field_definitions = {},
                              .constraints = {},
                              .objective = "objectives"};
    ModelParser::Model model2{
      .id = "model2",
      .description = "description",
      .parameters = {},
      .variables = {{"var1", "7", "pmax", ModelParser::ValueType::BOOL},
                    {"var2", "99999999.9999999", "vcost", ModelParser::ValueType::INTEGER}},
      .ports = {},
      .port_field_definitions = {},
      .constraints = {},
      .objective = "objectives"};
    library.models = {model1, model2};
    ObjectModel::Library lib = ModelConverter::convert(library);
    BOOST_REQUIRE_EQUAL(lib.models().size(), 2);
    auto& modelo1 = lib.models().at("model1");
    BOOST_REQUIRE_EQUAL(modelo1.Parameters().size(), 2);
    BOOST_REQUIRE_EQUAL(modelo1.Variables().size(), 1);
    auto& modelo2 = lib.models().at("model2");
    BOOST_REQUIRE_EQUAL(modelo2.Parameters().size(), 0);
    BOOST_REQUIRE_EQUAL(modelo2.Variables().size(), 2);
}
