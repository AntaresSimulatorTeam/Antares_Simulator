// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <unit_test_utils.h>

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
            components: []
    )"s;
    YmlSystem::System systemObj = parser.parse(system, "");
    BOOST_CHECK(systemObj.id.empty());
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
    YmlSystem::System systemObj = parser.parse(system, "");
    BOOST_CHECK_EQUAL(systemObj.id, "base_system");
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
    YmlSystem::System systemObj = parser.parse(system, "");
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
    YmlSystem::System systemObj = parser.parse(system, "");
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
    YmlSystem::System systemObj = parser.parse(system, "");
    const auto& param = systemObj.components[0].parameters[0];
    BOOST_CHECK_EQUAL(param.id, "cost");
    BOOST_CHECK_EQUAL(param.time_dependent, false);
    BOOST_CHECK_EQUAL(param.scenario_dependent, false);
    BOOST_CHECK_EQUAL(std::stod(param.value), 30);
}

BOOST_AUTO_TEST_CASE(component_parameter_flags_default_to_false_when_missing)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            components:
                - id: N
                  model: std.node
                  parameters:
                    - id: cost
                      value: 30
    )"s;
    const YmlSystem::System systemObj = parser.parse(system, "");
    const auto& param = systemObj.components[0].parameters[0];
    BOOST_CHECK_EQUAL(param.time_dependent, false);
    BOOST_CHECK_EQUAL(param.scenario_dependent, false);
}

BOOST_AUTO_TEST_CASE(invalid_time_dependent_flag_throws_conversion_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            components:
                - id: N
                  model: std.node
                  parameters:
                    - id: cost
                      time-dependent: treu
                      value: 30
    )"s;
    BOOST_CHECK_THROW(parser.parse(system, ""), YAML::BadConversion);
}

BOOST_AUTO_TEST_CASE(invalid_scenario_dependent_flag_throws_conversion_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            components:
                - id: N
                  model: std.node
                  parameters:
                    - id: cost
                      scenario-dependent: [true]
                      value: 30
    )"s;
    BOOST_CHECK_THROW(parser.parse(system, ""), YAML::BadConversion);
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
                  properties:
                    - id: prop1
                      value: 1
                    - id: prop2
                      value: 2
    )"s;
    YmlSystem::System systemObj = parser.parse(system, "");
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

    BOOST_CHECK_EQUAL(systemObj.components[0].properties[0].id, "prop1");
    BOOST_CHECK_EQUAL(systemObj.components[0].properties[0].value, "1");
}

BOOST_AUTO_TEST_CASE(component_properties)
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
                  properties:
                    - id: prop1
                      value: 1
                    - id: prop2
                      value: 2
    )"s;
    YmlSystem::System systemObj = parser.parse(system, "");
    const auto& prop1 = systemObj.components[0].properties[0];
    const auto& prop2 = systemObj.components[0].properties[1];
    BOOST_CHECK_EQUAL(prop1.id, "prop1");
    BOOST_CHECK_EQUAL(prop2.id, "prop2");
    BOOST_CHECK_EQUAL(prop1.value, "1");
    BOOST_CHECK_EQUAL(prop2.value, "2");
}

BOOST_AUTO_TEST_CASE(link_capacity_connections_are_parsed)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            components:
                - id: link-addon
                  model: invest.link
            link-capacity-connections:
                - component: link-addon
                  port: capacity_port
                  link-component:
                    from: de
                    to: fr
    )"s;
    YmlSystem::System systemObj = parser.parse(system, "");
    BOOST_REQUIRE_EQUAL(systemObj.linkCapacityConnections.size(), 1);
    const auto& connection = systemObj.linkCapacityConnections[0];
    BOOST_CHECK_EQUAL(connection.componentId, "link-addon");
    BOOST_CHECK_EQUAL(connection.portId, "capacity_port");
    BOOST_CHECK_EQUAL(connection.linkComponent.from, "de");
    BOOST_CHECK_EQUAL(connection.linkComponent.to, "fr");
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
    BOOST_CHECK_THROW(parser.parse(system, ""), InputError);
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
    BOOST_CHECK_THROW(parser.parse(system, ""), InputError);
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
    BOOST_CHECK_THROW(parser.parse(system, ""), InputError);
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
    BOOST_CHECK_THROW(parser.parse(system, ""), InputError);
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
    BOOST_CHECK_THROW(parser.parse(system, ""), InputError);
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
    BOOST_CHECK_THROW(parser.parse(system, ""), InputError);
}

