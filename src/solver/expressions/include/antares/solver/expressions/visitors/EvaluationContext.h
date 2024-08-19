#pragma once

#include <map>

namespace Antares::Solver::Visitors
{
template<class KeyType, class ValueType>
class EvaluationContext
{
public:
    EvaluationContext() = default;
    explicit EvaluationContext(std::map<KeyType, ValueType> parameters,
                               std::map<KeyType, ValueType> variables);

    ValueType getVariableValue(const KeyType& key) const
    {
        return variables_.at(key);
    }

    ValueType getParameterValue(const KeyType& key) const
    {
        return parameters_.at(key);
    }

private:
    std::map<std::string, double> parameters_;
    std::map<std::string, double> variables_;
};

} // namespace Antares::Solver::Visitors
