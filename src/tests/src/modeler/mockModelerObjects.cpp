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

#include "antares/study/system-model/component.h"

using namespace Antares::ModelerStudy::SystemModel;

Model createModelWithParameters()
{
    ModelBuilder model_builder;
    return model_builder.withId("model")
      .withParameters({Parameter("param1", TimeDependent::NO, ScenarioDependent::NO),
                       Parameter("param2", TimeDependent::NO, ScenarioDependent::NO)})
      .build();
}

Model createModelWithoutParameters()
{
    ModelBuilder model_builder;
    return model_builder.withId("model").build();
}

std::pair<std::string, Antares::Expressions::Visitors::ParameterTypeAndValue>
build_context_parameter_with(const std::string& id,
                             const std::string& value,
                             const Antares::Expressions::Visitors::ParameterType& type = Antares::
                               Expressions::Visitors::ParameterType::CONSTANT)
{
    return {id, {.id = id, .type = type, .value = value}};
}

Component createComponent(const std::string& id = "component")
{
    Model model = createModelWithParameters();
    ComponentBuilder component_builder;
    return component_builder.withId(id)
      .withModel(&model)
      .withParameterValues(
        {build_context_parameter_with("param1", "5"), build_context_parameter_with("param2", "3")})
      .withScenarioGroupId("scenario_group")
      .build();
}
