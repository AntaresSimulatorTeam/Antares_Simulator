// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/antares/fatal-error.h>
#include <antares/application/ScenarioBuilderOwner.h>
#include "antares/solver/simulation/apply-scenario.h"
#include "antares/solver/simulation/timeseries-numbers.h"
#include "antares/solver/ts-generator/generator.h"
#include "antares/study/study.h"

Antares::Solver::ScenarioBuilderOwner::ScenarioBuilderOwner(Data::Study& study):
    study_(study)
{
}

void Antares::Solver::ScenarioBuilderOwner::callScenarioBuilder()
{
    TSGenerator::ResizeGeneratedTimeSeries(study_.areas, study_.parameters);

    // Sampled time-series Numbers
    // We will resize all matrix related to the time-series numbers
    // This operation can be done once since the number of years is constant
    // for a single simulation
    study_.resizeAllTimeseriesNumbers(1 + study_.runtime.rangeLimits.year[Data::rangeEnd]);
    if (not TimeSeriesNumbers::CheckNumberOfColumns(study_.areas))
    {
        throw FatalError(
          "Inconsistent number of time-series detected. Please check your input data.");
    }

    if (not TimeSeriesNumbers::Generate(study_))
    {
        throw FatalError("An unrecoverable error has occurred. Can not continue.");
    }
    if (study_.parameters.useCustomScenario)
    {
        ApplyCustomScenario(study_);
    }
}
