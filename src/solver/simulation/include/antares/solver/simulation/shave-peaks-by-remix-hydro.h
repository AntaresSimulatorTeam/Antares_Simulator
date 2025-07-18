
#pragma once

#include <tuple>
#include <vector>

namespace Antares::Solver::Simulation
{

std::vector<double> shavePeaksByRemixingHydro(std::vector<double>& HydroGen,
                                              std::vector<double>& UnsupE,
                                              const std::vector<double>& DispatchGen,
                                              const std::vector<double>& HydroPmax,
                                              const std::vector<double>& HydroPmin,
                                              const double initialLevel,
                                              const double capacity,
                                              const double efficiency,
                                              const bool reservoirManagement,
                                              const std::vector<double>& inflow,
                                              const std::vector<double>& overflow,
                                              const std::vector<double>& pump,
                                              const std::vector<double>& Spillage,
                                              const std::vector<double>& DTG_MRG);

} // namespace Antares::Solver::Simulation
