
#pragma once

#include <vector>

namespace Antares::Solver::Simulation
{

struct RemixHydroOutput
{
    std::vector<double> HydroGen;
    std::vector<double> UnsupE;
    std::vector<double> levels;
};

RemixHydroOutput shavePeaksByRemixingHydro(const std::vector<double>& DispatchGen,
                                           const std::vector<double>& HydroGen,
                                           const std::vector<double>& UnsupE,
                                           const std::vector<double>& HydroPmax,
                                           const std::vector<double>& HydroPmin,
                                           double init_level,
                                           double capacity,
                                           const std::vector<double>& inflow,
                                           const std::vector<double>& overflow,
                                           const std::vector<double>& pump,
                                           const std::vector<double>& Spillage,
                                           const std::vector<double>& DTG_MRG);

} // namespace Antares::Solver::Simulation
