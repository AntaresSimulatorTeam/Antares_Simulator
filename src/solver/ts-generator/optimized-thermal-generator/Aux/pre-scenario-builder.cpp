/*
** created by milos 13/11/2023
*/

#include <antares/fatal-error.h>
#include "pre-scenario-builder.h"
#include "../../../simulation/apply-scenario.h"
#include "../../../simulation/timeseries-numbers.h"

namespace Antares::Solver
{
void ApplyScenarioBuilderDueToMaintenancePlanning(Data::Study& study)
{
    if (!study.parameters.maintenancePlanning.isOptimized())
        return;

    if (!(study.parameters.timeSeriesToGenerate & Antares::Data::timeSeriesThermal))
        return;

    study.resizeAllTimeseriesNumbers(1 + study.runtime->rangeLimits.year[Data::rangeEnd]);

    if (!TimeSeriesNumbers::Generate(study))
    {
        throw FatalError("An unrecoverable error has occurred. Can not continue.");
    }

    if (study.parameters.useCustomScenario)
        ApplyCustomScenario(study);
}

} // namespace Antares::Solver
