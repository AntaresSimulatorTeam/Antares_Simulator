#include <fstream>
#include <algorithm>

#include "unfeasible-pb-analyzer.h"
#include "variables-bounds-consistency.h"
#include "constraint-slack-analysis.h"
#include <antares/logs/logs.h>


using namespace operations_research;

namespace Antares::Optimization
{

UnfeasiblePbAnalyzer::UnfeasiblePbAnalyzer(const std::string& solverName, const PROBLEME_SIMPLEXE_NOMME* ProbSpx)
{
    // gp : Here we have a dependency on PROBLEME_SIMPLEXE_NOMME and MPSolver.
    // gp : We should Convert() the PROBLEME_SIMPLEXE_NOMME into a MPSolver outside (and before) this constructor.
    // gp : And so we should have a MPSolver* passed here.
    // gp : It would be easier to test this class in isolation.
    problem_ = std::shared_ptr<MPSolver>(ProblemSimplexeNommeConverter(solverName, ProbSpx).Convert());

    analysisList_.push_back(std::make_unique<VariablesBoundsConsistency>(problem_));
    analysisList_.push_back(std::make_unique<ConstraintSlackAnalysis>(problem_));
}

void UnfeasiblePbAnalyzer::run()
{
    logs.info();
    logs.info() << "Solver: Starting unfeasibility analysis...";

    for (auto& analysis : analysisList_)
    {
        logs.info();
        logs.info() << analysis->title() << " : running...";
        analysis->run();
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