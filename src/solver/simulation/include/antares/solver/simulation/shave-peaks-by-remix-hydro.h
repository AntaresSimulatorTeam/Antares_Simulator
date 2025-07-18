
#pragma once

// #include <memory>
#include <vector>

namespace Antares::Solver::Simulation
{

class Storage
{
public:
    virtual double computeBound(unsigned hourPeak, unsigned hourBottom) = 0;
    virtual void checkInput() = 0; // should be private at some point, and called in constructor.
    virtual void update() = 0;
    virtual std::vector<double>& generation() = 0;
    virtual std::vector<double> levels() = 0;
};

// std::shared_ptr<Storage> makeHydroStorage();

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
