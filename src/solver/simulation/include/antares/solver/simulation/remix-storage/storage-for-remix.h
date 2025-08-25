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

    double maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) override;
    void update() override;
    const std::vector<double>& initialGen() override;
    std::vector<double>& generation() override;

protected:
    void checkInput(size_t size) override;

    std::vector<double>& generation_;
    const std::vector<double> initialGen_;
    std::vector<double>& unsupE_;
    const std::vector<double> pmax_;
    // pmin_ is not a const ref to vector because of the way we acquire the related hydro TS
    // (outside constructor). This requires a deep copy in constructor.
    // Maybe we should pass to constructor and have here a std::span<cont double> instead,
    // this would require a very light weight copy
    const std::vector<double> pmin_;
};

class StorageForRemixWithLevels: public StorageForRemixNoLevels
{
public:
    StorageForRemixWithLevels(std::vector<double>& generation,
                              std::vector<double>& unsupE,
                              std::vector<double>& levels,
                              const std::vector<double> Pmax,
                              const std::vector<double> Pmin,
                              const std::vector<double> inflows,
                              const std::vector<double> overflow,
                              const std::vector<double>& pump,
                              const std::vector<double> lowRuleCurve,
                              const std::vector<double> upRuleCurve,
                              const double initLevel,
                              const double pumpEfficiency);

    double maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) override;
    void update() override;

private:
    void checkLevels();
    void checkInput(size_t size) override;

    std::vector<double>& levels_;
    const std::vector<double> inflows_;
    const std::vector<double> overflow_;
    const std::vector<double>& pump_;

    const std::vector<double> ruleCurveLow_;
    const std::vector<double> ruleCurveUp_;

    const double initLevel_;
    const double pumpEff_;
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
                                                           const double efficiency);

} // namespace Antares::Solver::Simulation
