/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <antares/mersenne-twister/mersenne-twister.h>
#include "h2o2_j_donnees_optimisation.h"
#include "antares/study/fwd.h"
#include <functional> // std::reference_wrapper

namespace Constants
{
constexpr double denom = 1e3;
constexpr unsigned int seed = 0x79683264; // "hyd2" in hexa
} // namespace Constants

Hydro_problem_costs::Hydro_problem_costs(const Data::Study& study)
{
    end_days_levels = -1. / 32.;
    overflow = 32 * 68. + 1.;
    deviations = 1.;
    violations = 68.;

    switch (study.parameters.hydroHeuristicPolicy.hhPolicy)
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

void Hydro_problem_costs::apply_noise()
{
    Antares::MersenneTwister noiseGenerator;
    noiseGenerator.reset(Constants::seed); // Arbitrary seed, hard-coded since we don't really want
                                           // the user to change it
    const std::vector<std::reference_wrapper<double>> variables
      = {end_days_levels, overflow, deviations, violations, waste, deviationMax, violationMax};
    for (auto variable : variables)
        variable += noiseGenerator() / Constants::denom;
}
