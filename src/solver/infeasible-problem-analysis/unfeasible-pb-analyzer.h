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
    explicit UnfeasiblePbAnalyzer(std::vector<std::unique_ptr<UnfeasibilityAnalysis>> analysisList);
    void run(MPSolver* problem);
    void printReport() const;

private:
    std::vector<std::unique_ptr<UnfeasibilityAnalysis>> analysisList_;
};

std::unique_ptr<UnfeasiblePbAnalyzer> makeUnfeasiblePbAnalyzer();

} // namespace Antares::Optimization