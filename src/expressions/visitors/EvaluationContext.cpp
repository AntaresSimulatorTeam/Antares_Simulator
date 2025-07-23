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

#include <antares/expressions/visitors/EvaluationContext.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include "antares/optimisation/linear-problem-api/IScenario.h"

using namespace Antares::Optimisation::LinearProblemApi;

namespace Antares::Expressions::Visitors
{
EvaluationContext::EvaluationContext(std::map<std::string, ParameterTypeAndValue> system_parameters,
                                     std::map<std::string, double> variables,
                                     const ILinearProblemData& data,
                                     const IScenario& scenario):
    parameters_types_and_values_(std::move(system_parameters)),
    variables_(std::move(variables)),
    data_(data),
    scenario_(scenario)
{
}

double EvaluationContext::getVariableValue(const std::string& key) const
{
    return variables_.at(key);
}

static double convertToDouble(const std::string& key, const std::string& value)
{
    try
    {
        return std::stod(value);
    }
    catch (const std::invalid_argument&)
    {
        throw EvaluationContext::CouldNotEvaluateConstantParameter<std::invalid_argument>(
          "Parameter '" + key + "' has an invalid numerical format: '" + value + "'.");
    }
    catch (const std::out_of_range&)
    {
        throw EvaluationContext::CouldNotEvaluateConstantParameter<std::out_of_range>(
          "Parameter '" + key + "' is out of numerical range: '" + value + "'.");
    }
}

double EvaluationContext::getSystemParameterValueAsDouble(const std::string& key) const
{
    const auto it = parameters_types_and_values_.find(key);
    if (it == parameters_types_and_values_.end())
    {
        throw CouldNotEvaluateConstantParameter<std::out_of_range>(
          "Parameter '" + key + "' not found in system parameters.");
    }
    return convertToDouble(key, it->second.value);
}

std::string EvaluationContext::getSystemParameterValue(const std::string& key) const
{
    return parameters_types_and_values_.at(key).value;
}

double EvaluationContext::getParameterValue(const std::string& key,
                                            unsigned int year,
                                            unsigned int hour) const
{
    IScenario::TimeSeriesNumber time_series_number = scenario_.getData(year);
    return data_.getData(parameters_types_and_values_.at(key).value, time_series_number, hour);
}

ParameterType EvaluationContext::getParameterType(const std::string& key) const
{
    return parameters_types_and_values_.at(key).type;
}

ParameterTypeAndValue EvaluationContext::getParameter(const std::string& key) const
{
    return parameters_types_and_values_.at(key);
}

const ILinearProblemData& EvaluationContext::data() const
{
    return data_;
}

const Optimisation::LinearProblemApi::IScenario& EvaluationContext::scenario() const
{
    return scenario_;
}
} // namespace Antares::Expressions::Visitors
