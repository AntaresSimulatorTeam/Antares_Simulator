// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include "antares/expressions/nodes/LiteralNode.h"
#include "antares/study/system-model/component.h"
// If we don't turn clang-format off here, some antlr4 header does not compile :
// it collides with a #include <windows.h> somewhere in Yuni
// clang-format off
#include <unit_test_utils.h>
// clang-format on
#include <boost/test/unit_test.hpp>

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Optimisation;

struct ComponentBuilderCreationFixture
{
    ComponentBuilder component_builder;
};

static Model createModelWithParameters()
{
    ModelBuilder model_builder;
    return model_builder.withId("model")
      .withParameters({Parameter("param1", TimeDependent::NO, ScenarioDependent::NO),
                       Parameter("param2", TimeDependent::NO, ScenarioDependent::NO)})
      .build();
}

static Model createModelWithoutParameters()
{
    ModelBuilder model_builder;
    return model_builder.withId("model").build();
}

BOOST_FIXTURE_TEST_SUITE(_Component_, ComponentBuilderCreationFixture)

std::pair<std::string, ParameterTypeAndValue> build_context_parameter_with(
  const std::string& id,
  const std::string& value,
  const VariabilityType& type = VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO)
{
    return {id, {.id = id, .type = type, .value = value}};
}

BOOST_AUTO_TEST_CASE(nominal_build_with_parameters)
{
    Model model = createModelWithParameters();
    auto component = component_builder.withId("component")
                       .withModel(&model)
                       .withParameterValues({build_context_parameter_with("param1", "5"),
                                             build_context_parameter_with("param2", "3")})
                       .withScenarioGroupId("scenario_group")
                       .build();
    BOOST_CHECK_EQUAL(component.Id(), "component");
    BOOST_CHECK_EQUAL(component.getModel(), &model);
    BOOST_CHECK_EQUAL(std::stod(component.getParameterValue("param1")), 5);
    BOOST_CHECK_EQUAL(std::stod(component.getParameterValue("param2")), 3);
    BOOST_CHECK_EXCEPTION(component.getParameterValue("param3"),
                          std::invalid_argument,
                          checkMessage("Parameter 'param3' not found in component 'component'"));
    BOOST_CHECK_EQUAL(component.getScenarioGroupId(), "scenario_group");
}

BOOST_AUTO_TEST_CASE(nominal_build_without_parameters1)
{
    Model model = createModelWithoutParameters();
    auto component = component_builder.withId("component2")
                       .withModel(&model)
                       .withParameterValues({})
                       .withScenarioGroupId("scenario_group2")
                       .build();
    BOOST_CHECK_EQUAL(component.Id(), "component2");
    BOOST_CHECK_EQUAL(component.getModel(), &model);
    BOOST_CHECK_EXCEPTION(component.getParameterValue("param1"),
                          std::invalid_argument,
                          checkMessage("Parameter 'param1' not found in component 'component2'"));
    BOOST_CHECK_EQUAL(component.getScenarioGroupId(), "scenario_group2");
}

BOOST_AUTO_TEST_CASE(nominal_build_without_parameters2)
{
    Model model = createModelWithoutParameters();
    auto component = component_builder.withId("component3")
                       .withModel(&model)
                       .withScenarioGroupId("scenario_group3")
                       .build();
    BOOST_CHECK_EQUAL(component.Id(), "component3");
    BOOST_CHECK_EQUAL(component.getModel(), &model);
    BOOST_CHECK_EQUAL(component.getScenarioGroupId(), "scenario_group3");
}

