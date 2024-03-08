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

MPConstraint& LegacyLinearProblemImpl::addBalanceConstraint(string name, double bound, string nodeName, int timestamp)
{
    string key = getBalanceConstraintKey(nodeName, timestamp);
    if (balanceConstraintPerNodeName.contains(key)) {
        // add new name declared by filler to list of aliases of the existing constraint
        balanceConstraintPerNodeName.at(key).second.push_back(name);
        // return the existing constraint
        return *balanceConstraintPerNodeName.at(key).first;
    }
    MPConstraint *constraint = mpSolver->MakeRowConstraint(bound, bound, name);
    balanceConstraintPerNodeName.insert({key, {constraint, {name}}});
    return *constraint;
}

void LegacyLinearProblemImpl::setMinimization(bool isMinim)
{
    if (!isMinim) {
        // TODO: improve exception
        throw;
    } else {
        mpSolver->MutableObjective()->SetMinimization();
    }
}

void LegacyLinearProblemImpl::declareBalanceConstraint(const string &nodeName, int timestamp, MPConstraint *constraint)
{
    if (balanceConstraintPerNodeName.contains(nodeName)) {
        // TODO: improve exception
        throw;
    } else {
        auto name = constraint->name();
        balanceConstraintPerNodeName.insert({getBalanceConstraintKey(nodeName, timestamp), {constraint, {name}}});
    }
}

string LegacyLinearProblemImpl::getBalanceConstraintKey(const string& nodeName, int timestamp)
{
    return nodeName + "_" + to_string(timestamp);
}
