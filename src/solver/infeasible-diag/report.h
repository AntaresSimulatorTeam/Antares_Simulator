#pragma once

#include <string>
#include <vector>

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
    void printLargest(unsigned int n);

private:
    std::vector<SlackSolution> mConstraints;
};

} // namespace Optimization
} // namespace Antares
