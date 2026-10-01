// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include <antares/modeler-optimisation-container/EvaluationContext.h>
#include "antares/exception/RuntimeError.hpp"
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include "antares/optimisation/linear-problem-api/IScenario.h"

using namespace Antares::LinearProblem::Api;

namespace Antares::LinearProblem
{
double convertToDouble(const std::string& key, const std::string& value)
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

static IScenario::TimeSeriesNumber getTimeSeriesNumber(
  const ModelerStudy::SystemModel::ParameterTypeAndValue& parameter,
  const IScenario& scenario,
  unsigned int year)
{
    return isScenarioDependent(parameter.type) ? scenario.getData(year) : 1;
}

EvaluationContext::EvaluationContext(const ModelerStudy::SystemModel::Component* component,
                                     const ILinearProblemData* data,
                                     const IScenario* scenario):
    component_(component),
    data_(data),
    scenario_(scenario)
{
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
    const auto parameter = getParameter(key);
    const auto time_series_number = getTimeSeriesNumber(parameter, *scenario_, year);
    return data_->getData(parameter.value, time_series_number, hour);
}

std::span<const double> EvaluationContext::getParameterValue(const std::string& key,
                                                             unsigned int year,
                                                             unsigned int firstHour,
                                                             unsigned int lastHour) const
{
    const auto parameter = getParameter(key);
    const auto time_series_number = getTimeSeriesNumber(parameter, *scenario_, year);
    return data_->getData(parameter.value, time_series_number, firstHour, lastHour);
}

LinearProblem::VariabilityType EvaluationContext::getParameterType(const std::string& key) const
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

const LinearProblem::Api::IScenario& EvaluationContext::scenario() const
{
    if (!scenario_)
    {
        throw Error::RuntimeError("EvaluationContext::scenario() called with null scenario pointer");
    }
    return *scenario_;
}
} // namespace Antares::LinearProblem
