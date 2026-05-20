// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/io/inputs/InputError.h>
#include <antares/io/inputs/yml-system/decoders.h>
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

// --------------------------------------------------------------------------
// Decoder error-path coverage: requireMap / requireSize / System::decode
// --------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(system_node_is_a_scalar_throws_input_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system: "not a map"
    )"s;
    BOOST_CHECK_THROW(parser.parse(system), InputError);
}

BOOST_AUTO_TEST_CASE(component_is_a_scalar_throws_input_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            components:
                - "not a map"
    )"s;
    BOOST_CHECK_THROW(parser.parse(system), InputError);
}

BOOST_AUTO_TEST_CASE(parameter_is_a_scalar_throws_input_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            components:
                - id: N
                  model: std.node
                  parameters:
                    - "not a map"
    )"s;
    BOOST_CHECK_THROW(parser.parse(system), InputError);
}

BOOST_AUTO_TEST_CASE(connection_is_a_scalar_throws_input_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            connections:
                - "not a map"
    )"s;
    BOOST_CHECK_THROW(parser.parse(system), InputError);
}

BOOST_AUTO_TEST_CASE(area_connection_is_a_scalar_throws_input_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            area-connections:
                - "not a map"
    )"s;
    BOOST_CHECK_THROW(parser.parse(system), InputError);
}

BOOST_AUTO_TEST_CASE(thermal_capacity_connection_is_a_scalar_throws_input_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            thermal-capacity-connections:
                - "not a map"
    )"s;
    BOOST_CHECK_THROW(parser.parse(system), InputError);
}

BOOST_AUTO_TEST_CASE(connection_with_missing_field_throws_input_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            connections:
                - component1: N
                  port1: p1
                  component2: M
    )"s;
    BOOST_CHECK_THROW(parser.parse(system), InputError);
}

BOOST_AUTO_TEST_CASE(connection_with_extra_field_throws_input_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            connections:
                - component1: N
                  port1: p1
                  component2: M
                  port2: p2
                  extra: bad
    )"s;
    BOOST_CHECK_THROW(parser.parse(system), InputError);
}

BOOST_AUTO_TEST_CASE(area_connection_with_wrong_field_count_throws_input_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            area-connections:
                - component: c1
                  port: p1
    )"s;
    BOOST_CHECK_THROW(parser.parse(system), InputError);
}

BOOST_AUTO_TEST_CASE(thermal_capacity_connection_with_wrong_field_count_throws_input_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            thermal-capacity-connections:
                - component: c1
                  port: p1
    )"s;
    BOOST_CHECK_THROW(parser.parse(system), InputError);
}

BOOST_AUTO_TEST_CASE(thermal_component_with_wrong_field_count_throws_input_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            thermal-capacity-connections:
                - component: c1
                  port: p1
                  thermal-component:
                    area: fr
    )"s;
    BOOST_CHECK_THROW(parser.parse(system), InputError);
}

BOOST_AUTO_TEST_CASE(thermal_component_null_uses_default_silently)
{
    // The thermal-component value is explicitly null: this exercises the
    // silent return-false path of requireMap and yields a default-constructed
    // ThermalComponent rather than an exception.
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            thermal-capacity-connections:
                - component: c1
                  port: p1
                  thermal-component: ~
    )"s;
    YmlSystem::System systemObj = parser.parse(system);
    BOOST_REQUIRE_EQUAL(systemObj.thermalCapacityConnections.size(), 1u);
    const auto& tcc = systemObj.thermalCapacityConnections[0];
    BOOST_CHECK_EQUAL(tcc.componentId, "c1");
    BOOST_CHECK_EQUAL(tcc.portId, "p1");
    BOOST_CHECK(tcc.thermalComponent.areaId.empty());
    BOOST_CHECK(tcc.thermalComponent.clusterId.empty());
}

// Direct unit tests against the decoder templates: they let us exercise the
// silent "absent/null node -> return false" branch of requireMap for decoder
// types that cannot reach it through public YAML input (yaml-cpp's vector
// decoder rethrows when an element decoder returns false, so this branch is
// otherwise unreachable for Parameter, Component, Connection, AreaConnection
// and ThermalCapacityConnection).
BOOST_AUTO_TEST_CASE(decoder_returns_false_for_null_or_undefined_nodes)
{
    YAML::Node undefined; // default-constructed: undefined / invalid
    YAML::Node nullNode = YAML::Load("~");

    {
        YmlSystem::Parameter p;
        BOOST_CHECK(!YAML::convert<YmlSystem::Parameter>::decode(undefined, p));
        BOOST_CHECK(!YAML::convert<YmlSystem::Parameter>::decode(nullNode, p));
    }
    {
        YmlSystem::Component c;
        BOOST_CHECK(!YAML::convert<YmlSystem::Component>::decode(undefined, c));
        BOOST_CHECK(!YAML::convert<YmlSystem::Component>::decode(nullNode, c));
    }
    {
        YmlSystem::Connection c;
        BOOST_CHECK(!YAML::convert<YmlSystem::Connection>::decode(undefined, c));
        BOOST_CHECK(!YAML::convert<YmlSystem::Connection>::decode(nullNode, c));
    }
    {
        YmlSystem::AreaConnection ac;
        BOOST_CHECK(!YAML::convert<YmlSystem::AreaConnection>::decode(undefined, ac));
        BOOST_CHECK(!YAML::convert<YmlSystem::AreaConnection>::decode(nullNode, ac));
    }
    {
        YmlSystem::ThermalComponent tc;
        BOOST_CHECK(!YAML::convert<YmlSystem::ThermalComponent>::decode(undefined, tc));
        BOOST_CHECK(!YAML::convert<YmlSystem::ThermalComponent>::decode(nullNode, tc));
    }
    {
        YmlSystem::ThermalCapacityConnection tcc;
        BOOST_CHECK(!YAML::convert<YmlSystem::ThermalCapacityConnection>::decode(undefined, tcc));
        BOOST_CHECK(!YAML::convert<YmlSystem::ThermalCapacityConnection>::decode(nullNode, tcc));
    }
}

BOOST_AUTO_TEST_CASE(decoder_throws_for_defined_non_map_nodes)
{
    YAML::Node scalar = YAML::Load("\"some-scalar\"");

    {
        YmlSystem::Parameter p;
        BOOST_CHECK_THROW(YAML::convert<YmlSystem::Parameter>::decode(scalar, p), InputError);
    }
    {
        YmlSystem::Component c;
        BOOST_CHECK_THROW(YAML::convert<YmlSystem::Component>::decode(scalar, c), InputError);
    }
    {
        YmlSystem::Connection c;
        BOOST_CHECK_THROW(YAML::convert<YmlSystem::Connection>::decode(scalar, c), InputError);
    }
    {
        YmlSystem::AreaConnection ac;
        BOOST_CHECK_THROW(YAML::convert<YmlSystem::AreaConnection>::decode(scalar, ac), InputError);
    }
    {
        YmlSystem::ThermalComponent tc;
        BOOST_CHECK_THROW(YAML::convert<YmlSystem::ThermalComponent>::decode(scalar, tc),
                          InputError);
    }
    {
        YmlSystem::ThermalCapacityConnection tcc;
        BOOST_CHECK_THROW(YAML::convert<YmlSystem::ThermalCapacityConnection>::decode(scalar, tcc),
                          InputError);
    }
    {
        YmlSystem::System s;
        BOOST_CHECK_THROW(YAML::convert<YmlSystem::System>::decode(scalar, s), InputError);
    }
}
