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
    string nodeName_;
    vector<shared_ptr<Battery>> batteries_; // sera remplacé par la notion de ports
    vector<shared_ptr<Thermal>> thermals_;  // sera remplacé par la notion de ports
public:
    Balance(string nodeName,
            vector<shared_ptr<Battery>> batteries,
            vector<shared_ptr<Thermal>> thermals) :
     nodeName_(std::move(nodeName)),
     batteries_(std::move(batteries)),
     thermals_(std::move(thermals)){};
    void addVariables(LinearProblem& problem, const LinearProblemData::YearView& data) override;
    void addConstraints(LinearProblem& problem, const LinearProblemData::YearView& data) override;
    void addObjective(LinearProblem& problem, const LinearProblemData::YearView& data) override;
    void update(LinearProblem& problem, const LinearProblemData::YearView& data) override;
};

void Balance::addVariables(LinearProblem& problem, const LinearProblemData::YearView& data)
{
    // nothing to do
}

void Balance::addConstraints(LinearProblem& problem, const LinearProblemData::YearView& data)
{
    if (!data.hasTimedData("consumption_" + nodeName_))
    {
        throw;
    }
    auto consumption = data.getTimedData("consumption_" + nodeName_);
    // <!> IMPORTANT : we have to use the convention -production = -consumption, in order to be
    // compatible with the legacy code's balance constraint
    for (auto ts : data.getTimeStamps())
    {
        auto balanceConstraint = &problem.addBalanceConstraint(
          "Balance_" + nodeName_ + "_" + to_string(ts), -consumption[ts], nodeName_, ts);

        for (const auto& battery : batteries_)
        {
            auto p = &problem.getVariable(battery->getPVarName(ts));
            balanceConstraint->SetCoefficient(p, -1);
        }
        for (const auto& thermal : thermals_)
        {
            auto p = &problem.getVariable(thermal->getPVarName(ts));
            balanceConstraint->SetCoefficient(p, -1);
        }
    }
}

void Balance::addObjective(Antares::optim::api::LinearProblem& problem,
                           const LinearProblemData::YearView& data)
{
    // nothing to do
}

void Balance::update(Antares::optim::api::LinearProblem& problem,
                     const LinearProblemData::YearView& data)
{
    if (!data.hasTimedData("consumption_" + nodeName_))
    {
        throw;
    }
    auto consumption = data.getTimedData("consumption_" + nodeName_);
    for (auto ts : data.getTimeStamps())
    {
        auto balanceConstraint
          = &problem.getConstraint("Balance_" + nodeName_ + "_" + to_string(ts));
        balanceConstraint->SetLB(consumption[ts]);
        balanceConstraint->SetUB(consumption[ts]);
    }
}
