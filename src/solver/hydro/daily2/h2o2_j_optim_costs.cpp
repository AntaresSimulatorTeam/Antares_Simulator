// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/daily2/h2o2_j_donnees_optimisation.h"
#include "antares/study/fwd.h"

Hydro_problem_costs::Hydro_problem_costs(const Data::Parameters& parameters)
{
    constexpr unsigned int noiseSeed = 0x79683264; // "hyd2" in hexa
    noiseGenerator.reset(noiseSeed);
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
