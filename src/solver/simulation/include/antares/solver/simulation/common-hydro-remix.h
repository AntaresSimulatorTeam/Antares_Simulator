// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_SIMULATION_COMMON_HYDRO_REMIX_H__
#define __SOLVER_SIMULATION_COMMON_HYDRO_REMIX_H__

#include <vector>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Solver::Simulation
{
std::vector<double> extractSTSpmax(const PROPERTIES& sts_properties,
                                   const unsigned firstHourOfWeek);

} // namespace Antares::Solver::Simulation

#endif // __SOLVER_SIMULATION_COMMON_HYDRO_REMIX_H__
