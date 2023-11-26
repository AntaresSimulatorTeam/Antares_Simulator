//
// Created by milos on 14/11/23.
//

#include "OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

void OptimizedThermalGenerator::postScenarioOptimization(OptProblemSettings& optSett)
{
    // do not save if optimization failed at some step
    if (!optSett.solved)
        return;

    calculateScenarioResults(optSett);
    saveScenarioResults(optSett);
    resetResultStorage();

    return;
}

void OptimizedThermalGenerator::calculateScenarioResults(const OptProblemSettings& optSett)
{
    return;
}

void OptimizedThermalGenerator::saveScenarioResults(const OptProblemSettings& optSett)
{
    // save results

    int colSaveFrom = optSett.scenario * scenarioLength_;
    int colSaveTo = colSaveFrom + scenarioLength_ - 1;

    // loop through all areas and clusters and write results

    /*
    // do not forget archive!!
    if (archive)
        writeResultsToDisk(area, cluster);

    */
}

void OptimizedThermalGenerator::resetResultStorage()
{
    scenarioResults.clear();
    return;
}

} // namespace Antares::Solver::TSGenerator
