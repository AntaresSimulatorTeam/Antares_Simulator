#pragma once

#include <map>
#include <span>
#include <stdexcept>
#include <string>

#include "antares/study/system-model/component.h"

namespace Antares::Optimisation::LinearProblemApi
{
class IScenario;
class ILinearProblemData;
} // namespace Antares::Optimisation::LinearProblemApi

namespace Antares::Optimisation
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
                               const Optimisation::LinearProblemApi::ILinearProblemData* data,
                               const Optimisation::LinearProblemApi::IScenario* scenario);

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

    [[nodiscard]] ModelerStudy::SystemModel::ParameterType getParameterType(
      const std::string& key) const;

    [[nodiscard]] ModelerStudy::SystemModel::ParameterTypeAndValue getParameter(
      const std::string& key) const;

    [[nodiscard]] const Optimisation::LinearProblemApi::ILinearProblemData& data() const;

    [[nodiscard]] const Optimisation::LinearProblemApi::IScenario& scenario() const;

    template<class T>
    struct CouldNotEvaluateConstantParameter: T
    {
        using T::T;
    };

private:
    const ModelerStudy::SystemModel::Component* component_;
    const Optimisation::LinearProblemApi::ILinearProblemData* data_;
    const Optimisation::LinearProblemApi::IScenario* scenario_;
};
} // namespace Antares::Optimisation
