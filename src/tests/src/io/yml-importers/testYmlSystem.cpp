// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/io/inputs/yml-system/parser.h>

using namespace std::string_literals;
using namespace Antares::IO::Inputs;

BOOST_AUTO_TEST_CASE(empty_system)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: ""
            description: ""
            model-libraries: []
            components: []
    )"s;
    YmlSystem::System systemObj = parser.parse(system);
    BOOST_CHECK(systemObj.id.empty());
    BOOST_CHECK(systemObj.libraries.empty());
    BOOST_CHECK(systemObj.components.empty());
}

BOOST_AUTO_TEST_CASE(simple_id)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            description: a basic system
    )"s;
    YmlSystem::System systemObj = parser.parse(system);
    BOOST_CHECK_EQUAL(systemObj.id, "base_system");
}

BOOST_AUTO_TEST_CASE(libraries_one_model)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            model-libraries: [abc]
    )"s;
    YmlSystem::System systemObj = parser.parse(system);
    BOOST_CHECK_EQUAL(systemObj.libraries[0], "abc");
}

BOOST_AUTO_TEST_CASE(libraries_list_of_models)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            description: 3 model libraries
            model-libraries: [abc, def, 123]
            components: []
    )"s;
    YmlSystem::System systemObj = parser.parse(system);
    BOOST_CHECK_EQUAL(systemObj.libraries[0], "abc");
    BOOST_CHECK_EQUAL(systemObj.libraries[1], "def");
    BOOST_CHECK_EQUAL(systemObj.libraries[2], "123");
    BOOST_CHECK(systemObj.components.empty());
}

BOOST_AUTO_TEST_CASE(one_component)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            description: one simple component
            components:
                - id: N
                  model: abcde
                  scenario-group: group-234
    )"s;
    YmlSystem::System systemObj = parser.parse(system);
    BOOST_CHECK_EQUAL(systemObj.components[0].id, "N");
    BOOST_CHECK_EQUAL(systemObj.components[0].model, "abcde");
    BOOST_CHECK_EQUAL(systemObj.components[0].scenarioGroup, "group-234");
}

BOOST_AUTO_TEST_CASE(two_components)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            description: two components
            components:
                - id: N
                  model: std.node
                  scenario-group: group-234
                - id: G
                  model: std.generator
                  scenario-group: group-thermal
    )"s;
    YmlSystem::System systemObj = parser.parse(system);
    BOOST_CHECK_EQUAL(systemObj.components[0].id, "N");
    BOOST_CHECK_EQUAL(systemObj.components[0].model, "std.node");
    BOOST_CHECK_EQUAL(systemObj.components[0].scenarioGroup, "group-234");
    BOOST_CHECK_EQUAL(systemObj.components[1].id, "G");
    BOOST_CHECK_EQUAL(systemObj.components[1].model, "std.generator");
    BOOST_CHECK_EQUAL(systemObj.components[1].scenarioGroup, "group-thermal");
}

BOOST_AUTO_TEST_CASE(component_parameter)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            description: one component with one parameter
            components:
                - id: N
                  model: std.node
                  scenario-group: group-234
                  parameters:
                    - id: cost
                      time-dependent: false
                      scenario-dependent: false
                      value: 30
    )"s;
    YmlSystem::System systemObj = parser.parse(system);
    const auto& param = systemObj.components[0].parameters[0];
    BOOST_CHECK_EQUAL(param.id, "cost");
    BOOST_CHECK_EQUAL(param.time_dependent, false);
    BOOST_CHECK_EQUAL(param.scenario_dependent, false);
    BOOST_CHECK_EQUAL(std::stod(param.value), 30);
}

BOOST_AUTO_TEST_CASE(component_two_parameters)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            description: one component with one parameter
            components:
                - id: N
                  model: std.node
                  scenario-group: group-234
                  parameters:
                    - id: cost
                      time-dependent: false
                      scenario-dependent: false
                      value: 30
                    - id: p_max
                      time-dependent: false
                      scenario-dependent: false
                      value: 100
    )"s;
    YmlSystem::System systemObj = parser.parse(system);
    const auto& param = systemObj.components[0].parameters[0];
    const auto& param2 = systemObj.components[0].parameters[1];
    BOOST_CHECK_EQUAL(param.id, "cost");
    BOOST_CHECK_EQUAL(param.time_dependent, false);
    BOOST_CHECK_EQUAL(param.scenario_dependent, false);
    BOOST_CHECK_EQUAL(std::stod(param.value), 30);
    BOOST_CHECK_EQUAL(param2.id, "p_max");
    BOOST_CHECK_EQUAL(param2.time_dependent, false);
    BOOST_CHECK_EQUAL(param2.scenario_dependent, false);
    BOOST_CHECK_EQUAL(std::stod(param2.value), 100);
}
