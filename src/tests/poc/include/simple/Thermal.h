#pragma once

#include <utility>

#include "vector"
#include "antares/optim/api/LinearProblemFiller.h"

using namespace Antares::optim::api;
using namespace std;

class Thermal : public LinearProblemFiller
{
private:
    string id_;
    double maxP_;
    vector<double> pCost_; // TODO : put in LinearProblemData ?
    vector<string> pVarNames;

public:
    Thermal(string id, double maxP) : id_(std::move(id)), maxP_(maxP)
    {
    }
    void addVariables(LinearProblem& problem,
                      const LinearProblemData& data,
                      const BuildContext& ctx) override;
    void addConstraints(LinearProblem& problem,
                        const LinearProblemData& data,
                        const BuildContext& ctx) override;
    void addObjective(LinearProblem& problem,
                      const LinearProblemData& data,
                      const BuildContext& ctx) override;
    void update(LinearProblem& problem, const LinearProblemData& data) override;

    string getPVarName(int ts); // sera remplacé par la notion de ports
    double getPCost(int ts);    // sera remplacé par la notion de ports
};

void Thermal::addVariables(LinearProblem& problem,
                           const LinearProblemData& data,
                           const BuildContext& ctx)
{
    pVarNames.reserve(data.getTimeStamps().size());
    for (auto ts : data.getTimeStamps())
    {
        string pVarName = "P_" + id_ + "_" + to_string(ts);
        problem.addNumVariable(pVarName, 0, maxP_);
        pVarNames.push_back(pVarName);
    }
    // keep cost data for later (will be replaced with ports)
    if (!data.hasTimedData("cost_" + id_))
    {
        throw;
    }
    pCost_ = ctx.getTimedData("cost_" + id_);
}

void Thermal::addConstraints(LinearProblem& problem,
                             const LinearProblemData& data,
                             const BuildContext& ctx)
{
    // nothing to do
}

void Thermal::addObjective(Antares::optim::api::LinearProblem& problem,
                           const LinearProblemData& data,
                           const BuildContext& ctx)
{
    // nothing to do
}

void Thermal::update(Antares::optim::api::LinearProblem& problem, const LinearProblemData& data)
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
