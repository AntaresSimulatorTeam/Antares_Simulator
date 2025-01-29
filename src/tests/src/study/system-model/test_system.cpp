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

#include "antares/study/system-model/system.h"

using namespace Antares::Study::SystemModel;

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

BOOST_AUTO_TEST_SUITE(_System_)

BOOST_FIXTURE_TEST_CASE(nominal_build, SystemBuilderCreationFixture)
{
    components = {createComponent("component1"), createComponent("component2")};
    auto system = system_builder.withId("system").withComponents(components).build();
    BOOST_CHECK_EQUAL(system.Id(), "system");
    BOOST_CHECK_EQUAL(system.Components().size(), 2);
    BOOST_CHECK_EQUAL(system.Components().at("component1").Id(), "component1");
    BOOST_CHECK_EQUAL(system.Components().at("component2").Id(), "component2");
}

BOOST_FIXTURE_TEST_CASE(fail_on_no_id, SystemBuilderCreationFixture)
{
    components = {createComponent("component1"), createComponent("component2")};
    system_builder.withComponents(components);
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
    system_builder.withId("system").withComponents(components);
    BOOST_CHECK_EXCEPTION(system_builder.build(),
                          std::invalid_argument,
                          checkMessage("A system must contain at least one component"));
}

BOOST_FIXTURE_TEST_CASE(fail_on_components_with_same_id, SystemBuilderCreationFixture)
{
    components = {createComponent("component1"),
                  createComponent("component2"),
                  createComponent("component2")};
    system_builder.withId("system").withComponents({components});
    BOOST_CHECK_EXCEPTION(system_builder.build(),
                          std::invalid_argument,
                          checkMessage("System has at least two components with the same id "
                                       "('component2'), this is not supported"));
}

BOOST_AUTO_TEST_SUITE_END()