BOOST_AUTO_TEST_CASE(reuse_builder)
{
    Model model1 = createModelWithoutParameters();
    auto component1 = component_builder.withId("component1")
                        .withModel(&model1)
                        .withScenarioGroupId("scenario_group1")
                        .build();
    Model model2 = createModelWithParameters();
    auto component2 = component_builder.withId("component2")
                        .withModel(&model2)
                        .withParameterValues({build_context_parameter_with("param1", "5"),
                                              build_context_parameter_with("param2", "3")})
                        .withScenarioGroupId("scenario_group2")
                        .build();

    BOOST_CHECK_EQUAL(component1.Id(), "component1");
    BOOST_CHECK_EQUAL(component1.getModel(), &model1);
    BOOST_CHECK_EQUAL(component1.getScenarioGroupId(), "scenario_group1");
    BOOST_CHECK(component1.getParameterValues().empty());

    BOOST_CHECK_EQUAL(component2.Id(), "component2");
    BOOST_CHECK_EQUAL(component2.getModel(), &model2);
    BOOST_CHECK_EQUAL(component2.getScenarioGroupId(), "scenario_group2");
    BOOST_CHECK_EQUAL(component2.getParameterValues().size(), 2);
    BOOST_CHECK_EQUAL(std::stod(component2.getParameterValues().at("param1").value), 5);
    BOOST_CHECK_EQUAL(std::stod(component2.getParameterValues().at("param2").value), 3);
}

BOOST_AUTO_TEST_CASE(fail_on_no_id)
{
    Model model = createModelWithoutParameters();
    auto component = component_builder.withModel(&model).withScenarioGroupId("scenario_group");
    BOOST_CHECK_EXCEPTION(component_builder.build(),
                          std::invalid_argument,
                          checkMessage("A component can't have an empty id"));
}

BOOST_AUTO_TEST_CASE(fail_on_no_model)
{
    auto component = component_builder.withId("component").withScenarioGroupId("scenario_group");
    BOOST_CHECK_EXCEPTION(component_builder.build(),
                          std::invalid_argument,
                          checkMessage("A component can't have an empty model"));
}

BOOST_AUTO_TEST_CASE(fail_on_no_params1)
{
    Model model = createModelWithParameters();
    auto component = component_builder.withId("component")
                       .withModel(&model)
                       .withScenarioGroupId("scenario_group");
    BOOST_CHECK_EXCEPTION(component_builder.build(),
                          std::invalid_argument,
                          checkMessage(
                            "The component \"component\" has 0 parameter(s), but its model has 2"));
}

BOOST_AUTO_TEST_CASE(fail_on_no_params2)
{
    Model model = createModelWithParameters();
    auto component = component_builder.withId("component")
                       .withModel(&model)
                       .withParameterValues({})
                       .withScenarioGroupId("scenario_group");
    BOOST_CHECK_EXCEPTION(component_builder.build(),
                          std::invalid_argument,
                          checkMessage(
                            "The component \"component\" has 0 parameter(s), but its model has 2"));
}

BOOST_AUTO_TEST_CASE(fail_on_missing_param)
{
    Model model = createModelWithParameters();
    auto component = component_builder.withId("component")
                       .withModel(&model)
                       .withParameterValues({build_context_parameter_with("param2", "3")})
                       .withScenarioGroupId("scenario_group");
    BOOST_CHECK_EXCEPTION(component_builder.build(),
                          std::invalid_argument,
                          checkMessage(
                            "The component \"component\" has 1 parameter(s), but its model has 2"));
}

BOOST_AUTO_TEST_CASE(fail_on_missing_wrong_param)
{
    Model model = createModelWithParameters();
    auto component = component_builder.withId("component")
                       .withModel(&model)
                       .withParameterValues({build_context_parameter_with("param_1", "3"),
                                             build_context_parameter_with("param2", "3")})
                       .withScenarioGroupId("scenario_group");
    BOOST_CHECK_EXCEPTION(component_builder.build(),
                          std::invalid_argument,
                          checkMessage(
                            "The component 'component' has no value for parameter 'param1'"));
}

BOOST_AUTO_TEST_CASE(fail_on_too_many_params1)
{
    Model model = createModelWithParameters();
    auto component = component_builder.withId("component")
                       .withModel(&model)
                       .withParameterValues({build_context_parameter_with("param1", "3"),
                                             build_context_parameter_with("param2", "3"),
                                             build_context_parameter_with("param3", "3")})
                       .withScenarioGroupId("scenario_group");
    BOOST_CHECK_EXCEPTION(component_builder.build(),
                          std::invalid_argument,
                          checkMessage(
                            "The component \"component\" has 3 parameter(s), but its model has 2"));
}

