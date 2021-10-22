#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../utils/ortools_utils.h"

#include "report.h"

namespace Antares
{
namespace Optimization
{
class InfeasibleProblemDiag
{
public:
    InfeasibleProblemDiag() = delete;
    InfeasibleProblemDiag(PROBLEME_SIMPLEXE* ProbSpx, const std::string& pattern);
    InfeasibleProblemReport produceReport();

protected:
    std::unique_ptr<operations_research::MPSolver> mSolver;
    std::vector<operations_research::MPVariable*> mSlackVariables;

private:
    void buildObjective();
    operations_research::MPSolver::ResultStatus Solve();
    void addSlackVariables();
    const std::string mPattern;
};
} // namespace Optimization
} // namespace Antares
