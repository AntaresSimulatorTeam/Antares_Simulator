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
    const auto it = system_parameters_.find(key);
    if (it == system_parameters_.end())
    {
        throw CouldNotEvaluateConstantParameter<std::out_of_range>(
          "Parameter '" + key + "' not found in system parameters.");
    }
    return convertToDouble(key, it->second.value);
}

std::string EvaluationContext::getSystemParameterValue(const std::string& key) const
{
    return system_parameters_.at(key).value;
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
