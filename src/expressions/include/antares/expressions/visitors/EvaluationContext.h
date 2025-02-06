#pragma once

#include <map>
#include <string>
#include <vector>

namespace Antares::Expressions::Visitors
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
     */
    explicit EvaluationContext(std::map<std::string, std::vector<double>> parameters,
                               std::map<std::string, std::vector<double>> variables);

    /**
     * @brief Retrieves the value of a variable.
     *
     * @param name The name of the variable.
     * @return The value of the variable.
     * @throws std::out_of_range If the variable is not found.
     */
    std::vector<double> getVariableValue(const std::string& key) const;

    /**
     * @brief Retrieves the value of a parameter.
     *
     * @param name The name of the parameter.
     * @return The value of the parameter.
     * @throws std::out_of_range If the parameter is not found.
     */
    std::vector<double> getParameterValue(const std::string& key) const;
    [[nodiscard]] unsigned int numberOfTimesteps() const;

private:
    unsigned int number_timesteps_ = 0;

private:
    /**
     * @brief A map storing parameter values.
     */
    std::map<std::string, std::vector<double>> parameters_;
    /**
     * @brief A map storing variable values.
     */
    std::map<std::string, std::vector<double>> variables_;
};

} // namespace Antares::Expressions::Visitors
