#pragma once

#include <vector>

namespace Antares::Solver::Simulation
{
class StorageForRemix
{
public:
    virtual double maxExchange(unsigned hourPeak, unsigned hourBottom) = 0;
    // gp : checkInput(...) should be private at some point, and called in constructor.
    virtual void checkInput(size_t size) = 0;
    virtual void update() = 0;
    virtual std::vector<double>& generation() = 0;
};
} // namespace Antares::Solver::Simulation
