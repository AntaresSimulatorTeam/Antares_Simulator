#include <fstream>
#include <algorithm>

#include "unfeasible-pb-analyzer.h"
#include "variables-bounds-consistency.h"
#include "constraint-slack-analysis.h"
#include <antares/logs/logs.h>


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

UnfeasiblePbAnalyzer::UnfeasiblePbAnalyzer(std::vector<std::unique_ptr<UnfeasibilityAnalysis>> analysisList)
          : analysisList_(std::move(analysisList))
{}

void UnfeasiblePbAnalyzer::run(MPSolver* problem)
{
    logs.info();
    logs.info() << "Solver: Starting unfeasibility analysis...";

    for (auto& analysis : analysisList_)
    {
        logs.info();
        logs.info() << analysis->title() << " : running...";
        analysis->run(problem);
        if (analysis->hasDetectedInfeasibilityCause())
            return;

        logs.notice() << analysis->title() << " : nothing detected.";
    }
}

void UnfeasiblePbAnalyzer::printReport() const
{
    for (auto& analysis : analysisList_)
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