BOOST_AUTO_TEST_CASE(fail_on_too_many_params2)
{
    Model model = createModelWithoutParameters();
    auto component = component_builder.withId("component")
                       .withModel(&model)
                       .withParameterValues({build_context_parameter_with("param1", "3")})
                       .withScenarioGroupId("scenario_group");
    BOOST_CHECK_EXCEPTION(component_builder.build(),
                          std::invalid_argument,
                          checkMessage(
                            "The component \"component\" has 1 parameter(s), but its model has 0"));
}

BOOST_AUTO_TEST_CASE(param_variability_scenario_mismatch)
{
    Model model = createModelWithParameters();
    auto component = component_builder.withId("component")
                       .withModel(&model)
                       .withParameterValues(
                         {build_context_parameter_with("param1",
                                                       "3",
                                                       VariabilityType::VARYING_IN_SCENARIO_ONLY),
                          build_context_parameter_with("param2", "3")})
                       .withScenarioGroupId("scenario_group");
    BOOST_CHECK_EXCEPTION(component_builder.build(),
                          std::invalid_argument,
                          checkMessage("Model 'model': Component 'component': Parameter 'param1': "
                                       "Scenario dependance mismatch between model and system"));
}

BOOST_AUTO_TEST_CASE(param_variability_time_mismatch)
{
    Model model = createModelWithParameters();
    auto component = component_builder.withId("component")
                       .withModel(&model)
                       .withParameterValues(
                         {build_context_parameter_with("param1", "3"),
                          build_context_parameter_with("param2",
                                                       "3",
                                                       VariabilityType::VARYING_IN_TIME_ONLY)})
                       .withScenarioGroupId("scenario_group");
    BOOST_CHECK_EXCEPTION(component_builder.build(),
                          std::invalid_argument,
                          checkMessage("Model 'model': Component 'component': Parameter 'param2': "
                                       "Time dependance mismatch between model and system"));
}

BOOST_AUTO_TEST_CASE(param_variability_time_and_scenario_mismatch)
{
    Model model = createModelWithParameters();
    auto component = component_builder.withId("component")
                       .withModel(&model)
                       .withParameterValues({build_context_parameter_with(
                                               "param1",
                                               "3",
                                               VariabilityType::VARYING_IN_TIME_AND_SCENARIO),
                                             build_context_parameter_with("param2", "3")})
                       .withScenarioGroupId("scenario_group");
    BOOST_CHECK_EXCEPTION(component_builder.build(),
                          std::invalid_argument,
                          checkMessage(
                            "Model 'model': Component 'component': Parameter 'param1': "
                            "Time and Scenario dependance mismatch between model and system"));
}

BOOST_AUTO_TEST_CASE(fail_when_connecting_area_to_unexisting_port)
{
    ModelBuilder model_builder;
    auto model = model_builder.withId("myModel").build();

    auto component = component_builder.withId("myComponent")
                       .withModel(&model)
                       .withScenarioGroupId("sg")
                       .build();

    // Fail if trying to connect an area to an unexisting port
    BOOST_CHECK_EXCEPTION(
      component.addAreaConnection("wrongPort", "area1"),
      std::invalid_argument,
      checkMessage("Cannot connect area \"area1\" to port \"wrongPort\" of component "
                   "\"myComponent\": port does not exist in the component's model \"myModel\""));
    BOOST_CHECK(component.portToAreaConnections().empty());
}

