// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
