#pragma once

#include "vector"
#include "antares/optim/api/LinearProblemFiller.h"

using namespace Antares::optim::api;
using namespace std;

class Model
{
    vector<string> parameters;
    vector<string> variableDefinitions;
    vector<string> constraintDefinitions;
};
class Component
{
    Model model;
    map<string, double> parameterValues;
};
class PortConnexion
{
    string componentAndPortId1;
    string componentAndPortId2;
};
class PortConnexionsManager
{
    vector<PortConnexion> connexions;
};

class ComponentFiller : public LinearProblemFiller
{
private:
    PortConnexionsManager* portConnexionsManager_;
    Component* component_;

public:
    ComponentFiller(Component &component, PortConnexionsManager &portConnexionsManager) :
            component_(&component), portConnexionsManager_(&portConnexionsManager)
    {}

    void addVariables(LinearProblem &problem, const LinearProblemData &data) override;

    void addConstraints(LinearProblem &problem, const LinearProblemData &data) override;

    void addObjective(LinearProblem &problem, const LinearProblemData &data) override;

    void update(LinearProblem &problem, const LinearProblemData &data) override;
};

void ComponentFiller::addVariables(LinearProblem& problem, const LinearProblemData& data)
{
    // parse variables definition using component_->model and component_->parameterValues
    // call problem.addNumVariable(...) or problem.addIntVariable(...)
}

void ComponentFiller::addConstraints(LinearProblem& problem, const LinearProblemData& data)
{
    // parse constraint definition using component_->model and component_->parameterValues
    // call problem.addConstraint(...) or problem.addBalanceConstraint(...)
}

void ComponentFiller::addObjective(Antares::optim::api::LinearProblem& problem, const LinearProblemData& data)
{
    // parse ..
}

void ComponentFiller::update(Antares::optim::api::LinearProblem& problem, const LinearProblemData& data)
{
    // ?
}
