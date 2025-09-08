#pragma once

#include <map>
#include <stdexcept>
#include <string>

namespace Antares::Optimisation::LinearProblemApi
{
class IScenario;
class ILinearProblemData;
} // namespace Antares::Optimisation::LinearProblemApi

namespace Antares::Expressions::Visitors
{
enum class ParameterType : unsigned int
{
    CONSTANT = 0,
    TIMESERIE = 1
    // TODO: add varying_in_scenario_only, varying_in_time_and_scenario, and handle them in visitors
};

// this struct contains more or less the same infos as the one in system.h
struct ParameterTypeAndValue
{
    std::string id;
    ParameterType type;
    std::string value;
};

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
     * @param constant_parameters parameter values.
     * @param variables variable values.
     */
    explicit EvaluationContext(std::map<std::string, ParameterTypeAndValue> system_parameters,
                               std::map<std::string, double> variables,
                               const Optimisation::LinearProblemApi::ILinearProblemData& data,
                               const Optimisation::LinearProblemApi::IScenario& scenario);

    /**
     * @brief Retrieves the value of a variable.
     *
     * @param name The name of the variable.
     * @return The value of the variable.
     * @throws std::out_of_range If the variable is not found.
     */
    [[nodiscard]] double getVariableValue(const std::string& key) const;

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

    [[nodiscard]] ParameterType getParameterType(const std::string& key) const;

    [[nodiscard]] ParameterTypeAndValue getParameter(const std::string& key) const;

    [[nodiscard]] const Optimisation::LinearProblemApi::ILinearProblemData& data() const;

    [[nodiscard]] const Optimisation::LinearProblemApi::IScenario& scenario() const;

    template<class T>
    struct CouldNotEvaluateConstantParameter: T
    {
        using T::T;
    };

private:
    /**
     * @brief A map storing parameter values.
     */
    std::map<std::string, ParameterTypeAndValue> parameters_types_and_values_;

    /**
     * @brief A map storing variable values.
     */
    std::map<std::string, double> variables_;
    const Optimisation::LinearProblemApi::ILinearProblemData& data_;
    const Optimisation::LinearProblemApi::IScenario& scenario_;
};
} // namespace Antares::Expressions::Visitors
