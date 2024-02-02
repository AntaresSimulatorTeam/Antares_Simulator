//
// Created by mitripet on 26/01/24.
//

#include "LegacyLinearProblemImpl.h"
#include "ortools/linear_solver/linear_solver.h"
#include "utils/include/antares/solver/utils/ortools_utils.h"
using namespace Antares::optim::api;
using namespace std;

LegacyLinearProblemImpl::LegacyLinearProblemImpl(const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* legacyProblem,
                                                 const std::string& solverName)
{
    this->mpSolver = MPSolverFactory(legacyProblem, solverName);
    Antares::Optimization::ProblemSimplexeNommeConverter converter(solverName, legacyProblem);
    converter.TuneSolverSpecificOptions(this->mpSolver);
}

MPVariable* LegacyLinearProblemImpl::addNumVariable(string name, double lb, double ub)
{
    return mpSolver->MakeNumVar(lb, ub, name);
}

MPVariable* LegacyLinearProblemImpl::addIntVariable(string name, double lb, double ub)
{
    return mpSolver->MakeIntVar(lb, ub, name);
}

MPVariable* LegacyLinearProblemImpl::getVariable(string name)
{
    return mpSolver->LookupVariableOrNull(name);
}

MPConstraint* LegacyLinearProblemImpl::addConstraint(string name, double lb, double ub, string legacyBalanceConstraintNodeName)
{
    if (!legacyBalanceConstraintNodeName.empty() && balanceConstraintPerNodeName.contains(legacyBalanceConstraintNodeName)) {
        // add new name declared by filler to list of aliases of the existing constraint
        balanceConstraintPerNodeName.at(legacyBalanceConstraintNodeName).second.push_back(name);
        // return the existing constraint
        return balanceConstraintPerNodeName.at(legacyBalanceConstraintNodeName).first;
    }
    MPConstraint *constraint = mpSolver->MakeRowConstraint(lb, ub, name);
    balanceConstraintPerNodeName.insert({legacyBalanceConstraintNodeName, {constraint, {name}}});
    return constraint;
}

MPConstraint* LegacyLinearProblemImpl::getConstraint(string name)
{
    return mpSolver->LookupConstraintOrNull(name);
}

void LegacyLinearProblemImpl::setObjectiveCoefficient(MPVariable* variable, double coefficient)
{
    mpSolver->MutableObjective()->SetCoefficient(variable, coefficient);
}

void LegacyLinearProblemImpl::setMinimization(bool isMinim)
{
    isMinim ? mpSolver->MutableObjective()->SetMinimization() : mpSolver->MutableObjective()->SetMaximization();
}

MipSolution LegacyLinearProblemImpl::solve()
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

void LegacyLinearProblemImpl::declareBalanceConstraint(const string& nodeName, MPConstraint *constraint)
{
    if (balanceConstraintPerNodeName.contains(nodeName)) {
        // TODO : throw
    } else {
        auto name = constraint->name();
        balanceConstraintPerNodeName.insert({nodeName, {constraint, {name}}});
    }
}

