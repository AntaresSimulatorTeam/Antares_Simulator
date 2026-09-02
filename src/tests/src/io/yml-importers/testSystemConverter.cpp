// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <stdexcept>
#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/io/inputs/yml-system/converter.h>
#include "antares/io/inputs/InputError.h"
#include "antares/io/inputs/model-converter/modelConverter.h"
#include "antares/io/inputs/yml-model/parser.h"
#include "antares/study/system-model/library.h"

using namespace std::string_literals;
using namespace Antares::IO::Inputs;
using namespace Antares::ModelerStudy;

struct LibraryObjects
{
    YmlModel::Model model1{.id = "node",
                           .description = "description",
                           .parameters = {{"cost", true, false}},
                           .variables = {},
                           .ports = {},
                           .port_field_definitions = {},
                           .constraints = {{"constraint1",
                                            YmlModel::ExpressionLineNumber{"cost", 0},
                                            "subproblems",
                                            ""}},
                           .binding_constraints = {},
                           .objectives = {},
                           .extra_outputs = {},
                           .filename = ""};

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

auto getComponent(const std::vector<SystemModel::Component>& components, const std::string& id)
{
    return std::ranges::find_if(components,
                                [&id](const auto& component) { return component.Id() == id; });
}

BOOST_FIXTURE_TEST_CASE(full_model_system, LibraryObjects)
{
    const auto system = R"(
        system:
            id: base_system
            description: real application model
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

    auto systemModel = SystemConverter::convert(systemObj, libraries);

    const auto& components = systemModel.Components();
    BOOST_CHECK_EQUAL(components.size(), 1);
    const auto compoN = getComponent(components, "N");
    BOOST_CHECK(compoN != components.cend());
    BOOST_CHECK_EQUAL(compoN->getModel()->Id(), "node");
    BOOST_CHECK_EQUAL(std::stod(getComponent(components, "N")->getParameterValue("cost")), 30);
}

BOOST_FIXTURE_TEST_CASE(bad_param_name_in_component, LibraryObjects)
{
    const auto system = R"(
        system:
            id: base_system
            description: real application model
            components:
                - id: N
                  model: std.node
                  scenario-group: group-234
                  parameters:
                    - id: param_not_in_model
                      time-dependent: false
                      scenario-dependent: false
                      value: 30
    )"s;

    YmlSystem::System systemObj = parser.parse(system, "");

    BOOST_CHECK_THROW(SystemConverter::convert(systemObj, libraries), std::invalid_argument);
}

BOOST_FIXTURE_TEST_CASE(library_not_existing, LibraryObjects)
{
    const auto system = R"(
        system:
            id: base_system
            components:
                - id: N
                  model: abc.node
                  scenario-group: group-234
    )"s;

    YmlSystem::System systemObj = parser.parse(system, "");

    BOOST_CHECK_THROW(SystemConverter::convert(systemObj, libraries), InputError);
}

BOOST_FIXTURE_TEST_CASE(model_not_existing, LibraryObjects)
{
    const auto system = R"(
        system:
            id: base_system
            components:
                - id: N
                  model: std.abc
                  scenario-group: group-234
    )"s;

    YmlSystem::System systemObj = parser.parse(system, "");

    BOOST_CHECK_THROW(SystemConverter::convert(systemObj, libraries), InputError);
}

BOOST_FIXTURE_TEST_CASE(bad_library_model_format, LibraryObjects)
{
    const auto system = R"(
        system:
            id: base_system
            components:
                - id: N
                  model: std___node
                  scenario-group: group-234
                  parameters:
                    - id: cost
                      time-dependent: false
                      scenario-dependent: false
                      value: 30
    )"s;

    YmlSystem::System systemObj = parser.parse(system, "");

    BOOST_CHECK_THROW(SystemConverter::convert(systemObj, libraries), InputError);
}

static const auto libraryYaml_1 = R"(
        library:
          id: std
          description: Standard library
          port-types:
            - id: flow
              description: abc
              fields:
                  - id: port_name

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
                  field: port_name
                  definition: generation
              objective:
                - id: objective
                - expression: cost * generation

            - id: node
              description: A basic balancing node model
              ports:
                - id: injection_port
                  type: flow
    )"s;

static const auto libraryYaml_2 = R"(
        library:
          id: mylib
          description: Extra library
          port-types:
            - id: flow
              description: abc
              fields:
                  - id: port_name

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
                  field: port_name
                  definition: -demand
    )"s;

static const auto systemYml = R"(
        system:
          id: system1
          description: basic description

          components:
            - id: N
              model: std.node
              scenario-group: group-234

            - id: G
              model: std.generator
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

            - id: D
              model: mylib.demand
              scenario-group: group-qsf
              parameters:
                - id: demand
                  time-dependent: false
                  scenario-dependent: false
                  value: 100
    )"s;

