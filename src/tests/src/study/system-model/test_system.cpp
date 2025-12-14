/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include "antares/io/inputs/model-converter/convertorVisitor.h"
#include "antares/io/inputs/yml-model/Library.h"
#include "antares/study/system-model/system.h"

// If we don't turn clang-format off here, some antlr4 header does not compile :
// it collides with a #include <windows.h> somewhere in Yuni
// clang-format off
#include <unit_test_utils.h>
// clang-format on
#include <boost/test/unit_test.hpp>

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::IO::Inputs::ModelConverter;
using namespace Antares::IO::Inputs;

struct SystemBuilderCreationFixture
{
    SystemBuilder system_builder;
    std::vector<Component> components;
};

static Component createComponent(std::string id)
{
    ModelBuilder model_builder;
    auto model = model_builder.withId("model").build();
    ComponentBuilder component_builder;
    auto component = component_builder.withId(id)
                       .withModel(&model)
                       .withScenarioGroupId("scenario_group")
                       .build();
    return component;
}

static Antares::IO::Inputs::YmlModel::Variable GiveMeOneVar()
{
    return {.id = "x",
            .lower_bound = "2",
            .upper_bound = "4",
            .variable_type = Antares::IO::Inputs::YmlModel::ValueType::CONTINUOUS,
            .time_dependent = true,
            .scenario_dependent = true};
}

// port1 sends and port2 receives
static Model createModelWith2PortsOneWayExchange()
{
    ModelBuilder model_builder;
    PortField port_field("field");

    Port port1("port1", PortType("type", {port_field}));
    Port port2("port2", PortType("type", {port_field}));
    const auto var = GiveMeOneVar();
    YmlModel::Model ymlmodel{.id = "model1",
                             .description = "description",
                             .parameters = {},
                             .variables = {var},
                             .ports = {},
                             .port_field_definitions = {{"port1", "field", var.id}},
                             .constraints = {},
                             .binding_constraints = {},
                             .objectives = {},
                             .extra_outputs = {}};
    auto nodeRegistry = convertExpressionToNode(var.id, ymlmodel);
    std::vector<PortFieldDefinition> portFieldDefinitions;
    portFieldDefinitions.emplace_back(port1,
                                      port_field,
                                      Expression(var.id, std::move(nodeRegistry)));

    return model_builder.withId("model")
      .withPorts({port1, port2})
      .withPortFieldDefinitions(std::move(portFieldDefinitions))
      .build();
}

BOOST_AUTO_TEST_SUITE(_System_)

BOOST_FIXTURE_TEST_CASE(nominal_build, SystemBuilderCreationFixture)
{
    components = {createComponent("component1"), createComponent("component2")};
    auto system = system_builder.withId("system").withComponents(std::move(components)).build();
    BOOST_CHECK_EQUAL(system.Id(), "system");
    const auto& compos = system.Components();
    BOOST_CHECK_EQUAL(compos.size(), 2);
    const auto compo1 = std::ranges::find_if(compos,
                                             [](const Component& compo)
                                             { return compo.Id() == "component1"; });
    BOOST_CHECK(compo1 != compos.cend());
    const auto compo2 = std::ranges::find_if(compos,
                                             [](const Component& compo)
                                             { return compo.Id() == "component2"; });
    BOOST_CHECK(compo2 != compos.cend());
}

Component buildComponent(const std::string& id, const Model& model)
{
    ComponentBuilder component_builder;
    auto component = component_builder.withId(id)
                       .withModel(&model)
                       .withScenarioGroupId("scenario_group")
                       .build();
    return component;
}

BOOST_FIXTURE_TEST_CASE(nominal_build_with_connections_two_ports_one_way_exchange,
                        SystemBuilderCreationFixture)
{
    auto model = createModelWith2PortsOneWayExchange();
    auto comp1 = buildComponent("component1", model);
    auto comp2 = buildComponent("component2", model);
    components.push_back(comp1);
    components.push_back(comp2);

    auto system = system_builder.withId("system").withComponents(std::move(components)).build();

    BOOST_CHECK_EQUAL(system.Id(), "system");
    BOOST_CHECK_EQUAL(system.Components().size(), 2);
}

