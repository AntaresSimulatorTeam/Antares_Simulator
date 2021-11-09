#include "problem.h"
#include "exceptions.h"

#include <fstream>
#include <regex>
#include <algorithm>

using namespace operations_research;

namespace Antares
{
namespace Optimization
{
InfeasibleProblemAnalysis::InfeasibleProblemAnalysis(const PROBLEME_SIMPLEXE_NOMME* ProbSpx)
{
    mSolver = std::unique_ptr<MPSolver>(convert_to_MPSolver(ProbSpx));
}

void InfeasibleProblemAnalysis::addSlackVariables()
{
    /* Optimization:
       We assess that less than 1 every 3 constraint will match
       the regex. If more, push_back may force the copy of memory blocks.
       This should not happen in most cases.
    */
    const unsigned int selectedConstraintsInverseRatio = 3;
    mSlackVariables.reserve(mSolver->NumConstraints() / selectedConstraintsInverseRatio);
    std::regex rgx(mPattern);
    const double infinity = MPSolver::infinity();
    for (MPConstraint* constraint : mSolver->constraints())
    {
        if (std::regex_match(constraint->name(), rgx))
        {
            if (constraint->lb() != -infinity)
            {
                const MPVariable* slack
                  = mSolver->MakeNumVar(0, infinity, constraint->name() + "::low");
                constraint->SetCoefficient(slack, 1.);
                mSlackVariables.push_back(slack);
            }

            if (constraint->ub() != infinity)
            {
                const MPVariable* slack
                  = mSolver->MakeNumVar(0, infinity, constraint->name() + "::up");
                constraint->SetCoefficient(slack, -1.);
                mSlackVariables.push_back(slack);
            }
        }
    }
}

void InfeasibleProblemAnalysis::buildObjective() const
{
    MPObjective* objective = mSolver->MutableObjective();
    // Reset objective function
    objective->Clear();
    // Only slack variables have a non-zero cost
    for (const MPVariable* slack : mSlackVariables)
    {
        objective->SetCoefficient(slack, 1.);
    }
    objective->SetMinimization();
}

MPSolver::ResultStatus InfeasibleProblemAnalysis::Solve() const
{
    return mSolver->Solve();
}

InfeasibleProblemReport InfeasibleProblemAnalysis::produceReport()
{
    addSlackVariables();
    if (mSlackVariables.empty())
    {
        throw SlackVariablesEmpty(
          "Cannot generate infeasibility report: no constraints have been selected");
    }
    buildObjective();
    const MPSolver::ResultStatus status = Solve();
    if ((status != MPSolver::OPTIMAL) && (status != MPSolver::FEASIBLE))
    {
        throw ProblemResolutionFailed(
          "Linear problem could not be solved, and infeasibility analysis could not help");
    }
    return InfeasibleProblemReport(mSlackVariables);
}
} // namespace Optimization
} // namespace Antares
