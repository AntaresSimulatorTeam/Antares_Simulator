#pragma once

#include "vector"
#include "antares/optim/api/LinearProblemFiller.h"
#include "Component.h"
#include "PortConnection.h"
#include <iostream>

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
    // TODO : replace with AST
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
        else if (component_.getModel() == BATTERY
                 || component_.getModel() == BATTERY_WITH_VARIABLE_SIZING)
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
    if (component_.getModel() == BATTERY_WITH_VARIABLE_SIZING)
    {
        string maxPVarName = "maxP_" + component_.getId();
        double maxP = component_.getDoubleParameterValue("maxP");
        problem.addNumVariable(maxPVarName, 0, maxP);

        string maxStockVarName = "maxStock_" + component_.getId();
        double maxStock = component_.getDoubleParameterValue("maxStock");
        problem.addNumVariable(maxStockVarName, 0, maxStock);
    }
}

void ComponentFiller::addConstraints(LinearProblem& problem,
                                     const LinearProblemData& data,
                                     const BuildContext& ctx)
{
    // TODO : replace with AST
    if (component_.getModel() == BATTERY || component_.getModel() == BATTERY_WITH_VARIABLE_SIZING)
    {
        for (auto scenario : ctx.getScenarios())
        {
            for (auto ts : ctx.getTimeStamps())
            {
                string pVarName
                  = "P_" + component_.getId() + "_" + to_string(ts) + "_" + to_string(scenario);
                auto p = &problem.getVariable(pVarName);
                string stockVarName
                  = "Stock_" + component_.getId() + "_" + to_string(ts) + "_" + to_string(scenario);
                auto stock = &problem.getVariable(stockVarName);

                // Stock(t) = Stock(t-T) - T/60 * P(t)
                auto stockConstraint = &problem.addConstraint(
                  "Stock_constr_" + to_string(ts) + "_" + to_string(scenario), 0, 0);
                stockConstraint->SetCoefficient(stock, 1);
                stockConstraint->SetCoefficient(p, data.getTimeResolutionInMinutes() * 1.0 / 60.0);

                int previous_stock_ts;
                if (ts > 0)
                {
                    previous_stock_ts = ts - 1;
                }
                else
                {
                    // We consider that the battery has to cycle, ie Stock(0-T) = Stock(final t)
                    previous_stock_ts = ctx.getTimeStamps()[ctx.getTimeStamps().size() - 1];
                }
                string stockVarNameTm1 = "Stock_" + component_.getId() + "_"
                                         + to_string(previous_stock_ts) + "_" + to_string(scenario);
                auto previousE = &problem.getVariable(stockVarNameTm1);
                stockConstraint->SetCoefficient(previousE, -1);
                if (component_.getModel() == BATTERY_WITH_VARIABLE_SIZING)
                {
                    // Sizing constraints
                    auto maxP = &problem.getVariable("maxP_" + component_.getId());
                    auto maxStock = &problem.getVariable("maxStock_" + component_.getId());
                    // P >= -maxP
                    auto maxPConstraintLB = &problem.addConstraint(
                      "maxStock_constr_" + to_string(ts) + "_" + to_string(scenario),
                      0,
                      problem.infinity());
                    maxPConstraintLB->SetCoefficient(p, 1);
                    maxPConstraintLB->SetCoefficient(maxP, 1);
                    // P <= maxP
                    auto maxPConstraintUB = &problem.addConstraint(
                      "maxStock_constr_" + to_string(ts) + "_" + to_string(scenario),
                      0,
                      problem.infinity());
                    maxPConstraintUB->SetCoefficient(p, -1);
                    maxPConstraintUB->SetCoefficient(maxP, 1);
                    // Stock <= maxStock
                    auto maxStockConstraint = &problem.addConstraint(
                      "maxStock_constr_" + to_string(ts) + "_" + to_string(scenario),
                      0,
                      problem.infinity());
                    maxStockConstraint->SetCoefficient(stock, -1);
                    maxStockConstraint->SetCoefficient(maxStock, 1);
                }
            }
            if (data.hasScalarData("initialStock_" + component_.getId()))
            {
                // If the user has defined in the input data the initial stock strategy, force the
                // model to reach that stock at the beginning & end of the horizon
                double initialStock
                  = data.getScalarData("initialStock_" + component_.getId(), scenario);
                int final_ts = ctx.getTimeStamps()[ctx.getTimeStamps().size() - 1];
                string finalStockVarName = "Stock_" + component_.getId() + "_" + to_string(final_ts)
                                           + "_" + to_string(scenario);
                auto finalStockVar = &problem.getVariable(finalStockVarName);
                finalStockVar->SetLB(initialStock);
                finalStockVar->SetUB(initialStock);
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
                    for (const auto& [varName, coeff] : connection.componentFiller->getPortPin(
                           connection.portName, ts, data, scenario))
                    {
                        auto p = &problem.getVariable(varName);
                        balanceConstraint->SetCoefficient(p, coeff * -1.0);
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
    // TODO : replace with AST
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
                    for (const auto& [varName, coeff] : connection.componentFiller->getPortPin(
                           connection.portName, ts, data, scenario))
                    {
                        auto variable = &problem.getVariable(varName);
                        problem.setObjectiveCoefficient(
                          *variable,
                          coeff / ctx.getScenarios().size()
                            + problem.getObjectiveCoefficient(*variable));
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
    // TODO : replace with AST
    const std::string pVarName
      = "P_" + component_.getId() + "_" + to_string(timestamp) + "_" + to_string(scenario);

    // P BATTERY/THERMAL
    if (name == "P")
    {
        return {{pVarName, 1.0}};
    }

    // cost THERMAL
    if (name == "cost")
    {
        if (component_.getModel() == BATTERY_WITH_VARIABLE_SIZING)
        {
            std::string maxPVarName = "maxP_" + component_.getId();
            std::string maxStockVarName = "maxStock_" + component_.getId();

            // TODO : not very clean since scenario is ignored, which works because
            // the PRICE_MINIM filler does not add costs for the scenarios
            double maxPLifeTimeCost = component_.getDoubleParameterValue("maxPLifeTimeCost");
            double maxStockLifeTimeCost
              = component_.getDoubleParameterValue("maxStockLifeTimeCost");
            double lifeTimeInYears = component_.getDoubleParameterValue("lifeTimeInYears");
            double nTsInLifeTime
              = lifeTimeInYears * 365.25 * 1440 / (1.0 * data.getTimeResolutionInMinutes());
            double maxPCostPerTs = maxPLifeTimeCost / nTsInLifeTime;
            double maxStockCostPerTs = maxStockLifeTimeCost / nTsInLifeTime;

            return {{maxPVarName, maxPCostPerTs}, {maxStockVarName, maxStockCostPerTs}};
            // TODO: we can add variable cost on P here too, instead of if/else
        }
        else
        {
            if (!data.hasTimedData("cost_" + component_.getId()))
            {
                return {};
            }
            // TODO BuildContext
            double cost = data.getTimedData("cost_" + component_.getId(), scenario).at(timestamp);
            return {{pVarName, cost}};
        }
    }

    return {};
}
