#pragma once

#include <utility>

#include "antares/optim/api/LinearProblemFiller.h"
#include "Battery.h"
#include "Thermal.h"

using namespace Antares::optim::api;
using namespace std;

class Balance : public LinearProblemFiller
{
private:
    vector<int>& timeSteps_;
    string nodeName_;
    vector<Battery*> &batteries_; // sera remplacé par la notion de ports
    vector<Thermal*> &thermals_; // sera remplacé par la notion de ports
    vector<double>& consumption_; // TODO : à mettre dans la structure de données LinearProblemData
public:
    Balance(vector<int>& timeSteps, string nodeName, vector<Battery*>& batteries, vector<Thermal*>& thermals, vector<double>& consumption) :
            timeSteps_(timeSteps), nodeName_(std::move(nodeName)), batteries_(batteries), thermals_(thermals), consumption_(consumption) {};
    void addVariables(LinearProblem& problem, const LinearProblemData& data) override;
    void addConstraints(LinearProblem& problem, const LinearProblemData& data) override;
    void addObjective(LinearProblem& problem, const LinearProblemData& data) override;
    void update(LinearProblem& problem, const LinearProblemData& data) override;
};

void Balance::addVariables(LinearProblem& problem, const LinearProblemData& data)
{
    // nothing to do
}

void Balance::addConstraints(LinearProblem& problem, const LinearProblemData& data)
{
    for (auto ts : timeSteps_) {
        auto balanceConstraint =
                &problem.addBalanceConstraint("Balance_" + nodeName_ + "_" + to_string(ts), consumption_[ts], consumption_[ts], nodeName_, ts);

        for (auto* battery : batteries_) {
            auto p = &problem.getVariable(battery->getPVarName(ts));
            balanceConstraint->SetCoefficient(p, 1);
        }
        for (auto* thermal : thermals_) {
            auto p = &problem.getVariable(thermal->getPVarName(ts));
            balanceConstraint->SetCoefficient(p, 1);
        }
    }
}

void Balance::addObjective(Antares::optim::api::LinearProblem& problem, const LinearProblemData& data)
{
    // nothing to do
}

void Balance::update(Antares::optim::api::LinearProblem& problem, const LinearProblemData& data)
{
    // nothing to do
}