BOOST_AUTO_TEST_CASE(link_capacity_connection_is_a_scalar_throws_input_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            link-capacity-connections:
                - "not a map"
    )"s;
    BOOST_CHECK_THROW(parser.parse(system, ""), InputError);
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
    BOOST_CHECK_THROW(parser.parse(system, ""), InputError);
}

BOOST_AUTO_TEST_CASE(connection_with_extra_field_is_rejected)
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
    BOOST_CHECK_EXCEPTION(parser.parse(system, ""),
                          InputError,
                          containsMessage("Unrecognised field: extra"));
}

BOOST_AUTO_TEST_CASE(all_missing_fields_are_reported_at_once)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            components:
                - id: N
                  scenario-groups: g
                - model: lib.model
            connections:
                - component1: N
                  port1: p1
    )"s;
    BOOST_CHECK_EXCEPTION(parser.parse(system, ""),
                          InputError,
                          [](const InputError& e)
                          {
                              const std::string msg = e.what();
                              return msg.find("Missing field: model") != std::string::npos
                                     && msg.find("Missing field: id") != std::string::npos
                                     && msg.find("Missing field: component2") != std::string::npos;
                          });
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
    BOOST_CHECK_THROW(parser.parse(system, ""), InputError);
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
    BOOST_CHECK_THROW(parser.parse(system, ""), InputError);
}

BOOST_AUTO_TEST_CASE(link_capacity_connection_with_wrong_field_count_throws_input_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            link-capacity-connections:
                - component: c1
                  port: p1
    )"s;
    BOOST_CHECK_THROW(parser.parse(system, ""), InputError);
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
    BOOST_CHECK_THROW(parser.parse(system, ""), InputError);
}

BOOST_AUTO_TEST_CASE(thermal_component_null_throws_input_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            thermal-capacity-connections:
                - component: c1
                  port: p1
                  thermal-component: ~
    )"s;
    BOOST_CHECK_THROW(parser.parse(system, ""), InputError);
}

BOOST_AUTO_TEST_CASE(link_component_with_wrong_field_count_throws_input_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            link-capacity-connections:
                - component: c1
                  port: p1
                  link-component:
                    from: fr
    )"s;
    BOOST_CHECK_THROW(parser.parse(system, ""), InputError);
}

BOOST_AUTO_TEST_CASE(link_component_null_throws_input_error)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            link-capacity-connections:
                - component: c1
                  port: p1
                  link-component: ~
    )"s;
    BOOST_CHECK_THROW(parser.parse(system, ""), InputError);
}

BOOST_AUTO_TEST_CASE(non_mapping_entries_are_rejected_by_field_validation)
{
    YmlSystem::Parser parser;
    const std::vector<std::string> systems = {
      "system:\n  components:\n    - scalar\n",
      "system:\n  connections:\n    - scalar\n",
      "system:\n  area-connections:\n    - scalar\n",
      "system:\n  thermal-capacity-connections:\n    - scalar\n",
      "system:\n  thermal-capacity-connections:\n    - component: c\n      port: p\n"
      "      thermal-component: null\n",
      "system:\n  link-capacity-connections:\n    - scalar\n",
      "system:\n  link-capacity-connections:\n    - component: c\n      port: p\n"
      "      link-component: null\n",
      "system: scalar\n"};
    for (const auto& system: systems)
    {
        BOOST_TEST_CONTEXT(system)
        {
            BOOST_CHECK_EXCEPTION(parser.parse(system, ""),
                                  InputError,
                                  containsMessage("expected a YAML mapping"));
        }
    }
}

BOOST_AUTO_TEST_CASE(misspelled_component_field_is_rejected)
{
    YmlSystem::Parser parser;
    const auto system = R"(
        system:
            id: base_system
            components:
                - id: N
                  model: lib.model
                  scenario-groups: g
    )"s;
    BOOST_CHECK_EXCEPTION(parser.parse(system, ""),
                          InputError,
                          containsMessage("Unrecognised field: scenario-groups"));
}
