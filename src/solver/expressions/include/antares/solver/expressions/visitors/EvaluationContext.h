#pragma once

#include <map>
#include <string>
#include <vector>

namespace Antares::Solver::Modeler::Api
{
class ILinearProblemData;
}

namespace Antares::Solver::Visitors
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
    // EvaluationContext() = default;
    /**
     * @brief Constructs an evaluation context with the specified parameter and variable
     * values.
     *
     * @param constant_parameters parameter values.
     * @param variables variable values.
     */
    explicit EvaluationContext(std::map<std::string, std::string> system_parameters,
                               std::map<std::string, double> variables,
                               Modeler::Api::ILinearProblemData& data);

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
    double getSystemParameterValueAsDouble(const std::string& key) const;
    std::string getSystemParameterValue(const std::string& key) const;

    std::vector<double> getParameterValue(const std::string& key,
                                          const std::string& scenarioGroup,
                                          const unsigned scenario) const;
    double getParameterValue(const std::string& key,
                             const std::string& scenarioGroup,
                             const unsigned scenario,
                             unsigned int hour) const;

private:
    /**
     * @brief A map storing parameter values.
     */
    std::map<std::string, std::string> system_parameters_;

    /**
     * @brief A map storing variable values.
     */
    std::map<std::string, double> variables_;
    Modeler::Api::ILinearProblemData& data_;
};

} // namespace Antares::Solver::Visitors
