// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <unit_test_utils.h>

#include <boost/test/data/test_case.hpp>
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

// --------------------------------------------------------------------------
// Decoder error-path coverage: requireMap / requireSize / System::decode
// --------------------------------------------------------------------------

namespace
{
struct SystemErrorCase
{
    std::string name;
    std::string yaml;
    std::string expectedMessage; // empty: only the exception type is checked
};

std::ostream& operator<<(std::ostream& os, const SystemErrorCase& c)
{
    return os << c.name;
}
} // namespace

BOOST_DATA_TEST_CASE(
  invalid_systems_are_rejected,
  boost::unit_test::data::make<SystemErrorCase>({
    {"system_is_a_scalar", "system: scalar\n", "expected a YAML mapping"},
    {"component_is_a_scalar", "system:\n  components:\n    - scalar\n", "expected a YAML mapping"},
    {"parameter_is_a_scalar",
     "system:\n  components:\n    - id: N\n      model: std.node\n      parameters:\n"
     "        - scalar\n",
     "expected a YAML mapping"},
    {"connection_is_a_scalar",
     "system:\n  connections:\n    - scalar\n",
     "expected a YAML mapping"},
    {"area_connection_is_a_scalar",
     "system:\n  area-connections:\n    - scalar\n",
     "expected a YAML mapping"},
    {"thermal_capacity_connection_is_a_scalar",
     "system:\n  thermal-capacity-connections:\n    - scalar\n",
     "expected a YAML mapping"},
    {"thermal_component_is_null",
     "system:\n  thermal-capacity-connections:\n    - component: c\n      port: p\n"
     "      thermal-component: null\n",
     "expected a YAML mapping"},
    {"connection_with_missing_field",
     "system:\n  connections:\n    - component1: N\n      port1: p1\n      component2: M\n",
     ""},
    {"connection_with_extra_field",
     "system:\n  connections:\n    - component1: N\n      port1: p1\n      component2: M\n"
     "      port2: p2\n      extra: bad\n",
     "Unrecognised field: extra"},
    {"area_connection_with_wrong_field_count",
     "system:\n  area-connections:\n    - component: c1\n      port: p1\n",
     ""},
    {"thermal_capacity_connection_with_wrong_field_count",
     "system:\n  thermal-capacity-connections:\n    - component: c1\n      port: p1\n",
     ""},
    {"thermal_component_with_wrong_field_count",
     "system:\n  thermal-capacity-connections:\n    - component: c1\n      port: p1\n"
     "      thermal-component:\n        area: fr\n",
     ""},
    {"misspelled_component_field",
     "system:\n  components:\n    - id: N\n      model: lib.model\n      scenario-groups: g\n",
     "Unrecognised field: scenario-groups"},
  }),
  c)
{
    YmlSystem::Parser parser;
    BOOST_CHECK_EXCEPTION(parser.parse(c.yaml, ""), InputError, containsMessage(c.expectedMessage));
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
