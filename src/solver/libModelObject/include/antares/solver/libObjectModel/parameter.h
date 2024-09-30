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

#include <string>

#include "valueType.h"

namespace Antares::Solver::ObjectModel
{

/**
 * A parameter of the model: a parameter is mainly defined by a name and expected type.
 * When the model is instantiated as a component, a value must be provided for
 * parameters, either as constant values or timeseries-based values.
 */
class Parameter
{
public:
    enum class TimeDependent : bool
    {
        NO = false,
        YES = true
    };

    enum class ScenarioDependent : bool
    {
        NO = false,
        YES = true
    };

    Parameter();

    explicit Parameter(std::string name,
                       ValueType type,
                       TimeDependent timeDependent,
                       ScenarioDependent scenarioDependent):
        name_(std::move(name)),
        type_(type),
        timeDependent_(timeDependent),
        scenarioDependent_(scenarioDependent)
    {
    }

    const std::string& Name() const
    {
        return name_;
    }

    ValueType Type() const
    {
        return type_;
    }

    bool isTimeDependent() const
    {
        return timeDependent_ == TimeDependent::YES;
    }

    bool isScenarioDependent() const
    {
        return scenarioDependent_ == ScenarioDependent::YES;
    }

private:
    std::string name_;
    ValueType type_;
    TimeDependent timeDependent_ = TimeDependent::YES;             // optional at construction
    ScenarioDependent scenarioDependent_ = ScenarioDependent::YES; // optional at construction
};

} // namespace Antares::Solver::ObjectModel
