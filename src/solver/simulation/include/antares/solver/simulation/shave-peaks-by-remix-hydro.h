
#pragma once

#include <memory>
#include <vector>

namespace Antares::Solver::Simulation
{

class Storage
{
public:
    virtual double computeBound(unsigned hourPeak, unsigned hourBottom) = 0;

    // should be private at some point, and called in constructor.
    virtual void checkInput(size_t size) = 0;

    virtual void update() = 0;
    virtual std::vector<double>& generation() = 0;
    virtual std::vector<double> levels() = 0;
};

class HydroStorage: public Storage
{
public:
    HydroStorage(std::vector<double>& generation,
                 std::vector<double>& unsupE,
                 const std::vector<double>& Pmax,
                 const std::vector<double>& Pmin,
                 const std::vector<double>& inflows,
                 const std::vector<double>& overflow,
                 const std::vector<double>& pump,
                 const double& initLevel,
                 const double& capacity,
                 const double& pumpEfficiency,
                 const bool& reservoirManagement);

    double computeBound(unsigned hourPeak, unsigned hourBottom) override;
    void checkInput(size_t size) override;
    void update() override;
    std::vector<double>& generation() override;
    std::vector<double> levels() override;

private:
    std::vector<double>& generation_;
    std::vector<double>& unsupE_;
    const std::vector<double>& pmax_;
    const std::vector<double>& pmin_;
    const std::vector<double>& inflows_;
    const std::vector<double>& overflow_;
    const std::vector<double>& pump_;
    std::vector<double> levels_;

    const double& initLevel_;
    const double& capacity_;
    const double& pumpEff_;
    const bool& reservoirManagement_;
};

// std::shared_ptr<Storage> makeHydroStorage();

void shavePeaksByRemixingHydro(std::vector<double>& UnsupE,
                               const std::vector<double>& DispatchGen,
                               const std::vector<double>& Spillage,
                               const std::vector<double>& DTG_MRG,
                               std::shared_ptr<Storage> storage);

} // namespace Antares::Solver::Simulation
