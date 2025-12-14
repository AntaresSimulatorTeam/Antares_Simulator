/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include <antares/expressions/expression.h>

#include "optimConfig.h"
#include "timeAndScenarioType.h"
#include "valueType.h"

namespace Antares::ModelerStudy::SystemModel
{

/// A decision variable of the model
class Variable final
{
public:
    Variable(std::string id,
             Expression lower_bound,
             Expression upper_bound,
             ValueType type,
             TimeDependent timeDependent,
             ScenarioDependent scenarioDependent,
             Modeler::Config::Location location = Modeler::Config::Location::SUBPROBLEMS):
        id_(std::move(id)),
        type_(type),
        lowerBound_(std::move(lower_bound)),
        upperBound_(std::move(upper_bound)),
        timeDependent_(timeDependent),
        scenarioDependent_(scenarioDependent),
        location_(location)
    {
    }

    const std::string& Id() const
    {
        return id_;
    }

    ValueType Type() const
    {
        return type_;
    }

    const Expression& LowerBound() const
    {
        return lowerBound_;
    }

    const Expression& UpperBound() const
    {
        return upperBound_;
    }

    [[nodiscard]] bool isTimeDependent() const
    {
        return timeDependent_ == TimeDependent::YES;
    }

    [[nodiscard]] bool IsScenarioDependent() const
    {
        return scenarioDependent_ == ScenarioDependent::YES;
    }

    [[nodiscard]] Modeler::Config::Location location() const
    {
        return location_;
    }

private:
    std::string id_;
    ValueType type_;
    Expression lowerBound_;
    Expression upperBound_;
    TimeDependent timeDependent_ = TimeDependent::YES;
    ScenarioDependent scenarioDependent_ = ScenarioDependent::YES;
    Modeler::Config::Location location_ = Modeler::Config::Location::SUBPROBLEMS;
};

} // namespace Antares::ModelerStudy::SystemModel
