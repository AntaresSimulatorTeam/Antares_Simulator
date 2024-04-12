#pragma once

#include "vector"
#include "antares/optim/api/LinearProblemFiller.h"
#include "Component.h"
#include "PortConnection.h"

using namespace Antares::optim::api;
using namespace std;

class ComponentFiller : public LinearProblemFiller
{
private:
    PortConnectionsManager* portConnectionsManager_;
    Component component_;
    [[nodiscard]] map<string, double> getPortPin(string name,
                                                 int timestamp,
                                                 const LinearProblemData& linearProblemData,
                                                 BuildContext::ScenarioID scenario) const;

public:
    ComponentFiller(Component component, PortConnectionsManager& portConnectionsManager) :
     portConnectionsManager_(&portConnectionsManager), component_(std::move(component))
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
};

void ComponentFiller::addVariables(LinearProblem& problem,
                                   const LinearProblemData& data,
                                   const BuildContext& ctx)
{
    // TODO : remplacer par des AST
    for (auto scenario : ctx.getScenarios())
    {
        if (component_.getModel() == THERMAL)
        {
            for (auto ts : ctx.getTimeStamps())
            {
                string pVarName
                  = "P_" + component_.getId() + "_" + to_string(ts) + "_" + to_string(scenario);
                double maxP = component_.getDoubleParameterValue("maxP");
                problem.addNumVariable(pVarName, 0, maxP);
            }
        }
        else if (component_.getModel() == BATTERY)
        {
            auto timestamps = ctx.getTimeStamps();
            for (auto ts : timestamps)
            {
                // P < 0 : charge
                // P > 0 : décharge
                string pVarName
                  = "P_" + component_.getId() + "_" + to_string(ts) + "_" + to_string(scenario);
                double maxP = component_.getDoubleParameterValue("maxP");
                problem.addNumVariable(pVarName, -maxP, maxP);

                string stockVarName
                  = "Stock_" + component_.getId() + "_" + to_string(ts) + "_" + to_string(scenario);
                double maxStock = component_.getDoubleParameterValue("maxStock");
                problem.addNumVariable(stockVarName, 0, maxStock);
            }
        }
    }
}

void ComponentFiller::addConstraints(LinearProblem& problem,
                                     const LinearProblemData& data,
                                     const BuildContext& ctx)
{
    // TODO : remplacer par des AST
    if (component_.getModel() == BATTERY)
    {
        if (!data.hasScalarData("initialStock_" + component_.getId()))
        {
            throw;
        }

        for (auto scenario : ctx.getScenarios())
        {
            double initialStock
              = data.getScalarData("initialStock_" + component_.getId(), scenario);
            for (auto ts : ctx.getTimeStamps())
            {
                string pVarName
                  = "P_" + component_.getId() + "_" + to_string(ts) + "_" + to_string(scenario);
                auto p = &problem.getVariable(pVarName);
                string stockVarName
                  = "Stock_" + component_.getId() + "_" + to_string(ts) + "_" + to_string(scenario);
                auto e = &problem.getVariable(stockVarName);

                // Stock(t) = Stock(t-T) - T/60 * P(t)
                auto stockConstraint
                  = &problem.addConstraint("Stock_constr_" + to_string(ts), 0, 0);
                stockConstraint->SetCoefficient(e, 1);
                stockConstraint->SetCoefficient(p, data.getTimeResolutionInMinutes() * 1.0 / 60.0);
                if (ts > 0)
                {
                    string stockVarNameTm1 = "Stock_" + component_.getId() + "_" + to_string(ts - 1)
                                             + "_" + to_string(scenario);
                    auto previousE = &problem.getVariable(stockVarNameTm1);
                    stockConstraint->SetCoefficient(previousE, -1);
                }
                else
                {
                    stockConstraint->SetLB(initialStock);
                    stockConstraint->SetUB(initialStock);
                }
            }
        }
    }
    else if (component_.getModel() == BALANCE)
    {
        string nodeName = component_.getStringParameterValue("nodeName");
        if (!data.hasTimedData("consumption_" + nodeName))
        {
            throw;
        }
        for (auto scenario : ctx.getScenarios())
        {
            auto consumption = data.getTimedData("consumption_" + nodeName, scenario);
            for (auto ts : ctx.getTimeStamps())
            {
                // <!> IMPORTANT : we have to use the convention -production = -consumption, in
                // order to be compatible with the legacy code's balance constraint
                auto balanceConstraint = &problem.addBalanceConstraint(
                  "Balance_" + nodeName + "_" + to_string(ts) + "_" + to_string(scenario),
                  -consumption[ts],
                  nodeName,
                  ts);
                for (const auto& connection : portConnectionsManager_->getConectionsTo(this, "P"))
                {
                    for (const auto& varAndCoeff : connection.componentFiller->getPortPin(
                           connection.portName, ts, data, scenario))
                    {
                        auto p = &problem.getVariable(varAndCoeff.first);
                        balanceConstraint->SetCoefficient(p, varAndCoeff.second * -1.0);
                    }
                }
            }
        }
    }
}

void ComponentFiller::addObjective(Antares::optim::api::LinearProblem& problem,
                                   const LinearProblemData& data,
                                   const BuildContext& ctx)
{
    // TODO : remplacer par des AST
    if (component_.getModel() == PRICE_MINIM)
    {
        problem.setMinimization(true);
        for (auto scenario : ctx.getScenarios())
        {
            for (auto ts : ctx.getTimeStamps())
            {
                for (const auto& connection :
                     portConnectionsManager_->getConectionsTo(this, "cost"))
                {
                    for (const auto& varAndCoeff : connection.componentFiller->getPortPin(
                           connection.portName, ts, data, scenario))
                    {
                        auto variable = &problem.getVariable(varAndCoeff.first);
                        problem.setObjectiveCoefficient(*variable, varAndCoeff.second);
                    }
                }
            }
        }
    }
}

void ComponentFiller::update(Antares::optim::api::LinearProblem& problem,
                             const LinearProblemData& data)
{
    // ?
}

map<string, double> ComponentFiller::getPortPin(string name,
                                                int timestamp,
                                                const LinearProblemData& data,
                                                BuildContext::ScenarioID scenario) const
{
    // TODO : remplacer par des AST
    if (component_.getModel() == THERMAL)
    {
        string pVarName
          = "P_" + component_.getId() + "_" + to_string(timestamp) + "_" + to_string(scenario);
        if (name == "P")
        {
            return {{pVarName, 1.0}};
        }
        else if (name == "cost")
        {
            if (!data.hasTimedData("cost_" + component_.getId()))
            {
                throw;
            }
            // TODO BuildContext
            double cost = data.getTimedData("cost_" + component_.getId(), scenario).at(timestamp);
            return {{pVarName, cost}};
        }
    }
    else if (component_.getModel() == BATTERY)
    {
        if (name == "P")
        {
            string pVarName
              = "P_" + component_.getId() + "_" + to_string(timestamp) + "_" + to_string(scenario);
            return {{pVarName, 1.0}};
        }
    }
    return {};
}
