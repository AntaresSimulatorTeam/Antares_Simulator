#pragma once

#include <string>
#include <vector>
#include <map>

#include "constraint.h"
#include "ortools/linear_solver/linear_solver.h"

namespace Antares
{
namespace Optimization
{
class InfeasibleProblemReport
{
public:
    InfeasibleProblemReport() = delete;
    explicit InfeasibleProblemReport(
      const std::vector<const operations_research::MPVariable*>& slackVariables);
    void prettyPrint();

private:
    void turnSlackVarsIntoConstraints(const std::vector<const operations_research::MPVariable*>& slackVariables);
    void sortConstraints();
    void trimConstraints();
    void extractItems();
    void logSuspiciousConstraints();

    std::vector<Constraint> mConstraints;
    std::map<ConstraintType, unsigned int> mTypes;
    const unsigned int nbVariables = 10;
};
} // namespace Optimization
} // namespace Antares
