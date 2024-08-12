#pragma once

#include <map>
#include <string>

namespace Antares::Solver::Expressions
{
class EvaluationContext
{
public:
    EvaluationContext() = default;
    explicit EvaluationContext(std::map<std::string, double> parameters,
                               std::map<std::string, double> variables);

    double getVariableValue(const std::string&) const;
    double getParameterValue(const std::string&) const;

private:
    std::map<std::string, double> parameters_;
    std::map<std::string, double> variables_;
};

} // namespace Antares::Solver::Expressions
