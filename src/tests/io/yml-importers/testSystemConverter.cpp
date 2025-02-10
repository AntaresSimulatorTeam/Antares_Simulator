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

#include <antares/io/inputs/yml-system/converter.h>
#include <antares/io/inputs/yml-system/parser.h>
#include <antares/study/system-model/system.h>
#include "antares/io/inputs/model-converter/modelConverter.h"
#include "antares/io/inputs/yml-model/parser.h"
#include "antares/study/system-model/library.h"

using namespace std::string_literals;
using namespace Antares::Solver;
using namespace Antares::Study;

struct LibraryObjects
{
    YmlModel::Model model1{.id = "node",
                           .description = "description",
                           .parameters = {{"cost", true, false}},
                           .variables = {},
                           .ports = {},
                           .port_field_definitions = {},
                           .constraints = {{"constraint1", "cost"}},
                           .objective = ""};

    YmlSystem::Parser parser;
    YmlModel::Library library;
    std::vector<SystemModel::Library> libraries;

    LibraryObjects()
    {
        library.id = "std";
        library.models = {model1};
        libraries = {ModelConverter::convert(library)};
    }

    ~LibraryObjects() = default;
};

BOOST_FIXTURE_TEST_CASE(full_model_system, LibraryObjects)
{
    const auto system = R"(
        system:
            id: base_system
            description: real application model
            model-libraries: [std]
            components:
                - id: N
                  model: std.node
                  scenario-group: group-234
                  parameters:
                    - id: cost
                      type: constant
                      value: 30
    )"s;

    YmlSystem::System systemObj = parser.parse(system);

    auto systemModel = SystemConverter::convert(systemObj, libraries);

    BOOST_CHECK_EQUAL(systemModel.Components().size(), 1);
    BOOST_CHECK_EQUAL(systemModel.Components().at("N").Id(), "N");
    BOOST_CHECK_EQUAL(systemModel.Components().at("N").getModel()->Id(), "node");
    BOOST_CHECK_EQUAL(systemModel.Components().at("N").getParameterValue("cost"), 30);
}

BOOST_FIXTURE_TEST_CASE(bad_param_name_in_component, LibraryObjects)
{
    const auto system = R"(
        system:
            id: base_system
            description: real application model
            model-libraries: [std]
            components:
                - id: N
                  model: std.node
                  scenario-group: group-234
                  parameters:
                    - id: param_not_in_model
                      type: constant
                      value: 30
    )"s;

    YmlSystem::System systemObj = parser.parse(system);

    BOOST_CHECK_THROW(SystemConverter::convert(systemObj, libraries), std::invalid_argument);
}

BOOST_FIXTURE_TEST_CASE(library_not_existing, LibraryObjects)
{
    const auto system = R"(
        system:
            id: base_system
            model-libraries: [abc]
            components:
                - id: N
                  model: abc.node
                  scenario-group: group-234
    )"s;

    YmlSystem::System systemObj = parser.parse(system);

    BOOST_CHECK_THROW(SystemConverter::convert(systemObj, libraries), std::runtime_error);
}

BOOST_FIXTURE_TEST_CASE(model_not_existing, LibraryObjects)
{
    const auto system = R"(
        system:
            id: base_system
            model-libraries: [std]
            components:
                - id: N
                  model: std.abc
                  scenario-group: group-234
    )"s;

    YmlSystem::System systemObj = parser.parse(system);

    BOOST_CHECK_THROW(SystemConverter::convert(systemObj, libraries), std::runtime_error);
}

BOOST_FIXTURE_TEST_CASE(bad_library_model_format, LibraryObjects)
{
    const auto system = R"(
        system:
            id: base_system
            model-libraries: [std]
            components:
                - id: N
                  model: std___node
                  scenario-group: group-234
                  parameters:
                    - id: cost
                      type: constant
                      value: 30
    )"s;

    YmlSystem::System systemObj = parser.parse(system);

    BOOST_CHECK_THROW(SystemConverter::convert(systemObj, libraries), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(Full_system_test)
{
    const auto libraryYaml = R"(
        library:
          id: std
          description: Standard library
          port-types: []

          models:
            - id: generator
              description: A basic generator model
              parameters:
                - id: cost
                  time-dependent: false
                  scenario-dependent: false
                - id: p_max
                  time-dependent: false
                  scenario-dependent: false
              variables:
                - id: generation
                  lower-bound: 0
                  upper-bound: p_max
              ports:
                - id: injection_port
                  type: flow
              port-field-definitions:
                - port: injection_port
                  field: flow
                  definition: generation
              objective: cost * generation

            - id: node
              description: A basic balancing node model
              ports:
                - id: injection_port
                  type: flow
              binding-constraints:
                - id: balance
                  expression: injection_port.flow = 0
    )"s;

    const auto libraryYaml2 = R"(
        library:
          id: mylib
          description: Extra library
          port-types: []

          models:
            - id: demand
              description: A basic fixed demand model
              parameters:
                - id: demand
                  time-dependent: true
                  scenario-dependent: true
              ports:
                - id: injection_port
                  type: flow
              port-field-definitions:
                - port: injection_port
                  field: flow
                  definition: -demand
    )"s;

    const auto systemYaml = R"(
        system:
          id: system1
          description: basic description
          model-libraries: [std, mylib]

          components:
            - id: N
              model: std.node
              scenario-group: group-234

            - id: G
              model: std.generator
              scenario-group: group-234
              parameters:
                - id: cost
                  type: constant
                  value: 30
                - id: p_max
                  type: constant
                  value: 100

            - id: D
              model: mylib.demand
              scenario-group: group-qsf
              parameters:
                - id: demand
                  type: constant
                  value: 100
    )"s;

    YmlModel::Parser parserModel;
    YmlSystem::Parser parserSystem;

    std::vector<SystemModel::Library> libraries;
    libraries.push_back(ModelConverter::convert(parserModel.parse(libraryYaml)));
    libraries.push_back(ModelConverter::convert(parserModel.parse(libraryYaml2)));

    YmlSystem::System systemObj = parserSystem.parse(systemYaml);
    auto systemModel = SystemConverter::convert(systemObj, libraries);

    BOOST_CHECK_EQUAL(systemModel.Components().size(), 3);
    BOOST_CHECK_EQUAL(systemModel.Components().at("N").Id(), "N");
    BOOST_CHECK_EQUAL(systemModel.Components().at("N").getModel()->Id(), "node");
    BOOST_CHECK_EQUAL(systemModel.Components().at("N").getScenarioGroupId(), "group-234");

    BOOST_CHECK_EQUAL(systemModel.Components().at("G").getModel()->Id(), "generator");
    BOOST_CHECK_EQUAL(systemModel.Components().at("G").getParameterValue("cost"), 30);
    BOOST_CHECK_EQUAL(systemModel.Components().at("G").getParameterValue("p_max"), 100);

    BOOST_CHECK_EQUAL(systemModel.Components().at("D").getModel()->Id(), "demand");
    BOOST_CHECK_EQUAL(systemModel.Components().at("D").getParameterValue("demand"), 100);
}
