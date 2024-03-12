#include "include/antares/optim/standard_api/ComponentFiller.h"

using namespace Antares::optim::api;
using namespace Antares::optim::standard_api;
using namespace std;

void ComponentFiller::addVariables(LinearProblem &problem, const LinearProblemData &data)
{
    // TODO : remplacer par des AST
    if (component_.getModel() == THERMAL) {
        for (auto ts: data.getTimeStamps()) {
            string pVarName = "P_" + component_.getId() + "_" + to_string(ts);
            double maxP = component_.getDoubleParameterValue("maxP");
            problem.addNumVariable(pVarName, 0, maxP);
        }
    } else if (component_.getModel() == BATTERY) {
        auto timestamps = data.getTimeStamps();
        for (auto ts: timestamps) {
            // P < 0 : charge
            // P > 0 : décharge
            string pVarName = "P_" + component_.getId() + "_" + to_string(ts);
            double maxP = component_.getDoubleParameterValue("maxP");
            problem.addNumVariable(pVarName, -maxP, maxP);

            string stockVarName = "Stock_" + component_.getId() + "_" + to_string(ts);
            double maxStock = component_.getDoubleParameterValue("maxStock");
            problem.addNumVariable(stockVarName, 0, maxStock);
        }
    }
}

void ComponentFiller::addConstraints(LinearProblem &problem, const LinearProblemData &data)
{
    // TODO : remplacer par des AST
    if (component_.getModel() == BATTERY) {
        if (!data.hasScalarData("initialStock_" + component_.getId())) {
            throw;
        }
        double initialStock = data.getScalarData("initialStock_" + component_.getId());
        for (auto ts: data.getTimeStamps()) {
            string pVarName = "P_" + component_.getId() + "_" + to_string(ts);
            auto p = &problem.getVariable(pVarName);
            string stockVarName = "Stock_" + component_.getId() + "_" + to_string(ts);
            auto e = &problem.getVariable(stockVarName);

            // Stock(t) = Stock(t-T) - T/60 * P(t)
            auto stockConstraint = &problem.addConstraint("Stock_constr_" + to_string(ts), 0, 0);
            stockConstraint->SetCoefficient(e, 1);
            stockConstraint->SetCoefficient(p, data.getTimeResolutionInMinutes() * 1.0 / 60.0);
            if (ts > 0) {
                string stockVarNameTm1 = "Stock_" + component_.getId() + "_" + to_string(ts - 1);
                auto previousE = &problem.getVariable(stockVarNameTm1);
                stockConstraint->SetCoefficient(previousE, -1);
            } else {
                stockConstraint->SetLB(initialStock);
                stockConstraint->SetUB(initialStock);
            }
        }
    } else if (component_.getModel() == BALANCE) {
        string nodeName = component_.getStringParameterValue("nodeName");
        if (!data.hasTimedData("consumption_" + nodeName)) {
            throw;
        }
        auto consumption = data.getTimedData("consumption_" + nodeName);
        for (auto ts: data.getTimeStamps()) {
            // <!> IMPORTANT : we have to use the convention -production = -consumption, in order to be compatible
            // with the legacy code's balance constraint
            auto balanceConstraint =
                    &problem.addBalanceConstraint("Balance_" + nodeName + "_" + to_string(ts), -consumption[ts],
                                                  nodeName, ts);
            for (const auto &connection: portConnectionsManager_->getConnectionsTo(this, "P")) {
                for (const auto &varAndCoeff: connection.connectableFiller->getPortPin(connection.portName, ts,
                                                                                       data)) {
                    auto p = &problem.getVariable(varAndCoeff.first);
                    balanceConstraint->SetCoefficient(p, varAndCoeff.second * -1.0);
                }
            }
        }
    }
}

void ComponentFiller::addObjective(Antares::optim::api::LinearProblem &problem, const LinearProblemData &data)
{
    // TODO : remplacer par des AST
    if (component_.getModel() == PRICE_MINIM) {
        problem.setMinimization(true);
        for (auto ts: data.getTimeStamps()) {
            for (const auto &connection: portConnectionsManager_->getConnectionsTo(this, "cost")) {
                for (const auto &varAndCoeff: connection.connectableFiller->getPortPin(connection.portName, ts,
                                                                                       data)) {
                    auto variable = &problem.getVariable(varAndCoeff.first);
                    problem.setObjectiveCoefficient(*variable, varAndCoeff.second);
                }
            }
        }
    }
}

void ComponentFiller::update(Antares::optim::api::LinearProblem &problem, const LinearProblemData &data)
{
    // ?
}

map<string, double> ComponentFiller::getPortPin(string name, int timestamp, const LinearProblemData &data) const
{
    // TODO : remplacer par des AST
    if (component_.getModel() == THERMAL) {
        string pVarName = "P_" + component_.getId() + "_" + to_string(timestamp);
        if (name == "P") {
            return {{pVarName, 1.0}};
        } else if (name == "cost") {
            if (!data.hasTimedData("cost_" + component_.getId())) {
                throw;
            }
            double cost = data.getTimedData("cost_" + component_.getId()).at(timestamp);
            return {{pVarName, cost}};
        }
    } else if (component_.getModel() == BATTERY) {
        if (name == "P") {
            string pVarName = "P_" + component_.getId() + "_" + to_string(timestamp);
            return {{pVarName, 1.0}};
        }
    }
    return {};
}