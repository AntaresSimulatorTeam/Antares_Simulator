#include "antares/solver/simulation/remix-storage/create-storage-for-remix.h"

#include "antares/solver/simulation/remix-storage/storage-for-remix-with-levels.h"

namespace Antares::Solver::Simulation
{

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
                                                    bool reservoirManagement,
                                                    const std::string& name)
{
    if (!reservoirManagement)
    {
        return std::make_shared<StorageForRemixNoLevels>(generation, unsupE, Pmax, Pmin);
    }

    size_t size = generation.size();
    const std::vector<double> lowRuleCurve(size, 0.);
    const std::vector<double> upRuleCurve(size, reservoirCapacity);
    const double generationEff = 1.;
    return std::make_shared<StorageForRemixWithLevels>(generation,
                                                       unsupE,
                                                       levels,
                                                       Pmax,
                                                       Pmin,
                                                       inflows,
                                                       overflow,
                                                       pump,
                                                       lowRuleCurve,
                                                       upRuleCurve,
                                                       initLevel,
                                                       generationEff,
                                                       pumpEfficiency,
                                                       name);
}

std::shared_ptr<IStorageForRemix> makeSTSforRemix(std::vector<double>& withdrawal,
                                                  std::vector<double>& unsupE,
                                                  std::vector<double>& levels,
                                                  const std::vector<double>& pmax,
                                                  const std::vector<double>& inflows,
                                                  const std::vector<double>& injection,
                                                  const std::vector<double>& lowRuleCurve,
                                                  const std::vector<double>& upRuleCurve,
                                                  const double initLevel,
                                                  const double withdrawalEff,
                                                  const double injectionEff,
                                                  const std::string& name)
{
    size_t size = withdrawal.size();

    std::vector<double> overflows(size, 0.);
    std::vector<double> pmin(size, 0.);

    return std::make_shared<StorageForRemixWithLevels>(withdrawal,
                                                       unsupE,
                                                       levels,
                                                       pmax,
                                                       pmin,
                                                       inflows,
                                                       overflows,
                                                       injection,
                                                       lowRuleCurve,
                                                       upRuleCurve,
                                                       initLevel,
                                                       withdrawalEff,
                                                       injectionEff,
                                                       name);
}

} // namespace Antares::Solver::Simulation
