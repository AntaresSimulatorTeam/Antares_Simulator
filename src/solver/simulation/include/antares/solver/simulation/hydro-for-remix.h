#pragma once

#include "storage-for-remix.h"

namespace Antares::Solver::Simulation
{
class HydroForRemix: public StorageForRemix
{
public:
    HydroForRemix(std::vector<double>& generation,
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
                  const bool& reservoirManagement);

    double maxExchange(unsigned hourPeak, unsigned hourBottom) override;
    void checkInput(size_t size) override;
    void update() override;
    std::vector<double>& generation() override;

private:
    std::vector<double>& generation_;
    std::vector<double>& unsupE_;
    std::vector<double>& levels_;
    const std::vector<double>& pmax_;
    const std::vector<double>& pmin_;
    const std::vector<double>& inflows_;
    const std::vector<double>& overflow_;
    const std::vector<double>& pump_;

    const double& initLevel_;
    const double& capacity_;
    const double& pumpEff_;
    const bool& reservoirManagement_;
};
} // namespace Antares::Solver::Simulation
