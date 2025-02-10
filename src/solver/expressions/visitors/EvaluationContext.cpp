#include <antares/solver/expressions/visitors/EvaluationContext.h>
#include <antares/solver/modeler/api/ILinearProblemData.h>

namespace Antares::Solver::Visitors
{
EvaluationContext::EvaluationContext(std::map<std::string, double> constant_parameters,
                                     std::map<std::string, double> variables,
                                     Modeler::Api::ILinearProblemData& data):
    constant_parameters_(std::move(constant_parameters)),
    variables_(std::move(variables)),
    data_(data)
{
}

double EvaluationContext::getVariableValue(const std::string& key) const
{
    return variables_.at(key);
}

double EvaluationContext::getConstantParameterValue(const std::string& key) const
{
    return constant_parameters_.at(key);
}

std::vector<double> EvaluationContext::getParameterValue(const std::string& key,
                                            const std::string& scenarioGroup,
                                            const unsigned scenario) const
{
    return data_.getData(key, scenarioGroup, scenario);
}
} // namespace Antares::Solver::Visitors
