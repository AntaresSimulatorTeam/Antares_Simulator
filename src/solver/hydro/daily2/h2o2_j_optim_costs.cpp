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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/hydro/daily2/h2o2_j_donnees_optimisation.h"
#include "antares/study/fwd.h"

Hydro_problem_costs::Hydro_problem_costs(const Data::Parameters& parameters)
{
    noiseGenerator.reset(Constants::seed);
    end_days_levels = -1. / 32.;
    overflow = 32 * 68. + 1.;
    deviations = 1.;
    violations = 68.;

    switch (parameters.hydroHeuristicPolicy.hhPolicy)
    {
    case Data::hhpMaximizeGeneration:
        waste = 33 * 68.;
        break;
    case Data::hhpAccommodateRuleCurves:
        waste = 34.0;
        break;
    default:
        waste = 34.0;
        break;
    }

    deviationMax = 2.0;
    violationMax = 68.0;
}
