//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

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
    for (int day = 0; day < timeHorizon_; ++day)
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
    double equalTo = getResidualLoad(optimizationDay);
    MPConstraint* ct = solver.MakeRowConstraint(equalTo, equalTo, ctName);

    insertPowerVars(ct, day);
    insertEnsVars(ct, day);
    insertSpillVars(ct, day);
    return;
}

void OptimizedThermalGenerator::insertEnsVars(MPConstraint* ct, int day)
{
    ct->SetCoefficient(var.day[day].Ens, 1.0);
}

void OptimizedThermalGenerator::insertSpillVars(MPConstraint* ct, int day)
{
    ct->SetCoefficient(var.day[day].Spill, -1.0);
}

void OptimizedThermalGenerator::insertPowerVars(MPConstraint* ct, int day)
{
    // loop per areas inside maintenance group
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        insertPowerVars(ct, day, area);
    }
}

void OptimizedThermalGenerator::insertPowerVars(MPConstraint* ct, int day, const Data::Area& area)
{
    // loop per thermal clusters inside the area
    for (const auto& clusterEntry : area.thermal.list.mapping)
    {
        const auto& cluster = *(clusterEntry.second);

        // we do not check if cluster.optimizeMaintenance = true here
        // we add all the clusters Power inside maintenance group
        if (!checkClusterExist(cluster))
            continue;

        insertPowerVars(ct, day, cluster);
    }
}

void OptimizedThermalGenerator::insertPowerVars(MPConstraint* ct,
                                                int day,
                                                const Data::ThermalCluster& cluster)
{
    // loop per units inside the cluster
    for (int unit = 0; unit < cluster.unitCount; ++unit)
    {
        insertPowerVars(ct, day, cluster, unit);
    }
}

void OptimizedThermalGenerator::insertPowerVars(MPConstraint* ct,
                                                int day,
                                                const Data::ThermalCluster& cluster,
                                                int unit)
{
    ct->SetCoefficient(
      var.day[day].areaMap[cluster.parentArea].clusterMap[&cluster].unitMap[unit].P, 1.0);
}

//////////////////////////////

// CONSTRAINTS per days, per units and per maintenance - constraint-per-each-day+unit+mnt[t][u][m]
void OptimizedThermalGenerator::setStartEndMntLogicConstraints(const OptProblemSettings& optSett)
{
    // loop per areas inside maintenance group
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        setStartEndMntLogicConstraints(optSett, area);
    }
}

void OptimizedThermalGenerator::setStartEndMntLogicConstraints(const OptProblemSettings& optSett,
                                                               const Data::Area& area)
{
    // loop per thermal clusters inside the area
    for (const auto& clusterEntry : area.thermal.list.mapping)
    {
        const auto& cluster = *(clusterEntry.second);

        // check if cluster exist, do we generate + optimizeMaintenance
        // create constraints only if start/end actually exist
        bool createStartEndVar = checkClusterExist(cluster)
                                 && cluster.doWeGenerateTS(globalThermalTSgeneration_)
                                 && cluster.optimizeMaintenance;
        if (!createStartEndVar)
            continue;

        setStartEndMntLogicConstraints(optSett, cluster);
    }
}

void OptimizedThermalGenerator::setStartEndMntLogicConstraints(const OptProblemSettings& optSett,
                                                               const Data::ThermalCluster& cluster)
{
    // loop per units inside the cluster
    for (int unit = 0; unit < cluster.unitCount; ++unit)
    {
        setStartEndMntLogicConstraints(optSett, cluster, unit);
    }
}

void OptimizedThermalGenerator::setStartEndMntLogicConstraints(const OptProblemSettings& optSett,
                                                               const Data::ThermalCluster& cluster,
                                                               int unit)
{
    int totalMntNumber = getNumberOfMaintenances(cluster);
    // loop per maintenances per unit
    for (int mnt = 0; mnt < totalMntNumber; ++mnt)
    {
        setEndOfMaintenanceEventBasedOnAverageDurationOfMaintenanceEvent(
          optSett, cluster, unit, mnt);
        setOnceStartIsSetToOneItWillBeOneUntilEndOfOptimizationTimeHorizon(
          optSett, cluster, unit, mnt);
    }

    // loop per maintenances per unit - except last one
    for (int mnt = 0; mnt < totalMntNumber - 1; ++mnt)
    {
        setUpFollowingMaintenanceBasedOnAverageDurationBetweenMaintenanceEvents(
          optSett, cluster, unit, mnt);
        setNextMaintenanceCanNotStartBeforePreviousMaintenance(optSett, cluster, unit, mnt);
    }
}

