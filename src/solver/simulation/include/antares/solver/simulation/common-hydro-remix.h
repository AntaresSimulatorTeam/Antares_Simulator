// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_SIMULATION_COMMON_HYDRO_REMIX_H__
#define __SOLVER_SIMULATION_COMMON_HYDRO_REMIX_H__

#include <span>
#include <vector>

#include "antares/solver/simulation/remix-storage/remix-utils.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Solver::Simulation
{
std::span<const double> weekSubRange(const std::vector<double>& v, unsigned firstHourOfWeek);
std::vector<double> extractSTSpmax(const PROPERTIES& sts_properties,
                                   const unsigned firstHourOfWeek);

} // namespace Antares::Solver::Simulation

namespace Antares::Solver::Simulation
{
inline std::vector<double> operator*(std::span<const double> span, double scalar)
{
    std::vector<double> result;
    result.reserve(span.size());
    for (size_t i = 0; i < span.size(); ++i)
    {
        result.push_back(span[i] * scalar);
    }
    return result;
}

} // namespace Antares::Solver::Simulation

#endif // __SOLVER_SIMULATION_COMMON_HYDRO_REMIX_H__
