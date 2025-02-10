#include <antares/solver/expressions/visitors/EvaluationContext.h>
#include <antares/solver/modeler/api/ILinearProblemData.h>

#include "../../modeler/api/include/antares/solver/modeler/api/ILinearProblemData.h"

namespace Antares::Solver::Visitors
{
EvaluationContext::EvaluationContext(std::map<std::string, std::string> system_parameters,
                                     std::map<std::string, double> variables,
                                     Modeler::Api::ILinearProblemData& data):
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
    return std::stod(system_parameters_.at(key));
}

std::string EvaluationContext::getSystemParameterValue(const std::string& key) const
{
    return system_parameters_.at(key);
}

std::vector<double> EvaluationContext::getParameterValue(const std::string& key,
                                                         const std::string& scenarioGroup,
                                                         const unsigned scenario) const
{
    return data_.getData(system_parameters_.at(key), scenarioGroup, scenario);
}

double EvaluationContext::getParameterValue(const std::string& key,
                                            const std::string& scenarioGroup,
                                            const unsigned scenario,
                                            unsigned int hour) const
{
    return data_.getData(key, scenarioGroup, scenario, hour);
}
} // namespace Antares::Solver::Visitors
