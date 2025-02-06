#include <antares/expressions/visitors/EvaluationContext.h>

namespace Antares::Expressions::Visitors
{
EvaluationContext::EvaluationContext(std::map<std::string, double> parameters,
                                     std::map<std::string, double> variables):
    parameters_(std::move(parameters)),
    variables_(std::move(variables))
{
}

double EvaluationContext::getVariableValue(const std::string& key) const
{
    return variables_.at(key);
}

double EvaluationContext::getParameterValue(const std::string& key) const
{
    return parameters_.at(key);
}
} // namespace Antares::Expressions::Visitors
