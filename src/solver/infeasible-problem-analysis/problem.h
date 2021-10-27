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
class InfeasibleProblemAnalysis
{
public:
    InfeasibleProblemAnalysis() = delete;
    InfeasibleProblemAnalysis(PROBLEME_SIMPLEXE_NOMME* ProbSpx, const std::string& pattern);
    InfeasibleProblemReport produceReport(std::size_t nbSlackVariablesInReport);

private:
    void buildObjective() const;
    void addSlackVariables();
    operations_research::MPSolver::ResultStatus Solve() const;

    std::unique_ptr<operations_research::MPSolver> mSolver;
    std::vector<operations_research::MPVariable*> mSlackVariables;
    const std::string mPattern;
};
} // namespace Optimization
} // namespace Antares
