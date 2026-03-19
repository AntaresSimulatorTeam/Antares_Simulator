// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include <antares/modeler-optimisation-container/EvaluationContext.h>
#include "antares/exception/RuntimeError.hpp"
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include "antares/optimisation/linear-problem-api/IScenario.h"

using namespace Antares::Optimisation::LinearProblemApi;

namespace Antares::Optimisation
{
EvaluationContext::EvaluationContext(const ModelerStudy::SystemModel::Component* component,
                                     const ILinearProblemData* data,
                                     const IScenario* scenario):
    component_(component),
    data_(data),
    scenario_(scenario)
{
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
    const auto& parameters_types_and_values = component_->getParameterValues();
    const auto it = parameters_types_and_values.find(key);
    if (it == parameters_types_and_values.end())
    {
        throw CouldNotEvaluateConstantParameter<std::out_of_range>(
          "Parameter '" + key + "' not found in system parameters.");
    }
    return convertToDouble(key, it->second.value);
}

std::string EvaluationContext::getSystemParameterValue(const std::string& key) const
{
    const auto& parameters_types_and_values = component_->getParameterValues();
    return parameters_types_and_values.at(key).value;
}

double EvaluationContext::getParameterValue(const std::string& key,
                                            unsigned int year,
                                            unsigned int hour) const
{
    IScenario::TimeSeriesNumber time_series_number = scenario_->getData(year);
    const auto& parameters_types_and_values = component_->getParameterValues();
    return data_->getData(parameters_types_and_values.at(key).value, time_series_number, hour);
}

std::span<const double> EvaluationContext::getParameterValue(const std::string& key,
                                                             unsigned int year,
                                                             unsigned int firstHour,
                                                             unsigned int lastHour) const
{
    IScenario::TimeSeriesNumber time_series_number = scenario_->getData(year);
    const auto& parameters_types_and_values = component_->getParameterValues();
    return data_->getData(parameters_types_and_values.at(key).value,
                          time_series_number,
                          firstHour,
                          lastHour);
}

Optimisation::VariabilityType EvaluationContext::getParameterType(const std::string& key) const
{
    const auto& parameters_types_and_values = component_->getParameterValues();
    return parameters_types_and_values.at(key).type;
}

ModelerStudy::SystemModel::ParameterTypeAndValue EvaluationContext::getParameter(
  const std::string& key) const
{
    const auto& parameters_types_and_values = component_->getParameterValues();
    return parameters_types_and_values.at(key);
}

const ILinearProblemData& EvaluationContext::data() const
{
    if (!data_)
    {
        throw Error::RuntimeError("EvaluationContext::data() called with null data pointer");
    }
    return *data_;
}

const Optimisation::LinearProblemApi::IScenario& EvaluationContext::scenario() const
{
    if (!scenario_)
    {
        throw Error::RuntimeError("EvaluationContext::scenario() called with null scenario pointer");
    }
    return *scenario_;
}
} // namespace Antares::Optimisation
