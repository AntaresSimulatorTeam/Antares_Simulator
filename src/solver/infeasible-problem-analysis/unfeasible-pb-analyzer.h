#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../utils/ortools_utils.h"
#include "unfeasibility-analysis.h"

namespace Antares::Optimization
{

/*!
 * In charge of anayzing the possible reasons for the unfeasibility of an optimization problem.
 *
 * The analyzer relies on the execution of possibly multiple UnfeasibilityAnalysis.
 */
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