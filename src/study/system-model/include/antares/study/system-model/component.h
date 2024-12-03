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
#pragma once

#include <map>

#include "model.h"

namespace Antares::Study::SystemModel
{

/**
 * Defines the attributes of the Component class
 * Made into a struct to avoid duplication in ComponentBuilder
 */
class ComponentData
{
public:
    std::string id;
    const Model* model = nullptr;
    std::map<std::string, double> parameter_values;
    std::string scenario_group_id;

    void reset()
    {
        id.clear();
        model = nullptr;
        parameter_values.clear();
        scenario_group_id.clear();
    }
};

/**
 * Defines an actual component of the simulated system.
 */
class Component
{
public:
    // Only allowing one private constructor (see below) to forbid empty Components
    Component() = delete;

    const std::string& Id() const
    {
        return data_.id;
    }

    const Model* getModel() const
    {
        return data_.model;
    }

    const std::map<std::string, double>& getParameterValues() const
    {
        return data_.parameter_values;
    }

    double getParameterValue(const std::string& parameter_id) const
    {
        if (!data_.parameter_values.contains(parameter_id))
        {
            throw std::invalid_argument("Parameter '" + parameter_id + "' not found in component '"
                                        + data_.id + "'");
        }
        return data_.parameter_values.at(parameter_id);
    }

    std::string getScenarioGroupId() const
    {
        return data_.scenario_group_id;
    }

private:
    // Only ComponentBuilder is allowed to build Component instances
    friend class ComponentBuilder;
    explicit Component(const ComponentData& component_data);
    ComponentData data_;
};

class ComponentBuilder
{
public:
    ComponentBuilder& withId(std::string_view id);
    ComponentBuilder& withModel(const Model* model);
    ComponentBuilder& withParameterValues(std::map<std::string, double> parameter_values);
    ComponentBuilder& withScenarioGroupId(const std::string& scenario_group_id);
    Component build();

private:
    ComponentData data_;
};

} // namespace Antares::Study::SystemModel
