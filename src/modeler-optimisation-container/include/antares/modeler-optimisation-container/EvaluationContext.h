// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <span>
#include <stdexcept>
#include <string>

#include "antares/study/system-model/component.h"

namespace Antares::LinearProblem::Api
{
class IScenario;
class ILinearProblemData;
} // namespace Antares::LinearProblem::Api

namespace Antares::LinearProblem
{

/**
 * @brief Represents the context for evaluating expressions.
 *
 * Stores and provides access to parameter and variable values.
 */
class EvaluationContext
{
public:
    /**
     * @brief Constructs an evaluation context with the specified parameter and variable
     * values.
     *
     * @param component
     * @param system_parameters
     * @param constant_parameters parameter values.
     * @param variables variable values.
     */
    explicit EvaluationContext(const ModelerStudy::SystemModel::Component* component,
                               const LinearProblem::Api::ILinearProblemData* data,
                               const LinearProblem::Api::IScenario* scenario);

    /**
     * @brief Retrieves the value of a parameter.
     *
     * @param name The name of the parameter.
     * @return The value of the parameter.
     * @throws std::out_of_range If the parameter is not found.
     */
    [[nodiscard]] double getSystemParameterValueAsDouble(const std::string& key) const;

    [[nodiscard]] std::string getSystemParameterValue(const std::string& key) const;

    [[nodiscard]] double getParameterValue(const std::string& key,
                                           unsigned int year,
                                           unsigned int hour) const;
    std::span<const double> getParameterValue(const std::string& key,
                                              unsigned int year,
                                              unsigned int firstHour,
                                              unsigned int lastHour) const;

    [[nodiscard]] LinearProblem::VariabilityType getParameterType(const std::string& key) const;

    [[nodiscard]] ModelerStudy::SystemModel::ParameterTypeAndValue getParameter(
      const std::string& key) const;

    [[nodiscard]] const LinearProblem::Api::ILinearProblemData& data() const;

    [[nodiscard]] const LinearProblem::Api::IScenario& scenario() const;

    template<class T>
    struct CouldNotEvaluateConstantParameter: T
    {
        using T::T;
    };

private:
    const ModelerStudy::SystemModel::Component* component_;
    const LinearProblem::Api::ILinearProblemData* data_;
    const LinearProblem::Api::IScenario* scenario_;
};
} // namespace Antares::LinearProblem
