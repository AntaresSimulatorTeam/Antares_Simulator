#pragma once

#include <string>

#include "storage-for-remix-interface.h"

namespace Antares::Solver::Simulation
{

constexpr double TOLERANCE = 1.e-6;
const std::string error_msg_start = "Remix hydro input : ";

class StorageForRemixNoLevels: public IStorageForRemix
{
public:
    StorageForRemixNoLevels(std::vector<double>& generation,
                            std::vector<double>& unsupE,
                            const std::vector<double> Pmax,
                            const std::vector<double> Pmin);

    double maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) const override;
    void update() override;
    const std::vector<double>& initWithdrawal() const override;
    std::vector<double>& withdrawal() override;

protected:
    void checkInput(size_t size) override;

    std::vector<double>& withdrawal_;
    const std::vector<double> initWithdrawal_;
    std::vector<double>& unsupE_;
    const std::vector<double> pmax_;
    const std::vector<double> pmin_;
};

} // namespace Antares::Solver::Simulation
