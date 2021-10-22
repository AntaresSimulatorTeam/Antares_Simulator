#include "problem.h"

#include <fstream>
#include <regex>
#include <algorithm>
#include <cassert>

using namespace operations_research;

namespace Antares
{
namespace Optimization
{
// TODO : use solver provided by the user ?
InfeasibleProblemDiag::InfeasibleProblemDiag(PROBLEME_SIMPLEXE* ProbSpx,
                                             const std::string& pattern) :
 mPattern(pattern)
{
    mSolver = convert_to_MPSolver(ProbSpx);
}

void InfeasibleProblemDiag::addSlackVariables()
{
    std::regex rgx(mPattern);
    const double infinity = MPSolver::infinity();
    for (auto constraint : mSolver->constraints())
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
    for (MPVariable* variable : mSolver->variables())
    {
        objective->SetCoefficient(variable, 0.);
    }
    // Only slack variables have a non-zero cost
    for (MPVariable* slack : mSlackVariables)
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
    assert(mSlackVariables.size() > 0);
    Solve();

    InfeasibleProblemReport r;
    for (MPVariable* slack : mSlackVariables)
    {
        const double v = slack->solution_value();
        r.append(slack->name(), v);
    }
    return r;
}

InfeasibleProblemDiag::~InfeasibleProblemDiag()
{
    delete mSolver;
}
} // namespace Optimization
} // namespace Antares
