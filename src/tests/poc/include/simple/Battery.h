#pragma once

#include <utility>

#include "vector"
#include "antares/optim/api/LinearProblemFiller.h"

using namespace Antares::optim::api;
using namespace std;

class Battery : public LinearProblemFiller
{
private:
    string id_;
    double maxP_;
    double maxStock_;
    vector<string> pVarNames;
    vector<string> stockVarNames;

public:
    Battery(string id, double maxP, double maxStock) :
     id_(std::move(id)),
     maxP_(maxP),
     maxStock_(maxStock){

     };
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
};

void Battery::addVariables(LinearProblem& problem,
                           const LinearProblemData& data,
                           const BuildContext& ctx)
{
    auto timestamps = data.getTimeStamps();
    pVarNames.reserve(timestamps.size());
    stockVarNames.reserve(timestamps.size());
    for (auto ts : timestamps)
    {
        string pVarName = "P_" + id_ + "_" + to_string(ts);
        problem.addNumVariable(pVarName, -maxP_, maxP_);
        // - charge
        // + décharge
        pVarNames.push_back(pVarName);

        string stockVarName = "Stock_" + id_ + "_" + to_string(ts);
        problem.addNumVariable(stockVarName, 0, maxStock_);
        stockVarNames.push_back(stockVarName);
    }
}

void Battery::addConstraints(LinearProblem& problem,
                             const LinearProblemData& data,
                             const BuildContext& ctx)
{
    if (!data.hasScalarData("initialStock_" + id_))
    {
        throw;
    }
    double initialStock = data.getScalarData("initialStock_" + id_);
    for (auto ts : data.getTimeStamps())
    {
        auto p = &problem.getVariable(pVarNames[ts]);
        auto e = &problem.getVariable(stockVarNames[ts]);

        // Stock(t) = Stock(t-T) - T/60 * P(t)
        auto stockConstraint = &problem.addConstraint("Stock_constr_" + to_string(ts), 0, 0);
        stockConstraint->SetCoefficient(e, 1);
        stockConstraint->SetCoefficient(p, data.getTimeResolutionInMinutes() * 1.0 / 60.0);
        if (ts > 0)
        {
            auto previousE = &problem.getVariable(stockVarNames[ts - 1]);
            stockConstraint->SetCoefficient(previousE, -1);
        }
        else
        {
            stockConstraint->SetLB(initialStock);
            stockConstraint->SetUB(initialStock);
        }
    }
}

void Battery::addObjective(Antares::optim::api::LinearProblem& problem,
                           const LinearProblemData& data,
                           const BuildContext& ctx)
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
