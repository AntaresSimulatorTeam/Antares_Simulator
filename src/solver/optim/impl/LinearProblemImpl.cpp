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


MPVariable& LinearProblemImpl::addNumVariable(string name, double lb, double ub)
{
    // TODO: OR-Tools does not seem to care if you try to add two different variables / constraints with the same name
    // This is pretty dangerous, so we have to forbid it ourselves
    return *mpSolver->MakeNumVar(lb, ub, name);
}

MPVariable& LinearProblemImpl::addIntVariable(string name, double lb, double ub)
{
    // TODO: OR-Tools does not seem to care if you try to add two different variables / constraints with the same name
    // This is pretty dangerous, so we have to forbid it ourselves
    return *mpSolver->MakeIntVar(lb, ub, name);
}

MPVariable& LinearProblemImpl::getVariable(string name)
{
    return *mpSolver->LookupVariableOrNull(name);
}

MPConstraint& LinearProblemImpl::addConstraint(string name, double lb, double ub)
{
    // TODO: OR-Tools does not seem to care if you try to add two different variables / constraints with the same name
    // This is pretty dangerous, so we have to forbid it ourselves
    return *mpSolver->MakeRowConstraint(lb, ub, name);
}

MPConstraint& LinearProblemImpl::addBalanceConstraint(string name, double bound, string nodeName, int timestep)
{
    // TODO: OR-Tools does not seem to care if you try to add two different variables / constraints with the same name
    // This is pretty dangerous, so we have to forbid it ourselves
    // TODO: log ignored arguments
    return this->addConstraint(name, bound, bound);
}

MPConstraint& LinearProblemImpl::getConstraint(string name)
{
    MPConstraint* cnt = mpSolver->LookupConstraintOrNull(name);
    // TODO Better error handling (logs, etc.)
    if (!cnt)
        throw;
    return *cnt;
}

void LinearProblemImpl::setObjectiveCoefficient(const MPVariable& variable, double coefficient)
{
    mpSolver->MutableObjective()->SetCoefficient(&variable, coefficient);
}

void LinearProblemImpl::setMinimization(bool isMinim)
{
    isMinim ? mpSolver->MutableObjective()->SetMinimization() : mpSolver->MutableObjective()->SetMaximization();
}

MipSolution LinearProblemImpl::solve(const operations_research::MPSolverParameters& param)
{
    mpSolver->EnableOutput();
    auto status = mpSolver->Solve(param);
    // TODO remove this
    // std::string str;
    // mpSolver->ExportModelAsLpFormat(false, &str);
    map<string, double> solution;
    for (auto var : mpSolver->variables())
    {
        solution.insert({var->name(), var->solution_value()});
    }
    return {status, solution};
}

LinearProblemImpl::~LinearProblemImpl() = default;
