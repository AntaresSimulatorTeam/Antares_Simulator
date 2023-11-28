//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

void OptimizedThermalGenerator::postTimeStepOptimization(OptProblemSettings& optSett)
{
    appendTimeStepResults(optSett);
    reCalculateDaysSinceLastMnt(optSett);
    return;
}

// save/append optimization results form range 0-timeStep
void OptimizedThermalGenerator::appendTimeStepResults(const OptProblemSettings& optSett)
{
    // we have vectors of start (zeros and ones)
    // lets convert that into maintenance start day vector
    // and then randomly generate maintenance duration
    // and create std::pairs - of start_day + mnt_duration

    // loop per units
    for (std::size_t unitIndexTotal = 0; unitIndexTotal < vars.clusterUnits.size();
         ++unitIndexTotal)
    {
        // Unit-unitIndexTotal - is index in a vector of all the units (area * cluster * units)
        // not to be confused by Unit-index - index in cluster

        // variables structure: vars and result structure: scenarioResults
        // are created at the same time in the same loop
        // so the order of the units should be the same !!!
        // so lets avoid creating some search/find method
        // that will find the Unit in scenarioResults according to its parentCluster and index
        // and just loop
        // assert parentCluster and index

        const auto& readResultUnit = vars.clusterUnits[unitIndexTotal];
        auto& storeResultUnit = scenarioResults[unitIndexTotal];

        assert(readResultUnit.parentCluster == storeResultUnit.parentCluster
               && "Read and Store Units do not point to the same parent cluster.");
        assert(readResultUnit.index == storeResultUnit.index
               && "Read and Store Units do not point to the same unit index.");

        auto& cluster = *(readResultUnit.parentCluster);

        // loop per maintenances of unit

        // TODO CR27: do we even have to loop through maintenances
        // or only see if first maintenance start before timeStep_
        // rest (second, third) maintenances will definitely happen after timeStep_ ?!
        // Talk with Hugo

        // if createStartEndVariables for the readResultUnit is false
        // maintenances.size() is going to be zero - so in a way there is our check
        for (int mnt = 0; mnt < readResultUnit.maintenances.size(); ++mnt)
        {
            int localMaintenanceStart = readResultUnit.maintenances[mnt].startDay(timeStep_);
            if (localMaintenanceStart == -1)
                continue;

            int globalMaintenanceStart = localMaintenanceStart + optSett.firstDay;
            int dayInTheYearStart
              = dayOfTheYear(globalMaintenanceStart); // TODO CR27: check this with Hugo!

            int PODOfTheDay
              = (int)cluster.prepro->data[Data::PreproThermal::poDuration][dayInTheYearStart];
            double app = maintenanceData.areaMap[cluster.parentArea]
                           .clusterMap[&cluster]
                           .AP[dayInTheYearStart];
            double bpp = maintenanceData.areaMap[cluster.parentArea]
                           .clusterMap[&cluster]
                           .BP[dayInTheYearStart];
            int maintenanceDuration = durationGenerator(
              cluster.plannedLaw, PODOfTheDay, cluster.plannedVolatility, app, bpp);

            storeResultUnit.maintenanceResults.push_back(
              std::make_pair(globalMaintenanceStart, maintenanceDuration));
        }
    }

    return;
}

// re-calculate parameters
void OptimizedThermalGenerator::reSetDaysSinceLastMnt()
{
    // we are back in first step, but not first scenario
    // we have messed up our values
    // we need to re-do
    // daysSinceLastMaintenance = cluster.originalRandomlyGeneratedDaysSinceLastMaintenance;
    // for all areas and clusters
    for (auto& area : maintenanceData.areaMap)
    {
        for (auto& cluster : area.second.clusterMap)
        {
            cluster.second.daysSinceLastMaintenance
              = cluster.first->originalRandomlyGeneratedDaysSinceLastMaintenance;
        }
    }
    return;
}

void OptimizedThermalGenerator::reCalculateDaysSinceLastMnt(const OptProblemSettings& optSett)
{
    if (optSett.scenario == 0 && optSett.isFirstStep)
        return;
    if (optSett.scenario != 0 && optSett.isFirstStep)
        reSetDaysSinceLastMnt();

    // re-calculate days since last maintenance inputs if necessary
    for (const auto& unit : scenarioResults)
    {
        reCalculateDaysSinceLastMnt(optSett, unit);
    }
}

void OptimizedThermalGenerator::reCalculateDaysSinceLastMnt(const OptProblemSettings& optSett,
                                                            const Unit& unit)
{
    auto& daysSinceLastMaintenance = maintenanceData.areaMap[unit.parentCluster->parentArea]
                                       .clusterMap[unit.parentCluster]
                                       .daysSinceLastMaintenance[unit.index];
    bool maintenanceHappened = false;

    if (unit.maintenanceResults.empty())
    {
        daysSinceLastMaintenance
          = reCalculateDaysSinceLastMnt(optSett, unit, maintenanceHappened, 0, 0);
        return;
    }

    int lastMaintenanceStart = unit.maintenanceResults.back().first;
    // check if maintenance happened in the observed timeStep
    // remember maintenanceResults - stores all the maintenances
    // last maintenance may be from some earlier timeStep
    if (lastMaintenanceStart < optSett.firstDay)
    {
        daysSinceLastMaintenance
          = reCalculateDaysSinceLastMnt(optSett, unit, maintenanceHappened, 0, 0);
        return;
    }

    maintenanceHappened = true;
    int lastMaintenanceDuration = unit.maintenanceResults.back().second;
    daysSinceLastMaintenance = reCalculateDaysSinceLastMnt(
      optSett, unit, maintenanceHappened, lastMaintenanceStart, lastMaintenanceDuration);
    return;
}

int OptimizedThermalGenerator::reCalculateDaysSinceLastMnt(const OptProblemSettings& optSett,
                                                           const Unit& unit,
                                                           bool maintenanceHappened,
                                                           int lastMaintenanceStart,
                                                           int lastMaintenanceDuration)
{
    if (maintenanceHappened)
        return std::max(
          0, optSett.firstDay + timeStep_ - (lastMaintenanceStart + lastMaintenanceDuration));
    else
        return timeStep_
               + unit.parentCluster->originalRandomlyGeneratedDaysSinceLastMaintenance[unit.index];
    // this can lead LatestStartOfFirstMaintenance to negative!!
}

} // namespace Antares::Solver::TSGenerator
