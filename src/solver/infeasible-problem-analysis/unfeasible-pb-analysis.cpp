#include <fstream>
#include <algorithm>

#include "unfeasible-pb-analysis.h"
#include "constraint-slack-analysis.h"
#include <antares/logs/logs.h>


using namespace operations_research;

namespace Antares::Optimization
{

// ============================
// Variables bounds analysis
// ============================

void VariablesBoundsConsistency::run()
{
    for (auto& var : problem_->variables())
    {
        double lowBound = var->lb();
        double upBound = var->ub();
        std::string name = var->name();
        if (lowBound > upBound)
        {
            storeIncorrectVariable(name, lowBound, upBound);
        }
    }

    if (foundIncorrectVariables())
        hasDetectedInfeasibilityCause_ = true;
}

void VariablesBoundsConsistency::storeIncorrectVariable(std::string name, double lowBound, double upBound)
{
    incorrectVars_.push_back(VariableBounds(name, lowBound, upBound));
}

bool VariablesBoundsConsistency::foundIncorrectVariables()
{
    return !incorrectVars_.empty();
}

void VariablesBoundsConsistency::printReport()
{
    for (auto& var : incorrectVars_)
    {
        logs.notice() << var.name << " : low bound = " << var.lowBound << ", up bound = " << var.upBound;
    }
}


// ===============================
// Unfeasibility analyzer 
// ===============================

// gp : this class should be renamed into UnfeasibilityAnalyzer

UnfeasiblePbAnalysis::UnfeasiblePbAnalysis(const std::string& solverName, const PROBLEME_SIMPLEXE_NOMME* ProbSpx)
{
    // gp : Here we have a dependency on PROBLEME_SIMPLEXE_NOMME and MPSolver.
    // gp : We should Convert() the PROBLEME_SIMPLEXE_NOMME into a MPSolver outside (and before) this constructor.
    // gp : And so we should have a MPSolver* passed here.
    // gp : It would be easier to test this class in isolation.
    problem_ = std::shared_ptr<MPSolver>(ProblemSimplexeNommeConverter(solverName, ProbSpx).Convert());

    analysisList_.push_back(std::make_unique<VariablesBoundsConsistency>(problem_));
    analysisList_.push_back(std::make_unique<ConstraintSlackAnalysis>(problem_));
}

void UnfeasiblePbAnalysis::run()
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

void UnfeasiblePbAnalysis::printReport()
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