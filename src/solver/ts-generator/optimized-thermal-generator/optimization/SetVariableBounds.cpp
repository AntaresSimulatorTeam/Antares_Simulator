//
// Created by milos on 14/11/23.
//

#include "../main/OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

// TODO CR27: see if to make this bound or constraint -
// it is definitely easier to do set it as a fix bound -
// but the solver might go crazy - as for adq.patch

// this will fix some start & end variable bounds to 0 or 1
void OptimizedThermalGenerator::fixBounds()
{
    // loop per units
    for (const auto& unit : vars.clusterUnits)
    {
        fixBounds(unit);
    }
    return;
}

void OptimizedThermalGenerator::fixBounds(const Unit& unit)
{
    if (!unit.createStartEndVariables)
        return;

    int averageMaintenanceDuration = par.getAverageMaintenanceDuration(*(unit.parentCluster));

    fixBoundsFirstMnt(unit);
    fixBounds(unit, averageMaintenanceDuration);
}

// Bounds for the start of the first maintenance
// first maintenance must start between tauLower and tauUpper
// start[u][0][tauLower-1] = 0
// start[u][0][tauUpper] = 1
void OptimizedThermalGenerator::fixBoundsFirstMnt(const Unit& unit)
{
    int earliestStartOfFirstMaintenance
      = par.calculateUnitEarliestStartOfFirstMaintenance(*(unit.parentCluster), unit.index);
    int latestStartOfFirstMaintenance
      = par.calculateUnitLatestStartOfFirstMaintenance(*(unit.parentCluster), unit.index);

    //
    // We assume here that vector "maintenance" has member [0]
    // meaning: for each unit we assume we have at least one maintenance
    // this assumption is ok - since method calculateNumberOfMaintenances()
    // will never return number bellow 2

    if (earliestStartOfFirstMaintenance >= 1)
    {
        // start[u][0][tauLower-1] = 0
        unit.maintenances[0].start[earliestStartOfFirstMaintenance - 1]->SetBounds(0.0, 0.0);
    }

    // start[u][0][tauUpper] = 1
    unit.maintenances[0].start[latestStartOfFirstMaintenance]->SetBounds(1.0, 1.0);

    return;
}

void OptimizedThermalGenerator::fixBounds(const Unit& unit, int averageMaintenanceDuration)
{
    // loop per maintenances of unit
    for (int mnt = 0; mnt < unit.maintenances.size(); ++mnt)
    {
        fixBoundsStartSecondMnt(unit, mnt);
        fixBoundsMntEnd(unit, mnt, averageMaintenanceDuration);
    }

    return;
}

// Ensure that units with max average duration between maintenances start their second maintenance
// start[u][q][T] = 1
void OptimizedThermalGenerator::fixBoundsStartSecondMnt(const Unit& unit, int mnt)
{
    unit.maintenances[mnt].start[par.timeHorizon_ - 1]->SetBounds(1.0, 1.0);
}

// End of the maintenance can't happen before average maintenance duration
// end[u][q][T = [0, average_maintenance_duration_per_unit]] = 0
void OptimizedThermalGenerator::fixBoundsMntEnd(const Unit& unit,
                                                int mnt,
                                                int averageMaintenanceDuration)
{
    for (int day = 0; day < averageMaintenanceDuration; ++day)
    {
        unit.maintenances[mnt].end[day]->SetBounds(0.0, 0.0);
    }
}

} // namespace Antares::Solver::TSGenerator
