// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

#include "timeAndScenarioType.h"
#include "valueType.h"

namespace Antares::ModelerStudy::SystemModel
{

/**
 * A parameter of the model: a parameter is mainly defined by a name and expected type.
 * When the model is instantiated as a component, a value must be provided for
 * parameters, either as constant values or timeseries-based values.
 */
class Parameter final
{
public:
    explicit Parameter(std::string id,
                       TimeDependent timeDependent,
                       ScenarioDependent scenarioDependent):
        id_(std::move(id)),
        timeDependent_(timeDependent),
        scenarioDependent_(scenarioDependent)
    {
    }

    const std::string& Id() const
    {
        return id_;
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
    std::string id_;
    TimeDependent timeDependent_ = TimeDependent::YES;             // optional at construction
    ScenarioDependent scenarioDependent_ = ScenarioDependent::YES; // optional at construction
};

} // namespace Antares::ModelerStudy::SystemModel
