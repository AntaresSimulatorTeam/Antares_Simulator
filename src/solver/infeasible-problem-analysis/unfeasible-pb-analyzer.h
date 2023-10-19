#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../utils/ortools_utils.h"
#include "unfeasibility-analysis.h"

namespace Antares::Optimization
{

class UnfeasiblePbAnalyzer
{
public:
    UnfeasiblePbAnalyzer() = delete;
    explicit UnfeasiblePbAnalyzer(const std::string& solverName, const PROBLEME_SIMPLEXE_NOMME* ProbSpx);
    void run();
    void printReport();

private:
    std::shared_ptr<operations_research::MPSolver> problem_;
    std::vector<std::unique_ptr<UnfeasibilityAnalysis>> analysisList_;
};
} // namespace Antares::Optimization