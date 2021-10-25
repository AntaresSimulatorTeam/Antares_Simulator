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
InfeasibleProblemDiag::InfeasibleProblemDiag(PROBLEME_SIMPLEXE* ProbSpx,
                                             const std::string& pattern) :
 mPattern(pattern)
{
    mSolver = std::unique_ptr<MPSolver>(convert_to_MPSolver(ProbSpx));
}

void InfeasibleProblemDiag::addSlackVariables()
{
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

void InfeasibleProblemDiag::buildObjective()
{
    MPObjective* objective = mSolver->MutableObjective();
    // Reset objective function
    for (const MPVariable* variable : mSolver->variables())
    {
        objective->SetCoefficient(variable, 0.);
    }
    // Only slack variables have a non-zero cost
    for (const MPVariable* slack : mSlackVariables)
    {
        objective->SetCoefficient(slack, 1.);
    }
    objective->SetMinimization();
}

MPSolver::ResultStatus InfeasibleProblemDiag::Solve()
{
    return mSolver->Solve();
}

InfeasibleProblemReport InfeasibleProblemDiag::produceReport()
{
    addSlackVariables();
    buildObjective();
    if (mSlackVariables.size() == 0)
        throw std::runtime_error(
          "No constraints have been pre-selected. Cannot generate infeasibility report.");
    Solve();

    InfeasibleProblemReport r;
    for (const MPVariable* slack : mSlackVariables)
    {
        r.append(slack->name(), slack->solution_value());
    }
    return r;
}
} // namespace Optimization
} // namespace Antares
