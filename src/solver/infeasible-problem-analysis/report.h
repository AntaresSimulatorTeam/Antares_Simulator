#pragma once

#include <string>
#include <vector>

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
    InfeasibleProblemReport(const std::vector<const operations_research::MPVariable*>&,
                            std::size_t);
    void prettyPrint();

private:
    void trim();
    void append(const std::string& constraintName, double value);
    std::vector<Constraint> mConstraints;
    const unsigned int nbVariables = 10;
};
} // namespace Optimization
} // namespace Antares
