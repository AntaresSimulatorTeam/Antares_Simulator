#pragma once

#include "storage-for-remix-interface.h"

namespace Antares::Solver::Simulation
{

class StorageForRemixNoLevels: public IStorageForRemix
{
public:
    StorageForRemixNoLevels(std::vector<double>& generation,
                            std::vector<double>& unsupE,
                            const std::vector<double> Pmax,
                            const std::vector<double> Pmin);

    double maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) const override;
    void update() override;
    const std::vector<double>& initWithdrawal() override;
    std::vector<double>& withdrawal() override;

protected:
    void checkInput(size_t size) override;

    std::vector<double>& withdrawal_;
    const std::vector<double> initWithdrawal_;
    std::vector<double>& unsupE_;
    const std::vector<double> pmax_;
    const std::vector<double> pmin_;
};

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
                              const double injectionEff);

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
    const double withdrawalEff_ = 1;
    const double injectionEff_ = 1;
};

std::shared_ptr<IStorageForRemix> makeHydroForRemix(std::vector<double>& generation,
                                                    std::vector<double>& unsupE,
                                                    std::vector<double>& levels,
                                                    const std::vector<double>& Pmax,
                                                    const std::vector<double>& Pmin,
                                                    const std::vector<double>& inflows,
                                                    const std::vector<double>& overflow,
                                                    const std::vector<double>& pump,
                                                    const double initLevel,
                                                    const double reservoirCapacity,
                                                    const double pumpEfficiency,
                                                    bool reservoirManagement);

std::shared_ptr<StorageForRemixWithLevels> makeSTSforRemix(std::vector<double>& withdrawal,
                                                           std::vector<double>& unsupE,
                                                           std::vector<double>& levels,
                                                           const std::vector<double>& pmax,
                                                           const std::vector<double>& inflows,
                                                           const std::vector<double>& injection,
                                                           const std::vector<double>& lowRuleCurve,
                                                           const std::vector<double>& upRuleCurve,
                                                           const double initLevel,
                                                           const double withdrawalEff,
                                                           const double injectionEff);

} // namespace Antares::Solver::Simulation
