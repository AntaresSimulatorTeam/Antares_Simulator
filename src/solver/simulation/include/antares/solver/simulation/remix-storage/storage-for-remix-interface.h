#pragma once

#include <memory>
#include <vector>

namespace Antares::Solver::Simulation
{
class IStorageForRemix
{
public:
    virtual double maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) = 0;
    virtual void update() = 0;
    virtual const std::vector<double>& initialGen() = 0;
    virtual std::vector<double>& generation() = 0;

private:
    virtual void checkInput(size_t size) = 0;
};

using listStorageForRemix = std::vector<std::shared_ptr<IStorageForRemix>>;

} // namespace Antares::Solver::Simulation