BOOST_AUTO_TEST_CASE(Full_system_test)
{
    YmlModel::Parser parserModel;
    YmlSystem::Parser parserSystem;

    std::vector<SystemModel::Library> libraries;
    libraries.push_back(ModelConverter::convert(parserModel.parse(libraryYaml_1)));
    libraries.push_back(ModelConverter::convert(parserModel.parse(libraryYaml_2)));

    YmlSystem::System systemObj = parserSystem.parse(systemYml, "");
    auto systemModel = SystemConverter::convert(systemObj, libraries);

    const auto& components = systemModel.Components();
    BOOST_CHECK_EQUAL(components.size(), 3);
    const auto compoN = getComponent(components, "N");
    BOOST_CHECK_EQUAL(compoN->Id(), "N");
    BOOST_CHECK_EQUAL(compoN->getModel()->Id(), "node");
    BOOST_CHECK_EQUAL(compoN->getScenarioGroupId(), "group-234");

    const auto compoG = getComponent(components, "G");
    BOOST_CHECK_EQUAL(compoG->getModel()->Id(), "generator");
    BOOST_CHECK_EQUAL(std::stod(compoG->getParameterValue("cost")), 30);
    BOOST_CHECK_EQUAL(std::stod(compoG->getParameterValue("p_max")), 100);

    const auto compoD = getComponent(components, "D");
    BOOST_CHECK_EQUAL(compoD->getModel()->Id(), "demand");
    BOOST_CHECK_EQUAL(std::stod(compoD->getParameterValue("demand")), 100);
}

constexpr size_t componentsPos = 10;
constexpr size_t connectionEntryPos = componentsPos + 2;
const std::string connectionFirstCompoMargin(connectionEntryPos, ' ');
const std::string connectionOtherFieldsMargin = connectionFirstCompoMargin + "  ";

struct RawConnection
{
    std::string firstCompo;
    std::string firstPort;
    std::string secondCompo;
    std::string secondPort;
};

void AddConnectionsToSystem(std::string& system, const std::vector<RawConnection>& connections)
{
    for (const auto& [firstCompo, firstPort, secondCompo, secondPort]: connections)
    {
        system += "\n";
        system += connectionFirstCompoMargin + "- component1: " + firstCompo;
        system += "\n";
        system += connectionOtherFieldsMargin + "port1: " + firstPort;
        system += "\n";
        system += connectionOtherFieldsMargin + "component2: " + secondCompo;
        system += "\n";
        system += connectionOtherFieldsMargin + "port2: " + secondPort;
    }
}

struct PrepareYaml
{
    YmlModel::Parser parserModel;
    YmlSystem::Parser parserSystem;
    std::vector<SystemModel::Library> libraries;

    std::string system = systemYml;

    PrepareYaml()
    {
        system += "\n";
        system += std::string(componentsPos, ' ') + "connections:";
        libraries.push_back(ModelConverter::convert(parserModel.parse(libraryYaml_1)));
        libraries.push_back(ModelConverter::convert(parserModel.parse(libraryYaml_2)));
    }
};

BOOST_FIXTURE_TEST_CASE(TryPortSelfConnection, PrepareYaml)
{
    AddConnectionsToSystem(system,
                           {{.firstCompo = "G",
                             .firstPort = "injection_port",
                             .secondCompo = "G",
                             .secondPort = "injection_port"}});

    YmlSystem::System systemObj = parserSystem.parse(system, "");
    BOOST_CHECK_THROW(SystemConverter::convert(systemObj, libraries), InputError);
}

BOOST_FIXTURE_TEST_CASE(SystemWithSenderAndReceiverPort, PrepareYaml)
{
    const std::string port_id = "injection_port";
    AddConnectionsToSystem(
      system,
      {{.firstCompo = "N", .firstPort = port_id, .secondCompo = "D", .secondPort = port_id}});

    YmlSystem::System systemObj = parserSystem.parse(system, "");
    auto systemModel = SystemConverter::convert(systemObj, libraries);

    auto& components = systemModel.Components();
    const auto component_N = getComponent(components, "N");
    const auto component_G = getComponent(components, "G");
    const auto component_D = getComponent(components, "D");

    auto connections_to_N = component_N->componentConnectionsViaPort(port_id);
    auto connections_to_G = component_G->componentConnectionsViaPort(port_id);
    auto connections_to_D = component_D->componentConnectionsViaPort(port_id);

    BOOST_CHECK(connections_to_N.size() == 1);
    BOOST_CHECK(connections_to_G.size() == 0);
    BOOST_CHECK(connections_to_D.size() == 1);

    BOOST_CHECK(connections_to_N[0].component()->Id() == "D");
    BOOST_CHECK(connections_to_D[0].component()->Id() == "N");
}

