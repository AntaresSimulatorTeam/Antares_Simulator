//
// Created by milos on 14/11/23.
//

#include "../main/OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

// this method is called at the very end
// after all time-steps and scenarios
void OptimizedThermalGenerator::writeTsResults()
{
    // we need to loop through all the clusters
    // and write the results
    // it would be much easier to loop using clusterData
    // inside of it we already excluded all the non-important clusters
    // however it is const Data::ThermalCluster*
    // so we cannot modify cluster values

    for (auto& entryWeightMap : maintenanceGroup_)
    {
        auto& area = *(entryWeightMap.first);

        for (auto& clusterEntry : area.thermal.list.mapping)
        {
            auto& cluster = *(clusterEntry.second);

            if (!(checkClusterExist(cluster) && cluster.doWeGenerateTS(globalThermalTSgeneration_)
                  && cluster.optimizeMaintenance))
                continue;

            if (derated)
                cluster.series.timeSeries.averageTimeseries();

            if (archive)
                writeResultsToDisk(area, cluster);

            cluster.calculationOfSpinning();
        }
    }
    return;
}

} // namespace Antares::Solver::TSGenerator
