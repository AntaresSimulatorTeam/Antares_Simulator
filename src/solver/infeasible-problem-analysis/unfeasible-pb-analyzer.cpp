// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/infeasible-problem-analysis/unfeasible-pb-analyzer.h"

#include <algorithm>
#include <fstream>

#include <antares/logs/logs.h>
#include "antares/solver/infeasible-problem-analysis/constraint-slack-analysis.h"
#include "antares/solver/infeasible-problem-analysis/variables-bounds-consistency.h"

using namespace operations_research;

namespace Antares::Optimization
{

std::unique_ptr<UnfeasiblePbAnalyzer> makeUnfeasiblePbAnalyzer()
{
    std::vector<std::unique_ptr<UnfeasibilityAnalysis>> analysisList;
    analysisList.push_back(std::make_unique<VariablesBoundsConsistency>());
    analysisList.push_back(std::make_unique<ConstraintSlackAnalysis>());

    return std::make_unique<UnfeasiblePbAnalyzer>(std::move(analysisList));
}

UnfeasiblePbAnalyzer::UnfeasiblePbAnalyzer(
  std::vector<std::unique_ptr<UnfeasibilityAnalysis>> analysisList):
    analysisList_(std::move(analysisList))
{
}

void UnfeasiblePbAnalyzer::run(MPSolver* problem)
{
    logs.info();
    logs.info() << "Solver: Starting unfeasibility analysis...";

    for (auto& analysis: analysisList_)
    {
        logs.info();
        logs.info() << analysis->title() << " : running...";
        analysis->run(problem);
        if (analysis->hasDetectedInfeasibilityCause())
        {
            return;
        }

        logs.notice() << analysis->title() << " : nothing detected.";
    }
}

void UnfeasiblePbAnalyzer::printReport() const
{
    for (auto& analysis: analysisList_)
    {
        if (analysis->hasDetectedInfeasibilityCause())
        {
            logs.info() << analysis->title() << " : printing report";
            analysis->printReport();
            return;
        }
    }
    logs.notice() << "Solver: unfeasibility analysis : could not find the cause of unfeasibility.";
}
} // namespace Antares::Optimization
