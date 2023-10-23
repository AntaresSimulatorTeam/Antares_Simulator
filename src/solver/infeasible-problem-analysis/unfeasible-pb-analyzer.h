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
    explicit UnfeasiblePbAnalyzer(std::vector<std::shared_ptr<UnfeasibilityAnalysis>> analysisList);
    void run(MPSolver* problem);
    void printReport();

private:
    std::vector<std::shared_ptr<UnfeasibilityAnalysis>> analysisList_;
};
} // namespace Antares::Optimization