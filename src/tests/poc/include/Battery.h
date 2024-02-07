#pragma once

#include "vector"
#include "antares/optim/api/LinearProblemFiller.h"

using namespace Antares::optim::api;
using namespace std;

class Battery : public LinearProblemFiller
{
private:
    std::vector<int>& timeSteps_;
    int timeStepInMinutes_;
    double maxP_;
    double maxE_;
    double initialE_;
    vector<string> pVarNames;
    vector<string> eVarNames;
public:
    Battery(std::vector<int> &timeSteps, int timeStepInMinutes, double maxP, double maxE, double initialE) :
            timeSteps_(timeSteps), timeStepInMinutes_(timeStepInMinutes), maxP_(maxP), maxE_(maxE), initialE_(initialE)
    {
        pVarNames.reserve(timeSteps.size());
        eVarNames.reserve(timeSteps.size());
    };
    void addVariables(LinearProblem& problem, const LinearProblemData& data) override;
    void addConstraints(LinearProblem& problem, const LinearProblemData& data) override;
    void addObjective(LinearProblem& problem, const LinearProblemData& data) override;
    void update(LinearProblem& problem, const LinearProblemData& data) override;
    string getPVarName(int ts); // sera remplacé par la notion de ports
};

void Battery::addVariables(LinearProblem& problem, const LinearProblemData& data)
{
    for (auto ts : timeSteps_) {
        string pVarName = "P_batt_" + to_string(ts);
        problem.addNumVariable(pVarName, -maxP_, maxP_);
        // - charge
        // + décharge
        pVarNames.push_back(pVarName);

        string eVarName = "E_batt_" + to_string(ts);
        problem.addNumVariable(eVarName, 0, maxE_);
        eVarNames.push_back(eVarName);
    }
}

void Battery::addConstraints(LinearProblem& problem, const LinearProblemData& data)
{
    for (auto ts : timeSteps_) {
        auto p = &problem.getVariable(pVarNames[ts]);
        auto e = &problem.getVariable(eVarNames[ts]);

        // E(t) = E(t-T) - T/60 * P(t)
        auto stockConstraint = &problem.addConstraint("E_constr_" + to_string(ts), 0, 0);
        stockConstraint->SetCoefficient(e, 1);
        stockConstraint->SetCoefficient(p, timeStepInMinutes_ * 1.0 / 60.0);
        if (ts > 0)
        {
            auto previousE = &problem.getVariable(eVarNames[ts - 1]);
            stockConstraint->SetCoefficient(previousE, -1);
        }
        else
        {
            stockConstraint->SetLB(initialE_);
            stockConstraint->SetUB(initialE_);
        }
    }
}

void Battery::addObjective(Antares::optim::api::LinearProblem& problem, const LinearProblemData& data)
{
    // nothing to do
}

void Battery::update(Antares::optim::api::LinearProblem& problem, const LinearProblemData& data)
{
    // nothing to do
}

string Battery::getPVarName(int ts)
{
    return pVarNames[ts];
}
