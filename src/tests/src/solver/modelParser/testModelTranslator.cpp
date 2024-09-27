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
#include "antares/solver/modelParser/model.h"

std::vector<Antares::Solver::ObjectModel::PortType> convertTypes(
  const Antares::Solver::ModelParser::Library& library)
{
    // Convert portTypes to Antares::Solver::ObjectModel::PortType
    std::vector<Antares::Solver::ObjectModel::PortType> out;
    for (const auto& portType: library.port_types)
    {
        std::vector<Antares::Solver::ObjectModel::PortField> fields;
        for (const auto& field: portType.fields)
        {
            fields.emplace_back(Antares::Solver::ObjectModel::PortField{field});
        }
        Antares::Solver::ObjectModel::PortType portTypeModel(portType.id,
                                                             portType.description,
                                                             std::move(fields));
        out.emplace_back(std::move(portTypeModel));
    }
    return out;
}

std::vector<Antares::Solver::ObjectModel::Model> convertModels(
  const Antares::Solver::ModelParser::Library& library)
{
    std::vector<Antares::Solver::ObjectModel::Model> models;
    for (const auto& model: library.models)
    {
        Antares::Solver::ObjectModel::ModelBuilder modelBuilder;
        std::vector<Antares::Solver::ObjectModel::Parameter> parameters;
        for (const auto& parameter: model.parameters)
        {
            parameters.emplace_back(Antares::Solver::ObjectModel::Parameter{
              parameter.name,
              Antares::Solver::ObjectModel::ValueType::FLOAT, // TODO: change to correct type
              static_cast<Antares::Solver::ObjectModel::Parameter::TimeDependent>(
                parameter.time_dependent),
              static_cast<Antares::Solver::ObjectModel::Parameter::ScenarioDependent>(
                parameter.scenario_dependent)});
        }
        auto modelObj = modelBuilder.withId(model.id)
                          .withObjective(Antares::Solver::ObjectModel::Expression{model.objective})
                          .withParameters(parameters)
                          .build();
        models.emplace_back(std::move(modelObj));
    }
    return models;
}

// Given a Antares::Solver::ModelParser::Library object, return a
// Antares::Solver::ObjectModel::Library object
Antares::Solver::ObjectModel::Library convert(const Antares::Solver::ModelParser::Library& library)
{
    Antares::Solver::ObjectModel::LibraryBuilder builder;
    std::vector<Antares::Solver::ObjectModel::PortType> portTypes = convertTypes(library);
    std::vector<Antares::Solver::ObjectModel::Model> models = convertModels(library);
    Antares::Solver::ObjectModel::Library lib = builder.withId(library.id)
                                                  .withDescription(library.description)
                                                  .withPortType(portTypes)
                                                  .withModel(models)
                                                  .build();
    return lib;
}

// Test empty library
BOOST_AUTO_TEST_CASE(test_library_empty)
{
    Antares::Solver::ModelParser::Library library;
    Antares::Solver::ObjectModel::Library lib = convert(library);
    BOOST_CHECK(lib.id().empty());
    BOOST_CHECK(lib.description().empty());
    BOOST_CHECK(lib.portTypes().empty());
    BOOST_CHECK(lib.models().empty());
}

// Test library with id and description
BOOST_AUTO_TEST_CASE(test_library_id_and_description)
{
    Antares::Solver::ModelParser::Library library;
    library.id = "test_id";
    library.description = "test_description";
    Antares::Solver::ObjectModel::Library lib = convert(library);
    BOOST_CHECK_EQUAL(lib.id(), "test_id");
    BOOST_CHECK_EQUAL(lib.description(), "test_description");
}

// Test library with port types
BOOST_AUTO_TEST_CASE(test_library_port_types_empty_fields)
{
    Antares::Solver::ModelParser::Library library;
    Antares::Solver::ModelParser::PortType portType1{"port1", "flow port", {}};
    Antares::Solver::ModelParser::PortType portType2{"port2", "impedance port", {}};
    library.port_types = {portType1, portType2};
    Antares::Solver::ObjectModel::Library lib = convert(library);
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
    Antares::Solver::ModelParser::Library library;
    Antares::Solver::ModelParser::PortType portType1{"port1", "flow port", {"field1", "field2"}};
    Antares::Solver::ModelParser::PortType portType2{"port2",
                                                     "impedance port",
                                                     {"field3", "field4"}};
    library.port_types = {portType1, portType2};
    Antares::Solver::ObjectModel::Library lib = convert(library);
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
    Antares::Solver::ModelParser::Library library;
    Antares::Solver::ModelParser::Model
      model1{"model1", "description", {}, {}, {}, {}, {}, "objectives"};
    library.models = {model1};
    Antares::Solver::ObjectModel::Library lib = convert(library);
    BOOST_REQUIRE_EQUAL(lib.models().size(), 1);
    BOOST_CHECK_EQUAL(lib.models().at("model1").Id(), "model1");
    BOOST_CHECK_EQUAL(lib.models().at("model1").Objective().Value(), "objectives");
}

// Test library with models and parameters
BOOST_AUTO_TEST_CASE(test_library_models_with_parameters)
{
    Antares::Solver::ModelParser::Library library;
    Antares::Solver::ModelParser::Model model1{"model1",
                                               "description",
                                               {{"param1", true, false}, {"param2", false, false}},
                                               {},
                                               {},
                                               {},
                                               {},
                                               "objectives"};
    library.models = {model1};
    Antares::Solver::ObjectModel::Library lib = convert(library);
    auto& model = lib.models().at("model1");
    BOOST_REQUIRE_EQUAL(model.Parameters().size(), 2);
    auto& parameter1 = model.Parameters().at("param1");
    auto& parameter2 = model.Parameters().at("param2");
    BOOST_CHECK_EQUAL(parameter1.Name(), "param1");
    BOOST_CHECK(parameter1.isTimeDependent());
    BOOST_CHECK(!parameter1.isScenarioDependent());
    BOOST_CHECK_EQUAL(parameter2.Name(), "param2");
    BOOST_CHECK(!parameter2.isTimeDependent());
    BOOST_CHECK(!parameter2.isScenarioDependent());
}
