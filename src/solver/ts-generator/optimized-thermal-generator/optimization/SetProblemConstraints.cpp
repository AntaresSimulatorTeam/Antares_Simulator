//
// Created by milos on 14/11/23.
//

#include "../main/OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
void OptimizedThermalGenerator::buildProblemConstraints(const OptProblemSettings& optSett)
{
    setLoadBalanceConstraints(optSett);
    setStartEndMntLogicConstraints(optSett);
    setMaxUnitOutputConstraints(optSett);
}

// load balance CONSTRAINTS - constraint-per-each-day[t] - we have sum through [u] inside of it
void OptimizedThermalGenerator::setLoadBalanceConstraints(const OptProblemSettings& optSett)
{
    for (int day = 0; day < par.timeHorizon_; ++day)
    {
        setLoadBalanceConstraints(optSett, day);
    }
    return;
}

void OptimizedThermalGenerator::setLoadBalanceConstraints(const OptProblemSettings& optSett,
                                                          int& day)
{
    int optimizationDay = day + optSett.firstDay;
    std::string ctName = "LoadBalanceConstraint[" + std::to_string(optimizationDay) + "]";
    double residualLoad = par.getResidualLoad(optimizationDay);
    MPConstraint* ct = solver.MakeRowConstraint(residualLoad, residualLoad, ctName);

    insertPowerVars(ct, day);
    insertEnsVars(ct, day);
    insertSpillVars(ct, day);
    return;
}

void OptimizedThermalGenerator::insertEnsVars(MPConstraint* ct, int day)
{
    ct->SetCoefficient(vars.ens[day], 1.0);
}

void OptimizedThermalGenerator::insertSpillVars(MPConstraint* ct, int day)
{
    ct->SetCoefficient(vars.spill[day], -1.0);
}

void OptimizedThermalGenerator::insertPowerVars(MPConstraint* ct, int day)
{
    for (const auto& unit : vars.clusterUnits)
    {
        insertPowerVars(ct, day, unit);
    }
}

void OptimizedThermalGenerator::insertPowerVars(MPConstraint* ct, int day, const Unit& unit)
{
    ct->SetCoefficient(unit.P[day], 1.0);
}

// CONSTRAINTS per days, per units and per maintenance - constraint-per-each-day+unit+mnt[u][m][t]
void OptimizedThermalGenerator::setStartEndMntLogicConstraints(const OptProblemSettings& optSett)
{
    for (const auto& unit : vars.clusterUnits)
    {
        setStartEndMntLogicConstraints(optSett, unit);
    }
}

void OptimizedThermalGenerator::setStartEndMntLogicConstraints(const OptProblemSettings& optSett,
                                                               const Unit& unit)
{
    if (!unit.createStartEndVariables)
        return;

    // loop per maintenances per unit
    for (int mnt = 0; mnt < unit.maintenances.size(); ++mnt)
    {
        assert(unit.maintenances[mnt].start.size() == par.timeHorizon_);
        assert(unit.maintenances[mnt].end.size() == par.timeHorizon_);
        setEndOfMaintenanceEventBasedOnAverageDurationOfMaintenanceEvent(optSett, unit, mnt);
        setOnceStartIsSetToOneItWillBeOneUntilEndOfOptimizationTimeHorizon(optSett, unit, mnt);
    }

    // loop per maintenances per unit - except last one
    for (int mnt = 0; mnt < unit.maintenances.size() - 1; ++mnt)
    {
        setUpFollowingMaintenanceBasedOnAverageDurationBetweenMaintenanceEvents(optSett, unit, mnt);
        setNextMaintenanceCanNotStartBeforePreviousMaintenance(optSett, unit, mnt);
    }
}

void OptimizedThermalGenerator::setEndOfMaintenanceEventBasedOnAverageDurationOfMaintenanceEvent(
  const OptProblemSettings& optSett,
  const Unit& unit,
  int mnt)
{
    const auto& cluster = *(unit.parentCluster);
    int averageMaintenanceDuration = par.getAverageMaintenanceDuration(cluster);
    for (int day = 0; day < par.timeHorizon_ - averageMaintenanceDuration; ++day)
    {
        std::string ctName = "E[u][q][t+Mu] = S[u][q][t] -> ["
                             + cluster.getFullName().to<std::string>() + "."
                             + std::to_string(unit.index) + "][" + std::to_string(mnt) + "]["
                             + std::to_string(day + optSett.firstDay) + "]";
        MPConstraint* ct = solver.MakeRowConstraint(0.0, 0.0, ctName);

        ct->SetCoefficient(unit.maintenances[mnt].end[day + averageMaintenanceDuration], 1.0);
        ct->SetCoefficient(unit.maintenances[mnt].start[day], -1.0);
    }
    return;
}