void OptimizedThermalGenerator::setEndOfMaintenanceEventBasedOnAverageDurationOfMaintenanceEvent(
  const OptProblemSettings& optSett,
  const Data::ThermalCluster& cluster,
  int unit,
  int mnt)
{
    int averageMaintenanceDuration = getAverageMaintenanceDuration(cluster);
    for (int day = 0; day < timeHorizon_ - averageMaintenanceDuration; ++day)
    {
        std::string ctName = "E[t+Mu][u][q] = S[t][u][q] -> ["
                             + std::to_string(day + optSett.firstDay) + "]["
                             + cluster.getFullName().to<std::string>() + "." + std::to_string(unit)
                             + "][" + std::to_string(mnt) + "]";
        MPConstraint* ct = solver.MakeRowConstraint(0.0, 0.0, ctName);

        ct->SetCoefficient(var.day[day + averageMaintenanceDuration]
                             .areaMap[cluster.parentArea]
                             .clusterMap[&cluster]
                             .unitMap[unit]
                             .end[mnt],
                           1.0);
        ct->SetCoefficient(
          var.day[day].areaMap[cluster.parentArea].clusterMap[&cluster].unitMap[unit].start[mnt],
          -1.0);
    }
    return;
}

void OptimizedThermalGenerator::
  setUpFollowingMaintenanceBasedOnAverageDurationBetweenMaintenanceEvents(
    const OptProblemSettings& optSett,
    const Data::ThermalCluster& cluster,
    int unit,
    int mnt)
{
    int averageDurationBetweenMaintenances = getAverageDurationBetweenMaintenances(cluster);
    for (int day = 0; day < timeHorizon_ - averageDurationBetweenMaintenances; ++day)
    {
        std::string ctName = "S[t+Tu][u][q+1] = E[t][u][q] -> ["
                             + std::to_string(day + optSett.firstDay) + "]["
                             + cluster.getFullName().to<std::string>() + "." + std::to_string(unit)
                             + "][" + std::to_string(mnt) + "]";
        MPConstraint* ct = solver.MakeRowConstraint(0.0, 0.0, ctName);

        ct->SetCoefficient(var.day[day + averageDurationBetweenMaintenances]
                             .areaMap[cluster.parentArea]
                             .clusterMap[&cluster]
                             .unitMap[unit]
                             .start[mnt + 1],
                           1.0);
        ct->SetCoefficient(
          var.day[day].areaMap[cluster.parentArea].clusterMap[&cluster].unitMap[unit].end[mnt],
          -1.0);
    }
    return;
}

void OptimizedThermalGenerator::setOnceStartIsSetToOneItWillBeOneUntilEndOfOptimizationTimeHorizon(
  const OptProblemSettings& optSett,
  const Data::ThermalCluster& cluster,
  int unit,
  int mnt)
{
    for (int day = 0; day < timeHorizon_ - 1; ++day)
    {
        std::string ctName = "S[t+1][u][q] >= S[t][u][q] -> ["
                             + std::to_string(day + optSett.firstDay) + "]["
                             + cluster.getFullName().to<std::string>() + "." + std::to_string(unit)
                             + "][" + std::to_string(mnt) + "]";
        MPConstraint* ct = solver.MakeRowConstraint(0.0 - solverDelta, solverInfinity, ctName);

        ct->SetCoefficient(var.day[day + 1]
                             .areaMap[cluster.parentArea]
                             .clusterMap[&cluster]
                             .unitMap[unit]
                             .start[mnt],
                           1.0);
        ct->SetCoefficient(
          var.day[day].areaMap[cluster.parentArea].clusterMap[&cluster].unitMap[unit].start[mnt],
          -1.0);
    }
    return;
}

void OptimizedThermalGenerator::setNextMaintenanceCanNotStartBeforePreviousMaintenance(
  const OptProblemSettings& optSett,
  const Data::ThermalCluster& cluster,
  int unit,
  int mnt)
{
    for (int day = 0; day < timeHorizon_; ++day)
    {
        std::string ctName = "S[t][u][q] >= S[t][u][q+1] -> ["
                             + std::to_string(day + optSett.firstDay) + "]["
                             + cluster.getFullName().to<std::string>() + "." + std::to_string(unit)
                             + "][" + std::to_string(mnt) + "]";
        MPConstraint* ct = solver.MakeRowConstraint(0.0 - solverDelta, solverInfinity, ctName);

        ct->SetCoefficient(
          var.day[day].areaMap[cluster.parentArea].clusterMap[&cluster].unitMap[unit].start[mnt],
          1.0);
        ct->SetCoefficient(
          var.day[day].areaMap[cluster.parentArea].clusterMap[&cluster].unitMap[unit].start[mnt
                                                                                            + 1],
          -1.0);
    }
    return;
}

//////////////////////////////

