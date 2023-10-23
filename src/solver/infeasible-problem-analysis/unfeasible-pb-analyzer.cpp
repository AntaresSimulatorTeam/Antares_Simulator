#include <fstream>
#include <algorithm>

#include "unfeasible-pb-analyzer.h"
#include "variables-bounds-consistency.h"
#include "constraint-slack-analysis.h"
#include <antares/logs/logs.h>


using namespace operations_research;

namespace Antares::Optimization
{

UnfeasiblePbAnalyzer::UnfeasiblePbAnalyzer(MPSolver* problem,
                                           std::vector<std::shared_ptr<UnfeasibilityAnalysis>> analysisList)
        : problem_(problem), analysisList_(analysisList)
{}

void UnfeasiblePbAnalyzer::run()
{
    logs.info();
    logs.info() << "Solver: Starting unfeasibility analysis...";

    for (auto& analysis : analysisList_)
    {
        logs.info();
        logs.info() << analysis->title() << " : running...";
        analysis->run(problem_);
        if (analysis->hasDetectedInfeasibilityCause())
            return;

        logs.notice() << analysis->title() << " : nothing detected.";
    }
}

void UnfeasiblePbAnalyzer::printReport()
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