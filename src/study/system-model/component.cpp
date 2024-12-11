/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <ranges>

#include <antares/study/system-model/component.h>

namespace Antares::Study::SystemModel
{

static void checkComponentDataValidity(const ComponentData& data)
{
    // Check that mandatory attributes are not empty
    if (data.id.empty())
    {
        throw std::invalid_argument("A component can't have an empty id");
    }
    if (data.model == nullptr)
    {
        throw std::invalid_argument("A component can't have an empty model");
    }
    if (data.scenario_group_id.empty())
    {
        throw std::invalid_argument("A component can't have an empty scenario_group_id");
    }
    // Check that parameters values are coherent with the model
    if (data.model->Parameters().size() != data.parameter_values.size())
    {
        throw std::invalid_argument(
          "The component has " + std::to_string(data.parameter_values.size())
          + " parameter(s), but its model has " + std::to_string(data.model->Parameters().size()));
    }
    for (const auto param: data.model->Parameters() | std::views::keys)
    {
        if (!data.parameter_values.contains(param))
        {
            throw std::invalid_argument("The component has no value for parameter '" + param + "'");
        }
    }
}

Component::Component(const ComponentData& component_data)
{
    checkComponentDataValidity(component_data);
    data_ = std::move(component_data);
}

/**
 * \brief Sets the ID of the component.
 *
 * \param id The ID to set.
 * \return Reference to the ComponentBuilder object.
 */
ComponentBuilder& ComponentBuilder::withId(const std::string_view id)
{
    data_.id = id;
    return *this;
}

/**
 * \brief Sets the model of the component.
 *
 * \param model The model to set.
 * \return Reference to the ComponentBuilder object.
 */
ComponentBuilder& ComponentBuilder::withModel(const Model* model)
{
    data_.model = model;
    return *this;
}

/**
 * \brief Sets the parameter values of the component. The parameters included should be all of the
 * model's parameters.
 *
 * \param parameter_values The map of parameter values to set.
 * \return Reference to the ComponentBuilder object.
 */
ComponentBuilder& ComponentBuilder::withParameterValues(
  std::map<std::string, double> parameter_values)
{
    data_.parameter_values = std::move(parameter_values);
    return *this;
}

/**
 * \brief Sets the ID of the scenario group to which the component belongs.
 *
 * \param scenario_group_id The scenario group ID to set.
 * \return Reference to the ComponentBuilder object.
 */
ComponentBuilder& ComponentBuilder::withScenarioGroupId(const std::string& scenario_group_id)
{
    data_.scenario_group_id = scenario_group_id;
    return *this;
}

/**
 * \brief Builds and returns the Component object.
 *
 * \return The constructed Component object.
 */
Component ComponentBuilder::build()
{
    Component component(data_);
    data_.reset(); // makes the ComponentBuilder re-usable
    return component;
}

} // namespace Antares::Study::SystemModel
