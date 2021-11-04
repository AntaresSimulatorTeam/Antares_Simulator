#include "problem.h"

#include <fstream>
#include <regex>
#include <algorithm>
#include <stdexcept>

using namespace operations_research;

namespace Antares
{
namespace Optimization
{
InfeasibleProblemAnalysis::InfeasibleProblemAnalysis(PROBLEME_SIMPLEXE_NOMME* ProbSpx,
                                                     const std::string& pattern) :
 mPattern(pattern)
{
    mSolver = std::unique_ptr<MPSolver>(convert_to_MPSolver(ProbSpx));
}

void InfeasibleProblemAnalysis::addSlackVariables()
{
    // We assess that less than 1 every 3 constraint will match
    // the regex. If more, push_back may force the copy of memory blocks.
    // This should not happen in most cases.
    mSlackVariables.reserve(mSolver->NumConstraints() / 3);
    std::regex rgx(mPattern);
    const double infinity = MPSolver::infinity();
    for (MPConstraint* constraint : mSolver->constraints())
    {
        if (std::regex_match(constraint->name(), rgx))
        {
            if (constraint->lb() != -infinity)
            {
                MPVariable* slack;
                slack = mSolver->MakeNumVar(0, infinity, constraint->name() + "::low");
                constraint->SetCoefficient(slack, 1.);
                mSlackVariables.push_back(slack);
            }

            if (constraint->ub() != infinity)
            {
                MPVariable* slack;
                slack = mSolver->MakeNumVar(0, infinity, constraint->name() + "::up");
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

InfeasibleProblemReport InfeasibleProblemAnalysis::produceReport(
  std::size_t nbSlackVariablesInReport)
{
    addSlackVariables();
    if (mSlackVariables.empty())
        throw std::logic_error(
          "Cannot generate infeasibility report: no constraints have been selected");
    buildObjective();
    const MPSolver::ResultStatus s = Solve();
    if (s != MPSolver::OPTIMAL && s != MPSolver::FEASIBLE)
        throw std::domain_error(
          "Linear problem could not be solved, and infeasibility analysis could not help");

    InfeasibleProblemReport r;
    for (const MPVariable* slack : mSlackVariables)
    {
        r.append(slack->name(), slack->solution_value());
    }

    r.trimTo(nbSlackVariablesInReport);
    return r;
}
} // namespace Optimization
} // namespace Antares
