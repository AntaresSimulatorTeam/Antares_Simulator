#pragma once

#include <memory>
#include <vector>

namespace Antares::Solver::Simulation
{
class StorageForRemix
{
public:
    virtual double maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) = 0;
    virtual void update() = 0;
    virtual std::vector<double>& generation() = 0;

private:
    virtual void checkInput(size_t size) = 0;
};

std::shared_ptr<StorageForRemix> makeHydroForRemix(std::vector<double>& generation,
                                                   std::vector<double>& unsupE,
                                                   std::vector<double>& levels,
                                                   const std::vector<double>& Pmax,
                                                   const std::vector<double>& Pmin,
                                                   const std::vector<double>& inflows,
                                                   const std::vector<double>& overflow,
                                                   const std::vector<double>& pump,
                                                   const double& initLevel,
                                                   const double& capacity,
                                                   const double& pumpEfficiency,
                                                   bool reservoirManagement);

} // namespace Antares::Solver::Simulation
