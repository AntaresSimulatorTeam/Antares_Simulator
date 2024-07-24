/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

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