BOOST_FIXTURE_TEST_CASE(TryToConnectWithUnknownCompo, PrepareYaml)
{
    AddConnectionsToSystem(system,
                           {{.firstCompo = "N",
                             .firstPort = "injection_port",
                             .secondCompo = "DD",
                             .secondPort = "injection_port"}});
    YmlSystem::System systemObj = parserSystem.parse(system, "");
    BOOST_CHECK_THROW(SystemConverter::convert(systemObj, libraries), InputError);
}

BOOST_FIXTURE_TEST_CASE(TryToConnectWithUnknownPort, PrepareYaml)
{
    AddConnectionsToSystem(system,
                           {{.firstCompo = "N",
                             .firstPort = "injection_port",
                             .secondCompo = "D",
                             .secondPort = "yosh!"}});
    YmlSystem::System systemObj = parserSystem.parse(system, "");
    BOOST_CHECK_THROW(SystemConverter::convert(systemObj, libraries), std::invalid_argument);
}

BOOST_FIXTURE_TEST_CASE(DuplicatedCompo, PrepareYaml)
{
    const auto duplicatedCompo = R"(
        system:
          id: system1
          description: basic description
          components:
            - id: N
              model: std.node
              scenario-group: group-234
            - id: N
              model: std.lib
              scenario-group: group-234
)";

    YmlSystem::System systemObj = YmlSystem::Parser().parse(duplicatedCompo, "");
    BOOST_CHECK_EXCEPTION(SystemConverter::convert(systemObj,
                                                   {ModelConverter::convert(
                                                     YmlModel::Parser().parse(libraryYaml_1))}),
                          InputError,
                          checkMessage("System has at least two components with the same id "
                                       "('N'), this is not supported"));
}

// In a binding constraint containing sum_connections(...), non-linear functions (max, min,
// floor, ...) are allowed only when they do not involve variables, since in that case they
// only depend on parameters and literals, which evaluate to a constant before the linear
// optimization.
namespace
{
std::string nonLinearBehindConnectionLibrary(const std::string& portFieldDefinition)
{
    return R"(
        library:
          id: lib-nl
          port-types:
            - id: flow
              fields:
                - id: port_name
          models:
            - id: node
              parameters:
                - id: a
                - id: b
              ports:
                - id: injection_port
                  type: flow
              binding-constraints:
                - id: balance
                  expression: sum_connections(injection_port.port_name) = max(a, b)
            - id: sender
              parameters:
                - id: c
              variables:
                - id: x
              ports:
                - id: injection_port
                  type: flow
              port-field-definitions:
                - port: injection_port
                  field: port_name
                  definition: )"
           + portFieldDefinition + R"(
    )"s;
}

const auto nonLinearBehindConnectionSystem = R"(
        system:
          id: system-nl
          components:
            - id: node1
              model: lib-nl.node
              parameters:
                - id: a
                  time-dependent: false
                  scenario-dependent: false
                  value: 1
                - id: b
                  time-dependent: false
                  scenario-dependent: false
                  value: 2
            - id: sender1
              model: lib-nl.sender
              parameters:
                - id: c
                  time-dependent: false
                  scenario-dependent: false
                  value: 3
          connections:
            - component1: node1
              port1: injection_port
              component2: sender1
              port2: injection_port
    )"s;
} // namespace

BOOST_AUTO_TEST_CASE(nonlinear_function_with_only_parameters_is_allowed_behind_connection)
{
    const auto yaml = nonLinearBehindConnectionLibrary("-c");
    auto libraries = std::vector<SystemModel::Library>{
      ModelConverter::convert(YmlModel::Parser().parse(yaml))};

    YmlSystem::System systemObj = YmlSystem::Parser().parse(nonLinearBehindConnectionSystem, "");
    // max(a, b) only involves parameters : the binding constraint remains linear
    BOOST_CHECK_NO_THROW(SystemConverter::convert(systemObj, libraries));
}

BOOST_AUTO_TEST_CASE(nonlinear_function_with_variable_is_rejected_behind_connection)
{
    const auto yaml = nonLinearBehindConnectionLibrary("max(x, c)");
    auto libraries = std::vector<SystemModel::Library>{
      ModelConverter::convert(YmlModel::Parser().parse(yaml))};

    YmlSystem::System systemObj = YmlSystem::Parser().parse(nonLinearBehindConnectionSystem, "");
    // max(x, c) involves a variable : the binding constraint would be non-linear
    BOOST_CHECK_THROW(SystemConverter::convert(systemObj, libraries), InputError);
}
