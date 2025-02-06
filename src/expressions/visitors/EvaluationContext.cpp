#include <antares/expressions/visitors/EvaluationContext.h>

namespace Antares::Expressions::Visitors
{
EvaluationContext::EvaluationContext(std::map<std::string, ComponentParameter> parameters,
                                     std::map<std::string, double> variables,
                                     unsigned int number_timesteps):
    number_timesteps_(number_timesteps),
    // TODO check non-emptiness of parameters
    parameters_(std::move(parameters)),
    variables_(std::move(variables))

{
}

double EvaluationContext::getVariableValue(const std::string& key) const
{
    return variables_.at(key);
}

ComponentParameter EvaluationContext::getParameterValue(const std::string& key) const
{
    return parameters_.at(key);
}

[[nodiscard]] unsigned int EvaluationContext::numberOfTimesteps() const
{
    return number_timesteps_;
}
} // namespace Antares::Expressions::Visitors