BOOST_AUTO_TEST_CASE(fail_when_connecting_area_to_port_with_no_area_connection_field_id)
{
    PortField field1("field1");
    std::vector portFields1 = {field1};
    PortType portTypeWithoutAreaConnection("portType1", std::move(portFields1), "");

    Port portNoAC("portNoAC", portTypeWithoutAreaConnection);
    ModelBuilder model_builder;
    auto model = model_builder.withId("myModel").withPorts({portNoAC}).build();

    auto component = component_builder.withId("myComponent")
                       .withModel(&model)
                       .withScenarioGroupId("sg")
                       .build();

    BOOST_CHECK_EQUAL(component.areaConnectedToPort("portNoAC").has_value(), false);
    BOOST_CHECK_EXCEPTION(
      component.addAreaConnection("portNoAC", "area1"),
      std::invalid_argument,
      checkMessage(
        "Cannot connect area \"area1\" to port \"portNoAC\" of component \"myComponent\": port "
        "type \"portType1\" has no area-connection field ID defined"));
    BOOST_CHECK_EQUAL(component.areaConnectedToPort("portNoAC").has_value(), false);
    BOOST_CHECK(component.portToAreaConnections().empty());
}

BOOST_AUTO_TEST_CASE(fail_when_connecting_area_to_undefined_field)
{
    PortField field2("field2");
    std::vector portFields2 = {field2};
    PortType portTypeWithAreaConnection("portType2", std::move(portFields2), "field2");

    Port portACNoDef("portACNoDef", portTypeWithAreaConnection);
    ModelBuilder model_builder;
    auto model = model_builder.withId("myModel").withPorts({portACNoDef}).build();

    auto component = component_builder.withId("myComponent")
                       .withModel(&model)
                       .withScenarioGroupId("sg")
                       .build();

    BOOST_CHECK_EQUAL(component.areaConnectedToPort("portACNoDef").has_value(), false);
    BOOST_CHECK_EXCEPTION(
      component.addAreaConnection("portACNoDef", "area1"),
      std::invalid_argument,
      checkMessage(
        "Cannot connect area \"area1\" to port \"portACNoDef\" of component \"myComponent\": "
        "port field \"field2\" is not defined in the component's model \"myModel\""));
    BOOST_CHECK_EQUAL(component.areaConnectedToPort("portACNoDef").has_value(), false);
    BOOST_CHECK(component.portToAreaConnections().empty());
}

BOOST_AUTO_TEST_CASE(successfully_connect_area_to_port)
{
    PortField field2("field2");
    std::vector portFields2 = {field2};
    PortType portTypeWithAreaConnection("portType2", std::move(portFields2), "field2");

    Port portACDef("portACDef", portTypeWithAreaConnection);
    std::vector<PortFieldDefinition> portFieldDefs;
    portFieldDefs.emplace_back(portACDef, field2, Expression());

    ModelBuilder model_builder;
    auto model = model_builder.withId("myModel")
                   .withPorts({portACDef})
                   .withPortFieldDefinitions(std::move(portFieldDefs))
                   .build();

    auto component = component_builder.withId("myComponent")
                       .withModel(&model)
                       .withScenarioGroupId("sg")
                       .build();

    BOOST_CHECK_EQUAL(component.areaConnectedToPort("portACDef").has_value(), false);
    component.addAreaConnection("portACDef", "area1");
    BOOST_CHECK_EQUAL(component.areaConnectedToPort("portACDef").has_value(), true);
    BOOST_CHECK_EQUAL(component.areaConnectedToPort("portACDef").value(), "area1");
    BOOST_CHECK_EQUAL(component.portToAreaConnections().size(), 1);
    BOOST_CHECK_EQUAL(component.portToAreaConnections().at("portACDef"), "area1");
    BOOST_CHECK_EXCEPTION(component.addAreaConnection("portACDef", "area2"),
                          std::invalid_argument,
                          checkMessage(
                            "Cannot connect area \"area2\" to port \"portACDef\" of component "
                            "\"myComponent\": port is already connected to \"area1\""));
    BOOST_CHECK_EQUAL(component.areaConnectedToPort("portACDef").has_value(), true);
    BOOST_CHECK_EQUAL(component.areaConnectedToPort("portACDef").value(), "area1");
    BOOST_CHECK_EQUAL(component.portToAreaConnections().size(), 1);
    BOOST_CHECK_EQUAL(component.portToAreaConnections().at("portACDef"), "area1");

    BOOST_CHECK_THROW(component.nodeAtPortField("wrong port", "field"), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()
