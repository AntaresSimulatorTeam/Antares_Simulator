#pragma once

#include <string>
#include <vector>

#include "constraint.h"

// Report class, without OR-Tools dependency in its API
// Only dependency is in the implementation

namespace Antares
{
namespace Optimization
{
class InfeasibleProblemReport
{
public:
    using SlackSolution = std::pair<std::string, double>;
    InfeasibleProblemReport() = default;
    void append(const std::string& constraintName, double value);
    void trimTo(std::size_t nbSlackVariables);
    void rawPrint() const;
    void prettyPrint();

private:
    std::vector<Constraint> mConstraints;
};
} // namespace Optimization
} // namespace Antares
