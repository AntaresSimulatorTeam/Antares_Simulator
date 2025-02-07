#include <antares/expressions/visitors/EvaluationContext.h>
#include "antares/expressions/nodes/ParameterNode.h"

#include "../../optimisation/linear-problem-api/include/antares/optimisation/linear-problem-api/ILinearProblemData.h"

namespace Antares::Expressions::Visitors
{
EvaluationContext::EvaluationContext(std::map<std::string, ComponentParameter> parameters,
                                     std::map<std::string, double> variables,
                                     const std::vector<unsigned int>& timesteps,
                                     Optimisation::LinearProblemApi::ILinearProblemData& data):
    timesteps_(timesteps),
    // TODO check non-emptiness of parameters
    parameters_(std::move(parameters)),
    variables_(std::move(variables)),
    data_(data)

{
}

double EvaluationContext::getVariableValue(const std::string& key) const
{
    return variables_.at(key);
}

std::vector<double> EvaluationContext::getParameterValue(
  const Nodes::ParameterNode* parameter_node) const
{
    const auto& parameter_value = parameters_.at(parameter_node->value()).value;
    if (parameter_node->timeIndex() == TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO)
    {
        return std::vector(timesteps_.size(), std::stod(parameter_value));
    }
    // TODO
    return data_.getData(parameter_value, "group 1", 0);
}

[[nodiscard]] unsigned int EvaluationContext::numberOfTimesteps() const
{
    return timesteps_.size();
}

[[nodiscard]] std::vector<unsigned int> EvaluationContext::getTimesteps() const
{
    return timesteps_;
}
} // namespace Antares::Expressions::Visitors
