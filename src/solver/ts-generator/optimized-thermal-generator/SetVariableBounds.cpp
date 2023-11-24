//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

// TODO CR27: see if to make this bound or constraint -
// it is definitely easier to do set it as a fix bound -
// but the solver might go crazy - as for adq.patch

// this will fix some start & end variable bounds to 0 or 1
void OptimizedThermalGenerator::fixBounds()
{
    // loop per areas inside maintenance group
    for (const auto& entryWeightMap : maintenanceGroup_)
    {
        const auto& area = *(entryWeightMap.first);
        fixBounds(area);
    }
    return;
}

void OptimizedThermalGenerator::fixBounds(const Data::Area& area)
{
    // loop per thermal clusters inside the area
    for (auto it = area.thermal.list.mapping.begin(); it != area.thermal.list.mapping.end(); ++it)
    {
        const auto& cluster = *(it->second);

        // check if cluster exist, do we generate + optimizeMaintenance
        // create start end variables only for these clusters
        bool existStartEndVar = checkClusterExist(cluster)
                                && cluster.doWeGenerateTS(globalThermalTSgeneration_)
                                && cluster.optimizeMaintenance;
        if (!existStartEndVar)
            continue;

        fixBounds(cluster);
    }
}

void OptimizedThermalGenerator::fixBounds(const Data::ThermalCluster& cluster)
{
    int totalMntNumber = getNumberOfMaintenances(cluster);
    int avrMntDuration = getAverageMaintenanceDuration(cluster);

    // loop per units inside the cluster
    for (int unit = 0; unit < cluster.unitCount; ++unit)
    {
        fixBoundsFirstMnt(cluster, unit);
        fixBounds(cluster, unit, totalMntNumber, avrMntDuration);
    }
}

// Bounds for the start of the first maintenance
// first maintenance must start between tauLower and tauUpper
// start[tauLower-1][u][1] = 0
// start[tauUpper][u][1] = 1
void OptimizedThermalGenerator::fixBoundsFirstMnt(const Data::ThermalCluster& cluster, int unit)
{
    int earliestStartOfFirstMaintenance
      = calculateUnitEarliestStartOfFirstMaintenance(cluster, unit);
    int latestStartOfFirstMaintenance = calculateUnitLatestStartOfFirstMaintenance(cluster, unit);

    //
    // We assume here that vector "start" has member [0]
    // meaning: for each unit we assume we have at least one maintenance
    // this assumption is ok - since method calculateNumberOfMaintenances()
    // will never return number bellow 2

    if (earliestStartOfFirstMaintenance >= 1)
    {
        // start[tauLower-1][u][1] = 0
        var.day[earliestStartOfFirstMaintenance - 1]
          .areaMap[cluster.parentArea]
          .clusterMap[&cluster]
          .unitMap[unit]
          .start[0]
          ->SetBounds(0.0, 0.0); //inclusive range
    }

    // start[tauUpper][u][1] = 1
    var.day[latestStartOfFirstMaintenance]
      .areaMap[cluster.parentArea]
      .clusterMap[&cluster]
      .unitMap[unit]
      .start[0]
      ->SetBounds(1.0, 1.0); //inclusive range 

    return;
}

void OptimizedThermalGenerator::fixBounds(const Data::ThermalCluster& cluster,
                                          int unit,
                                          int totalMntNum,
                                          int avrMntDuration)
{
    // loop per maintenances of unit
    for (int mnt = 0; mnt < totalMntNum; ++mnt)
    {
        fixBoundsStartSecondMnt(cluster, unit, mnt);
        fixBoundsMntEnd(cluster, unit, mnt, avrMntDuration);
    }

    return;
}

// Ensure that units with max average duration between maintenances start their second maintenance
// start[T][u][q] = 1
void OptimizedThermalGenerator::fixBoundsStartSecondMnt(const Data::ThermalCluster& cluster,
                                                        int unit,
                                                        int mnt)
{
    var.day[timeHorizon_ - 1]
      .areaMap[cluster.parentArea]
      .clusterMap[&cluster]
      .unitMap[unit]
      .start[mnt]
      ->SetBounds(1.0, 1.0);
}

// End of the maintenance can't happen before average maintenance duration
// end[T = [0, average_maintenance_duration_per_unit]][u][q] = 0
void OptimizedThermalGenerator::fixBoundsMntEnd(const Data::ThermalCluster& cluster,
                                                int unit,
                                                int mnt,
                                                int avrMntDuration)
{
    for (int day = 0; day < avrMntDuration; ++day)
    {
        var.day[day]
          .areaMap[cluster.parentArea]
          .clusterMap[&cluster]
          .unitMap[unit]
          .end[mnt]
          ->SetBounds(0.0, 0.0);
    }
}

} // namespace Antares::Solver::TSGenerator
