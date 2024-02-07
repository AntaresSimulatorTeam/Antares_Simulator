#pragma once

#include "antares/optim/api/LinearProblemFiller.h"
#include "Thermal.h"

using namespace Antares::optim::api;
using namespace std;

class ProductionPriceMinimization : public LinearProblemFiller
{
private:
    vector<int>& timeSteps_;
    vector<Thermal*> &thermals_; // sera remplacé par la notion de ports
public:
    ProductionPriceMinimization(vector<int>& timeSteps, vector<Thermal*>& thermals) :
            timeSteps_(timeSteps), thermals_(thermals) {};
    void addVariables(LinearProblem& problem, const LinearProblemData& data) override;
    void addConstraints(LinearProblem& problem, const LinearProblemData& data) override;
    void addObjective(LinearProblem& problem, const LinearProblemData& data) override;
    void update(LinearProblem& problem, const LinearProblemData& data) override;
};

void ProductionPriceMinimization::addVariables(LinearProblem& problem, const LinearProblemData& data)
{
    // nothing to do
}

void ProductionPriceMinimization::addConstraints(LinearProblem& problem, const LinearProblemData& data)
{
    // nothing to do
}

void ProductionPriceMinimization::addObjective(Antares::optim::api::LinearProblem& problem, const LinearProblemData& data)
{
    problem.setMinimization(true);
    for (auto ts : timeSteps_) {
        for (auto* thermal : thermals_) {
            auto* p = &problem.getVariable(thermal->getPVarName(ts));
            problem.setObjectiveCoefficient(*p, thermal->getPCost(ts));
        }
    }
}

void ProductionPriceMinimization::update(Antares::optim::api::LinearProblem& problem, const LinearProblemData& data)
{
    // nothing to do
}
