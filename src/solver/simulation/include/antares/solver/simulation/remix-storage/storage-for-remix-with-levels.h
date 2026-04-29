#pragma once

#include "storage-for-remix-no-levels.h"

namespace Antares::Solver::Simulation
{

class StorageForRemixWithLevels: public StorageForRemixNoLevels
{
public:
    StorageForRemixWithLevels(std::vector<double>& withdrawal,
                              std::vector<double>& unsupE,
                              std::vector<double>& levels,
                              const std::vector<double> Pmax,
                              const std::vector<double> Pmin,
                              const std::vector<double> inflows,
                              const std::vector<double> overflow,
                              const std::vector<double>& injection,
                              const std::vector<double> lowRuleCurve,
                              const std::vector<double> upRuleCurve,
                              const double initLevel,
                              const double withdrawalEff,
                              const double injectionEff,
                              const std::string& name = "");

    double maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) const override;
    void update() override;

private:
    void checkLevels();
    void checkInput(size_t size) override;

    std::vector<double>& levels_;
    const std::vector<double> inflows_;
    const std::vector<double> overflow_;
    const std::vector<double>& injection_;

    const std::vector<double> ruleCurveLow_;
    const std::vector<double> ruleCurveUp_;

    const double initLevel_;
    const double withdrawalEff_;
    const double injectionEff_;
};

} // namespace Antares::Solver::Simulation
