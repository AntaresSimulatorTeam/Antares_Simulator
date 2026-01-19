// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "mockModelerObjects.h"

#include "antares/study/system-model/component.h"

using namespace Antares::ModelerStudy::SystemModel;

Model createModelWithParameters(std::vector<Parameter> params)
{
    ModelBuilder model_builder;
    return model_builder.withId("model").withParameters(std::move(params)).build();
}

Model createModelWithoutParameters()
{
    ModelBuilder model_builder;
    return model_builder.withId("model").build();
}

std::pair<std::string, ParameterTypeAndValue> build_context_parameter_with(
  const std::string& id,
  const std::string& value,
  const Antares::Optimisation::VariabilityType& type)
{
    return {id, {.id = id, .type = type, .value = value}};
}

Component createComponent(const Model& model, const std::string& id, unsigned index)
{
    ComponentBuilder component_builder;
    return component_builder.withId(id)
      .withIndex(index)
      .withModel(&model)
      .withParameterValues({})
      .withScenarioGroupId("scenario_group")
      .build();
}

Component createComponent(const Model& model,
                          const std::string& id,
                          std::map<std::string, ParameterTypeAndValue> parameter_values,
                          unsigned index)
{
    ComponentBuilder component_builder;
    return component_builder.withId(id)
      .withIndex(index)
      .withModel(&model)
      .withParameterValues(parameter_values)
      .withScenarioGroupId("scenario_group")
      .build();
}
