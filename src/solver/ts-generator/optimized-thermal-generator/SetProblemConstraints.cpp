//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{
void OptimizedThermalGenerator::buildProblemConstraints(const OptProblemSettings& optSett)
{
    setLoadBalanceConstraints(optSett);
    setStartEndMntLogicConstraints();
    setMaxUnitOutputConstraints(optSett);
    printConstraints();
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
void OptimizedThermalGenerator::setStartEndMntLogicConstraints()
{
    return;
}

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
    MPConstraint* ct = solver.MakeRowConstraint(0.0 - deltaSolver, maxPower + deltaSolver, ctName);

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
