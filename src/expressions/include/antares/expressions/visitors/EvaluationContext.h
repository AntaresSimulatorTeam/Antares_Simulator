#pragma once

#include <map>
#include <string>
#include <vector>

namespace Antares::Expressions::Visitors
{

struct ComponentParameter
{
    std::string id;
    std::string type;
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
     * @brief Default constructor, creates an evaluation context without parameter and variable
     * values.
     */
    EvaluationContext() = default;
    /**
     * @brief Constructs an evaluation context with the specified parameter and variable
     * values.
     *
     * @param parameters parameter values.
     * @param variables variable values.
     * @param number_timesteps
     */
    explicit EvaluationContext(std::map<std::string, ComponentParameter> parameters,
                               std::map<std::string, double> variables,
                               const std::vector<unsigned int>& timesteps/*,
                               Optimisation::LinearProblemApi::ILinearProblemData& data*/);

    /**
     * @brief Retrieves the value of a variable.
     *
     * @param name The name of the variable.
     * @return The value of the variable.
     * @throws std::out_of_range If the variable is not found.
     */
    double getVariableValue(const std::string& key) const;

    /**
     * @brief Retrieves the value of a parameter.
     *
     * @param name The name of the parameter.
     * @return The value of the parameter.
     * @throws std::out_of_range If the parameter is not found.
     */
    ComponentParameter getParameterValue(const std::string& key) const;
    [[nodiscard]] unsigned int numberOfTimesteps() const;
    [[nodiscard]] std::vector<unsigned int> getTimesteps() const;

private:
    std::vector<unsigned int> timesteps_;

    /**
     * @brief A map storing parameter values.
     */
    std::map<std::string, ComponentParameter> parameters_;
    /**
     * @brief A map storing variable values.
     */
    std::map<std::string, double> variables_;
};

} // namespace Antares::Expressions::Visitors
