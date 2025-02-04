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

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include "antares/study/system-model/component.h"

using namespace Antares::Study::SystemModel;

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

BOOST_AUTO_TEST_CASE(nominal_build_with_parameters)
{
    Model model = createModelWithParameters();
    auto component = component_builder.withId("component")
                       .withModel(&model)
                       .withParameterValues({{"param1", 5}, {"param2", 3}})
                       .withScenarioGroupId("scenario_group")
                       .build();
    BOOST_CHECK_EQUAL(component.Id(), "component");
    BOOST_CHECK_EQUAL(component.getModel(), &model);
    BOOST_CHECK_EQUAL(component.getParameterValue("param1"), 5);
    BOOST_CHECK_EQUAL(component.getParameterValue("param2"), 3);
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
                        .withParameterValues({{"param1", 5}, {"param2", 3}})
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
    BOOST_CHECK_EQUAL(component2.getParameterValues().at("param1"), 5);
    BOOST_CHECK_EQUAL(component2.getParameterValues().at("param2"), 3);
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

BOOST_AUTO_TEST_CASE(fail_on_no_scenario_group_id)
{
    Model model = createModelWithoutParameters();
    auto component = component_builder.withId("component").withModel(&model);
    BOOST_CHECK_EXCEPTION(component_builder.build(),
                          std::invalid_argument,
                          checkMessage("A component can't have an empty scenario_group_id"));
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
                       .withParameterValues({{"param2", 3}})
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
                       .withParameterValues({{"param_1", 3}, {"param2", 3}})
                       .withScenarioGroupId("scenario_group");
    BOOST_CHECK_EXCEPTION(component_builder.build(),
                          std::invalid_argument,
                          checkMessage(
                            "The component \"component\" has no value for parameter 'param1'"));
}

BOOST_AUTO_TEST_CASE(fail_on_too_many_params1)
{
    Model model = createModelWithParameters();
    auto component = component_builder.withId("component")
                       .withModel(&model)
                       .withParameterValues({{"param1", 3}, {"param2", 3}, {"param3", 3}})
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
                       .withParameterValues({{"param1", 3}})
                       .withScenarioGroupId("scenario_group");
    BOOST_CHECK_EXCEPTION(component_builder.build(),
                          std::invalid_argument,
                          checkMessage(
                            "The component \"component\" has 1 parameter(s), but its model has 0"));
}

BOOST_AUTO_TEST_SUITE_END()
