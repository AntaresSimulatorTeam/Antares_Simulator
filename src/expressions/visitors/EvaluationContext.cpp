#include <antares/expressions/visitors/EvaluationContext.h>

namespace Antares::Expressions::Visitors
{
EvaluationContext::EvaluationContext(std::map<std::string, std::vector<double>> parameters,
                                     std::map<std::string, std::vector<double>> variables):
    number_timesteps_(
      parameters.empty()
        ? 0
        : parameters.begin()
            ->second.size()), // assume that all parameters have the same timestep size
                              // TODO check non-emptiness of parameters
    parameters_(std::move(parameters)),
    variables_(std::move(variables))

{
}

std::vector<double> EvaluationContext::getVariableValue(const std::string& key) const
{
    return variables_.at(key);
}

std::vector<double> EvaluationContext::getParameterValue(const std::string& key) const
{
    return parameters_.at(key);
}

[[nodiscard]] unsigned int EvaluationContext::numberOfTimesteps() const
{
    return number_timesteps_;
}
} // namespace Antares::Expressions::Visitors
