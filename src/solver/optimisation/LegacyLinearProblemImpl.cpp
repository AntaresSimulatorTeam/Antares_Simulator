//
// Created by mitripet on 26/01/24.
//

#include "LegacyLinearProblemImpl.h"
#include "ortools/linear_solver/linear_solver.h"
#include "utils/include/antares/solver/utils/ortools_utils.h"

using namespace Antares::optim::api;
using namespace std;

LegacyLinearProblemImpl::LegacyLinearProblemImpl(const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME *legacyProblem,
                                                 const std::string &solverName) : LinearProblemImpl()
{
    this->mpSolver = MPSolverFactory(legacyProblem, solverName);
    Antares::Optimization::ProblemSimplexeNommeConverter converter(solverName, legacyProblem);
    converter.TuneSolverSpecificOptions(this->mpSolver);
    mpSolver->MutableObjective()->SetMinimization();
}

MPConstraint& LegacyLinearProblemImpl::addBalanceConstraint(string name, double lb, double ub, string nodeName, int timestep)
{
    string nodeWithTs = nodeName + "_" + to_string(timestep);
    if (balanceConstraintPerNodeName.contains(nodeWithTs)) {
        // add new name declared by filler to list of aliases of the existing constraint
        balanceConstraintPerNodeName.at(nodeWithTs).second.push_back(name);
        // return the existing constraint
        // TODO : vérifier aussi que LB/UB sont les mêmes ? ou forcer lb=ub ? comment gérer la conso ?
        return *balanceConstraintPerNodeName.at(nodeWithTs).first;
    }
    MPConstraint *constraint = mpSolver->MakeRowConstraint(lb, ub, name);
    balanceConstraintPerNodeName.insert({nodeWithTs, {constraint, {name}}});
    return *constraint;
}

void LegacyLinearProblemImpl::setMinimization(bool isMinim)
{
    if (!isMinim) {
        // TODO
        throw;
    }
}

void LegacyLinearProblemImpl::declareBalanceConstraint(const string &nodeName, MPConstraint *constraint)
{
    if (balanceConstraintPerNodeName.contains(nodeName)) {
        // TODO
        throw;
    } else {
        auto name = constraint->name();
        balanceConstraintPerNodeName.insert({nodeName, {constraint, {name}}});
    }
}

