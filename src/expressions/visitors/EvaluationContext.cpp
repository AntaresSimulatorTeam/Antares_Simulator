#include <antares/expressions/visitors/EvaluationContext.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>

namespace Antares::Expressions::Visitors
{
EvaluationContext::EvaluationContext(std::map<std::string, ContextParameter> system_parameters,
                                     std::map<std::string, double> variables,
                                     Optimisation::LinearProblemApi::ILinearProblemData& data):
    system_parameters_(std::move(system_parameters)),
    variables_(std::move(variables)),
    data_(data)
{
}

double EvaluationContext::getVariableValue(const std::string& key) const
{
    return variables_.at(key);
}

double EvaluationContext::getSystemParameterValueAsDouble(const std::string& key) const
{
    return std::stod(system_parameters_.at(key).value);
}

std::string EvaluationContext::getSystemParameterValue(const std::string& key) const
{
    return system_parameters_.at(key).value;
}

std::vector<double> EvaluationContext::getParameterValue(const std::string& key,
                                                         const std::string& scenarioGroup,
                                                         const unsigned scenario) const
{
    return data_.getData(system_parameters_.at(key).value, scenarioGroup, scenario);
}

double EvaluationContext::getParameterValue(const std::string& key,
                                            const std::string& scenarioGroup,
                                            const unsigned scenario,
                                            unsigned int hour) const
{
    return data_.getData(system_parameters_.at(key).value, scenarioGroup, scenario, hour);
}

ParameterType EvaluationContext::getParameterType(const std::string& key) const
{
    return system_parameters_.at(key).type;
}

ContextParameter EvaluationContext::getParameter(const std::string& key) const
{
    return system_parameters_.at(key);
}
} // namespace Antares::Expressions::Visitors