void OptimizedThermalGenerator::
  setUpFollowingMaintenanceBasedOnAverageDurationBetweenMaintenanceEvents(
    const OptProblemSettings& optSett,
    const Unit& unit,
    int mnt)
{
    const auto& cluster = *(unit.parentCluster);
    int averageDurationBetweenMaintenances = par.getAverageDurationBetweenMaintenances(cluster);
    for (int day = 0; day < par.timeHorizon_ - averageDurationBetweenMaintenances; ++day)
    {
        std::string ctName = "S[u][q+1][t+Tu] = E[u][q][t] -> ["
                             + cluster.getFullName().to<std::string>() + "."
                             + std::to_string(unit.index) + "][" + std::to_string(mnt) + "]["
                             + std::to_string(day + optSett.firstDay) + "]";
        MPConstraint* ct = solver.MakeRowConstraint(0.0, 0.0, ctName);

        ct->SetCoefficient(
          unit.maintenances[mnt + 1].start[day + averageDurationBetweenMaintenances], 1.0);
        ct->SetCoefficient(unit.maintenances[mnt].end[day], -1.0);
    }
    return;
}

void OptimizedThermalGenerator::setOnceStartIsSetToOneItWillBeOneUntilEndOfOptimizationTimeHorizon(
  const OptProblemSettings& optSett,
  const Unit& unit,
  int mnt)
{
    const auto& cluster = *(unit.parentCluster);
    for (int day = 0; day < par.timeHorizon_ - 1; ++day)
    {
        std::string ctName = "S[u][q][t+1] >= S[u][q][t] -> ["
                             + cluster.getFullName().to<std::string>() + "."
                             + std::to_string(unit.index) + "][" + std::to_string(mnt) + "]["
                             + std::to_string(day + optSett.firstDay) + "]";
        MPConstraint* ct = solver.MakeRowConstraint(0.0 - solverDelta, solverInfinity, ctName);

        ct->SetCoefficient(unit.maintenances[mnt].start[day + 1], 1.0);
        ct->SetCoefficient(unit.maintenances[mnt].start[day], -1.0);
    }
    return;
}

void OptimizedThermalGenerator::setNextMaintenanceCanNotStartBeforePreviousMaintenance(
  const OptProblemSettings& optSett,
  const Unit& unit,
  int mnt)
{
    const auto& cluster = *(unit.parentCluster);
    for (int day = 0; day < par.timeHorizon_; ++day)
    {
        std::string ctName = "S[u][q][t] >= S[u][q+1][t] -> ["
                             + cluster.getFullName().to<std::string>() + "."
                             + std::to_string(unit.index) + "][" + std::to_string(mnt) + "]["
                             + std::to_string(day + optSett.firstDay) + "]";
        MPConstraint* ct = solver.MakeRowConstraint(0.0 - solverDelta, solverInfinity, ctName);

        ct->SetCoefficient(unit.maintenances[mnt].start[day], 1.0);
        ct->SetCoefficient(unit.maintenances[mnt + 1].start[day], -1.0);
    }
    return;
}

// Maximum outputs of the units
// CONSTRAINTS per days and per units - constraint-per-each-day+unit[t][u][m-sum per m]
void OptimizedThermalGenerator::setMaxUnitOutputConstraints(const OptProblemSettings& optSett)
{
    for (int day = 0; day < par.timeHorizon_; ++day)
    {
        setMaxUnitOutputConstraints(optSett, day);
    }
    return;
}

void OptimizedThermalGenerator::setMaxUnitOutputConstraints(const OptProblemSettings& optSett,
                                                            int& day)
{
    for (const auto& unit : vars.clusterUnits)
    {
        setMaxUnitOutputConstraints(optSett, day, unit);
    }
}

void OptimizedThermalGenerator::setMaxUnitOutputConstraints(const OptProblemSettings& optSett,
                                                            int day,
                                                            const Unit& unit)
{
    const auto& cluster = *(unit.parentCluster);
    int optimizationDay = day + optSett.firstDay;
    double maxPower = par.getPowerOutput(cluster, optimizationDay);

    std::string ctName = "MaxPowerOutputConstraint[" + cluster.getFullName().to<std::string>() + "."
                         + std::to_string(unit.index) + "][" + std::to_string(optimizationDay)
                         + "]";
    MPConstraint* ct = solver.MakeRowConstraint(0.0 - solverDelta, maxPower + solverDelta, ctName);

    insertPowerVars(ct, day, unit); // re-using this method on purpose!

    // we add sum[per-q](s[u][q][t]-e[u][q][t])
    // only if we have defined variables start and end for the units
    // if not we ebd up with: 0 <= P <= Pmax
    // so here we just check if the cluster is involved in maintenance planing
    if (!unit.createStartEndVariables)
        return;

    insertStartSum(ct, day, unit, maxPower);
    insertEndSum(ct, day, unit, maxPower);
}

void OptimizedThermalGenerator::insertStartSum(MPConstraint* ct,
                                               int day,
                                               const Unit& unit,
                                               double maxPower)
{
    for (int mnt = 0; mnt < unit.maintenances.size(); ++mnt)
    {
        ct->SetCoefficient(unit.maintenances[mnt].start[day], maxPower);
    }
}

void OptimizedThermalGenerator::insertEndSum(MPConstraint* ct,
                                             int day,
                                             const Unit& unit,
                                             double maxPower)
{
    // loop per maintenances per unit
    for (int mnt = 0; mnt < unit.maintenances.size(); ++mnt)
    {
        ct->SetCoefficient(unit.maintenances[mnt].end[day], -maxPower);
    }
}

} // namespace Antares::Solver::TSGenerator
