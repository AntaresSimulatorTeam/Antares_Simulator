#pragma once

#include <utility>

#include "antares/optim/api/LinearProblemFiller.h"
#include "Thermal.h"

using namespace Antares::optim::api;
using namespace std;

class ProductionPriceMinimization : public LinearProblemFiller
{
private:
    vector<shared_ptr<Thermal>> thermals_; // sera remplacé par la notion de ports
public:
    explicit ProductionPriceMinimization(vector<shared_ptr<Thermal>> thermals) :
     thermals_(std::move(thermals)){};
    void addVariables(LinearProblem& problem, const LinearProblemData::YearView& data) override;
    void addConstraints(LinearProblem& problem, const LinearProblemData::YearView& data) override;
    void addObjective(LinearProblem& problem, const LinearProblemData::YearView& data) override;
    void update(LinearProblem& problem, const LinearProblemData::YearView& data) override;
};

void ProductionPriceMinimization::addVariables(LinearProblem& problem,
                                               const LinearProblemData::YearView& data)
{
    // nothing to do
}

void ProductionPriceMinimization::addConstraints(LinearProblem& problem,
                                                 const LinearProblemData::YearView& data)
{
    // nothing to do
}

void ProductionPriceMinimization::addObjective(Antares::optim::api::LinearProblem& problem,
                                               const LinearProblemData::YearView& data)
{
    problem.setMinimization(true);
    for (auto ts : data.getTimeStamps())
    {
        for (const auto& thermal : thermals_)
        {
            auto* p = &problem.getVariable(thermal->getPVarName(ts));
            problem.setObjectiveCoefficient(*p, thermal->getPCost(ts));
        }
    }
}

void ProductionPriceMinimization::update(Antares::optim::api::LinearProblem& problem,
                                         const LinearProblemData::YearView& data)
{
    // nothing to do
}
