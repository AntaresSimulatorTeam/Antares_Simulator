//
// Created by milos on 14/11/23.
//

#include "../main/OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

int OptimizationParameters::calculateUnitEarliestStartOfFirstMaintenance(
  const Data::ThermalCluster& cluster,
  uint unitIndex)
{
    // earliest start of the first maintenance of unit u (beginning of the window, can be negative):
    // let it return negative value - if it returns negative value we wont implement constraint:
    // s[u][0][tauLower-1] = 0

    return std::min(getAverageDurationBetweenMaintenances(cluster)
                      - getDaysSinceLastMaintenance(cluster, unitIndex) - cluster.poWindows,
                    timeHorizon_ - 1);
}

int OptimizationParameters::calculateUnitLatestStartOfFirstMaintenance(
  const Data::ThermalCluster& cluster,
  uint unitIndex)
{
    // latest start of the first maintenance of unit u, must be positive -
    // FIRST STEP ONLY!

    // cannot be negative: FIRST STEP ONLY
    // cluster.interPoPeriod -
    // cluster.originalRandomlyGeneratedDaysSinceLastMaintenance[unitIndex] - is always positive
    // or zero
    // cluster.poWindows is positive or zero
    // however we will make sure it does not surpass timeHorizon_ - 1 value
    // AFTER FIRST STEP it can go to negative value - so we will floor it to zero

    return std::min(
      std::max(0,
               getAverageDurationBetweenMaintenances(cluster)
                 - getDaysSinceLastMaintenance(cluster, unitIndex) + cluster.poWindows),
      timeHorizon_ - 1);
}

std::vector<int> OptimizationParameters::calculateNumberOfMaintenances(
  const Data::ThermalCluster& cluster)
{
    // getAverageMaintenanceDuration must be at least 1
    // so we do not need to check if div / 0

    std::vector<int> numberOfMaintenances;
    numberOfMaintenances.resize(cluster.unitCount);

    if (!(cluster.doWeGenerateTS(globalThermalTSgeneration_) && cluster.optimizeMaintenance))
        return numberOfMaintenances;

    for (int unit = 0; unit != cluster.unitCount; ++unit)
    {
        int div = (timeHorizon_
                   + std::min(getAverageDurationBetweenMaintenances(cluster) - 1,
                              getDaysSinceLastMaintenance(cluster, unit))
                   - getAverageDurationBetweenMaintenances(cluster))
                  / (getAverageDurationBetweenMaintenances(cluster)
                     + getAverageMaintenanceDuration(cluster));
        numberOfMaintenances[unit] = std::max(1 + div, minNumberOfMaintenances);
    }

    return numberOfMaintenances;
}

} // namespace Antares::Solver::TSGenerator