// one port1 sends 'rice' and receives 'corn', port2 does the opposite
static Model createModelWith2Ports2WayExchange()
{
    ModelBuilder model_builder;
    PortField rice("rice");
    PortField corn("corn");

    Port port1("port1", PortType("type", {rice, corn}));
    Port port2("port2", PortType("type", {rice, corn}));
    const auto var = GiveMeOneVar();
    const Antares::IO::Inputs::YmlModel::Parameter p{.id = "corn_price",
                                                     .time_dependent = false,
                                                     .scenario_dependent = false};

    YmlModel::Model ymlmodel{.id = "model1",
                             .description = "description",
                             .parameters = {p},
                             .variables = {var},
                             .ports = {},
                             .port_field_definitions = {{"port1", "rice", var.id},
                                                        {"port2", "corn", p.id}},
                             .constraints = {},
                             .binding_constraints = {},
                             .objectives = {},
                             .extra_outputs = {}};
    auto nodeRegistryForVar = convertExpressionToNode(var.id, ymlmodel);
    auto nodeRegistryForP = convertExpressionToNode(p.id, ymlmodel);
    std::vector<PortFieldDefinition> portFieldDefinitions;
    portFieldDefinitions.emplace_back(port1,
                                      rice,
                                      Expression(var.id, std::move(nodeRegistryForVar)));

    portFieldDefinitions.emplace_back(port2, corn, Expression(p.id, std::move(nodeRegistryForP)));

    return model_builder.withId("model")
      .withPorts({port1, port2})
      .withPortFieldDefinitions(std::move(portFieldDefinitions))
      .build();
}

BOOST_FIXTURE_TEST_CASE(nominal_build_with_connections_two_ports_two_way_exchange,
                        SystemBuilderCreationFixture)
{
    auto model = createModelWith2Ports2WayExchange();
    auto comp1 = buildComponent("component1", model);
    auto comp2 = buildComponent("component2", model);
    components.push_back(comp1);
    components.push_back(comp2);

    auto system = system_builder.withId("system").withComponents(std::move(components)).build();

    BOOST_CHECK_EQUAL(system.Id(), "system");
    BOOST_CHECK_EQUAL(system.Components().size(), 2);
}

BOOST_FIXTURE_TEST_CASE(fail_on_no_id, SystemBuilderCreationFixture)
{
    components = {createComponent("component1"), createComponent("component2")};
    system_builder.withComponents(std::move(components));
    BOOST_CHECK_EXCEPTION(system_builder.build(),
                          std::invalid_argument,
                          checkMessage("A system can't have an empty id"));
}

BOOST_FIXTURE_TEST_CASE(fail_on_no_component1, SystemBuilderCreationFixture)
{
    system_builder.withId("system");
    BOOST_CHECK_EXCEPTION(system_builder.build(),
                          std::invalid_argument,
                          checkMessage("A system must contain at least one component"));
}

BOOST_FIXTURE_TEST_CASE(fail_on_no_component2, SystemBuilderCreationFixture)
{
    system_builder.withId("system").withComponents(std::move(components));
    BOOST_CHECK_EXCEPTION(system_builder.build(),
                          std::invalid_argument,
                          checkMessage("A system must contain at least one component"));
}

BOOST_AUTO_TEST_CASE(PortFieldLessOperator)
{
    BOOST_CHECK(PortField("A") < PortField("B"));
}

BOOST_AUTO_TEST_CASE(PortFieldeEqualityOperator)
{
    BOOST_CHECK_EQUAL(PortField("A") == PortField("B"), false);
    BOOST_CHECK(PortField("A") == PortField("A"));
}

BOOST_AUTO_TEST_CASE(PrintSenderFieldRole)
{
    std::ostringstream os;
    os << FieldRole::Sender;
    BOOST_CHECK_EQUAL(os.str(), "Sender");
}

BOOST_AUTO_TEST_CASE(PrintReceiverFieldRole)
{
    std::ostringstream os;
    os << FieldRole::Receiver;
    BOOST_CHECK_EQUAL(os.str(), "Receiver");
}

BOOST_AUTO_TEST_SUITE_END()
