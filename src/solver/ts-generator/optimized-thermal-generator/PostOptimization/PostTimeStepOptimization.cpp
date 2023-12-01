//
// Created by milos on 14/11/23.
//

#include "../Main/OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

void OptimizedThermalGenerator::postTimeStepOptimization(OptProblemSettings& optSett)
{
    appendTimeStepResults(optSett);
    reCalculateDaysSinceLastMnt(optSett);
    reCalculateTimeHorizon();
    reCalculateNumberOfMaintenances();
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

        // if createStartEndVariables for the readResultUnit is false
        // maintenances.size() is going to be zero - so in a way there is our check
        if (readResultUnit.maintenances.empty())
            continue;

        // NO need to loop through maintenances
        // only one maintenance can happen in the timeStep_
        // TODO CR27: in phase-II we may change this and looping will be necessary
        {
            int localMaintenanceStart = readResultUnit.maintenances[0].startDay(par.timeStep_);
            if (localMaintenanceStart == -1)
                continue;

            int globalMaintenanceStart = localMaintenanceStart + optSett.firstDay;
            int dayInTheYearStart = dayOfTheYear(globalMaintenanceStart);

            int PODOfTheDay
              = (int)cluster.prepro->data[Data::PreproThermal::poDuration][dayInTheYearStart];
            double app = par.clusterData[&cluster].staticInputs.AP[dayInTheYearStart];
            double bpp = par.clusterData[&cluster].staticInputs.BP[dayInTheYearStart];
            int maintenanceDuration = durationGenerator(
              cluster.plannedLaw, PODOfTheDay, cluster.plannedVolatility, app, bpp);

            storeResultUnit.maintenanceResults.push_back(
              std::make_pair(globalMaintenanceStart, maintenanceDuration));
        }
    }

    return;
}

// re-calculate parameters

void OptimizedThermalGenerator::reCalculateDaysSinceLastMnt(const OptProblemSettings& optSett)
{
    // re-calculate days since last maintenance inputs if necessary
    for (const auto& unit : scenarioResults)
    {
        reCalculateDaysSinceLastMnt(optSett, unit);
    }
}

void OptimizedThermalGenerator::reCalculateDaysSinceLastMnt(const OptProblemSettings& optSett,
                                                            const Unit& unit)
{
    auto& daysSinceLastMaintenance
      = par.clusterData[unit.parentCluster].dynamicInputs.daysSinceLastMaintenance[unit.index];
    bool maintenanceHappened = false;

    if (unit.maintenanceResults.empty())
    {
        daysSinceLastMaintenance
          = reCalculateDaysSinceLastMnt(optSett, unit, maintenanceHappened, 0, 0);
        return;
    }

    maintenanceHappened = true;
    int lastMaintenanceStart = unit.maintenanceResults.back().first;
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
    int nextOptimizationFirstDay = optSett.firstDay + par.timeStep_;
    if (maintenanceHappened)
        return std::max(
          0, nextOptimizationFirstDay - (lastMaintenanceStart + lastMaintenanceDuration));
    // we let this go into negative value
    // it will only move the maintenance in the next optimization
    // further away from start
    // TODO CR27: no we don't! It broke the solver - we keep std::max for now!
    else
        return nextOptimizationFirstDay
               + unit.parentCluster->originalRandomlyGeneratedDaysSinceLastMaintenance[unit.index];
}

void OptimizedThermalGenerator::reCalculateTimeHorizon()
{
    par.timeHorizon_ = par.calculateTimeHorizon();
}

void OptimizedThermalGenerator::reCalculateNumberOfMaintenances()
{
    // re-calculate days since last maintenance inputs if necessary
    for (auto& cluster : par.clusterData)
    {
        cluster.second.dynamicInputs.numberOfMaintenances
          = par.calculateNumberOfMaintenances(*(cluster.first));
    }
}

} // namespace Antares::Solver::TSGenerator
