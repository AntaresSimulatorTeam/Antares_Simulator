

#include "vector"
#include "optim/api/LinearProblemFiller.h"

using namespace Antares::optim::api;
using namespace std;

class Thermal : public LinearProblemFiller
{
private:
    std::vector<int> timeSteps_;
    double maxP_;
    vector<double> pCost_; // TODO : put in LinearProblemData ?
    vector<string> pVarNames;
public:
    Thermal(std::vector<int> timeSteps,         double maxP,   vector<double> pCost) :
            timeSteps_(timeSteps), maxP_(maxP), pCost_(pCost) {};

    void addVariables(LinearProblem* problem, LinearProblemData* data) override;
    void addConstraints(LinearProblem* problem, LinearProblemData* data) override;
    void addObjective(LinearProblem* problem, LinearProblemData* data) override;
    void update(LinearProblem* problem, LinearProblemData* data) override;
    string getPVarName(int ts);
    double getPCost(int ts);
};

void Thermal::addVariables(LinearProblem *problem, LinearProblemData *data)
{
    for (auto ts : timeSteps_) {
        string pVarName = "P_thermal_" + to_string(ts);
        problem->addNumVariable(pVarName, 0, maxP_);
        pVarNames.push_back(pVarName);
    }
}

void Thermal::addConstraints(LinearProblem *problem, LinearProblemData *data)
{
    // nothing to do
}

void Thermal::addObjective(Antares::optim::api::LinearProblem *problem, Antares::optim::api::LinearProblemData *data)
{
    // nothing to do
}

void Thermal::update(Antares::optim::api::LinearProblem *problem, Antares::optim::api::LinearProblemData *data)
{
    // nothing to do
}

string Thermal::getPVarName(int ts)
{
    return pVarNames[ts];
}

double Thermal::getPCost(int ts)
{
    return pCost_[ts];
}

