//
// Created by mitripet on 26/01/24.
//

#include "include/antares/optim/impl/LinearProblemImpl.h"
#include "ortools/linear_solver/linear_solver.h"
#include "antares/solver/utils/ortools_utils.h"

using namespace Antares::optim::api;
using namespace std;

LinearProblemImpl::LinearProblemImpl() = default;

LinearProblemImpl::LinearProblemImpl(bool isMip, const std::string& solverName)
{
    this->mpSolver = MPSolverFactory(isMip, solverName);
}


MPVariable* LinearProblemImpl::addNumVariable(string name, double lb, double ub)
{

    // TODO attention ortools ne fail pas si on ajoute une même variable plusieurs fois => à faire en dehors ?
    return mpSolver->MakeNumVar(lb, ub, name);
}

MPVariable* LinearProblemImpl::addIntVariable(string name, double lb, double ub)
{
    return mpSolver->MakeIntVar(lb, ub, name);
}

MPVariable* LinearProblemImpl::getVariable(string name)
{
    return mpSolver->LookupVariableOrNull(name);
}

MPConstraint* LinearProblemImpl::addConstraint(string name, double lb, double ub)
{
    return mpSolver->MakeRowConstraint(lb, ub, name);
}

MPConstraint* LinearProblemImpl::addBalanceConstraint(string name, double lb, double ub, string nodeName, int timestep)
{
    // TODO : log ignored arguments ?
    return this->addConstraint(name, lb, ub);
}

MPConstraint* LinearProblemImpl::getConstraint(string name)
{
    return mpSolver->LookupConstraintOrNull(name);
}

void LinearProblemImpl::setObjectiveCoefficient(MPVariable* variable, double coefficient)
{
    mpSolver->MutableObjective()->SetCoefficient(variable, coefficient);
}

void LinearProblemImpl::setMinimization(bool isMinim)
{
    isMinim ? mpSolver->MutableObjective()->SetMinimization() : mpSolver->MutableObjective()->SetMaximization();
}

MipSolution LinearProblemImpl::solve()
{
    string lp;
    mpSolver->ExportModelAsLpFormat(false, &lp);
    auto status = mpSolver->Solve();
    map<MPVariable*, double> solution;
    for (auto var : mpSolver->variables())
    {
        solution.insert({var, var->solution_value()});
    }
    return {status, solution};
}