// Maximum outputs of the units
// CONSTRAINTS per days and per units - constraint-per-each-day+unit[t][u][m-sum per m]
void OptimizedThermalGenerator::setMaxUnitOutputConstraints(const OptProblemSettings& optSett)
{
    for (int day = 0; day < timeHorizon_; ++day)
    {
        setMaxUnitOutputConstraints(optSett, day);
    }
    return;
}

void OptimizedThermalGenerator::setMaxUnitOutputConstraints(const OptProblemSettings& optSett,
                                                            int& day)
{
    // loop per areas inside maintenance group
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        setMaxUnitOutputConstraints(optSett, day, area);
    }
}

void OptimizedThermalGenerator::setMaxUnitOutputConstraints(const OptProblemSettings& optSett,
                                                            int day,
                                                            const Data::Area& area)
{
    // loop per thermal clusters inside the area
    for (const auto& clusterEntry : area.thermal.list.mapping)
    {
        const auto& cluster = *(clusterEntry.second);

        // we do not check if cluster.optimizeMaintenance = true here
        // we add all the clusters Power inside maintenance group
        if (!checkClusterExist(cluster))
            continue;

        setMaxUnitOutputConstraints(optSett, day, cluster);
    }
}

void OptimizedThermalGenerator::setMaxUnitOutputConstraints(const OptProblemSettings& optSett,
                                                            int day,
                                                            const Data::ThermalCluster& cluster)
{
    int optimizationDay = day + optSett.firstDay;
    double maxPower = getPowerOutput(cluster, optimizationDay);
    // loop per units inside the cluster
    for (int unit = 0; unit < cluster.unitCount; ++unit)
    {
        setMaxUnitOutputConstraints(optSett, day, cluster, unit, maxPower);
    }
}

void OptimizedThermalGenerator::setMaxUnitOutputConstraints(const OptProblemSettings& optSett,
                                                            int day,
                                                            const Data::ThermalCluster& cluster,
                                                            int unit,
                                                            double maxPower)
{
    int optimizationDay = day + optSett.firstDay;
    std::string ctName = "MaxPowerOutputConstraint[" + std::to_string(optimizationDay) + "]["
                         + cluster.getFullName().to<std::string>() + "." + std::to_string(unit)
                         + "]";
    MPConstraint* ct = solver.MakeRowConstraint(0.0 - solverDelta, maxPower + solverDelta, ctName);

    insertPowerVars(ct, day, cluster, unit); // re-using this method on purpose!

    // we add sum[per-q](s[t][u][q]-e[t][u][q])
    // only if we have defined variables start and end for the units
    // if not we ebd up with: 0 <= P <= Pmax
    // note we already passed check: !checkClusterExist(cluster)
    // so here we just check if the cluster is involved in maintenance planing
    if (!(cluster.doWeGenerateTS(globalThermalTSgeneration_) && cluster.optimizeMaintenance))
        return;

    insertStartSum(ct, day, cluster, unit, maxPower);
    insertEndSum(ct, day, cluster, unit, maxPower);
}

void OptimizedThermalGenerator::insertStartSum(MPConstraint* ct,
                                               int day,
                                               const Data::ThermalCluster& cluster,
                                               int unit,
                                               double maxPower)
{
    int totalMntNumber = getNumberOfMaintenances(cluster);
    // loop per maintenances per unit
    for (int mnt = 0; mnt < totalMntNumber; ++mnt)
    {
        ct->SetCoefficient(
          var.day[day].areaMap[cluster.parentArea].clusterMap[&cluster].unitMap[unit].start[mnt],
          maxPower);
    }
}

void OptimizedThermalGenerator::insertEndSum(MPConstraint* ct,
                                             int day,
                                             const Data::ThermalCluster& cluster,
                                             int unit,
                                             double maxPower)
{
    int totalMntNumber = getNumberOfMaintenances(cluster);
    // loop per maintenances per unit
    for (int mnt = 0; mnt < totalMntNumber; ++mnt)
    {
        ct->SetCoefficient(
          var.day[day].areaMap[cluster.parentArea].clusterMap[&cluster].unitMap[unit].end[mnt],
          -maxPower);
    }
}

void OptimizedThermalGenerator::printConstraints()
{
    const int num_constraints = solver.NumConstraints();

    for (int i = 0; i < num_constraints; ++i)
    {
        const MPConstraint* const constraint = solver.constraint(i);
        std::cout << "**** Constraint " << i + 1 << " ****" << std::endl;
        std::cout << "Name: " << constraint->name() << std::endl;
        std::cout << "Lower Bound: " << constraint->lb() << std::endl;
        std::cout << "Upper Bound: " << constraint->ub() << std::endl;

        for (const auto& term : constraint->terms())
        {
            const MPVariable* const var = term.first;
            const double coefficient = term.second;
            std::cout << var->name() << ": " << coefficient << std::endl;
        }
        std::cout << "------------------------" << std::endl;
    }
}

} // namespace Antares::Solver::TSGenerator
