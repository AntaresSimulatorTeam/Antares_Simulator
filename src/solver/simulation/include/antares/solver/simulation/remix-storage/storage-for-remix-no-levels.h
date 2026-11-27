// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

#include "storage-for-remix-interface.h"

namespace Antares::Solver::Simulation
{
constexpr double TOLERANCE = 2.e-6;

class StorageForRemixNoLevels: public IStorageForRemix
{
public:
    StorageForRemixNoLevels(std::vector<double>& generation,
                            std::vector<double>& unsupE,
                            const std::vector<double> withdrawalMax,
                            const std::vector<double> Pmin,
                            const std::string& name = "");

    double maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) const override;
    void applyExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen, double amount) override;
    void update() override;
    const std::vector<double>& initWithdrawal() const override;
    const std::vector<double>& initInjection() const override;
    std::vector<double>& withdrawal() override;

protected:
    void checkInput(size_t size) override;
    // Max amount exchangeable by moving withdrawal only, ignoring levels
    double maxWithdrawalExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) const;
    void exchangeWithdrawal(unsigned hourOfMaxGen, unsigned hourOfMinGen, double amount);

    std::vector<double>& withdrawal_;
    const std::vector<double> initWithdrawal_;
    const std::vector<double> noInjection_;
    std::vector<double>& unsupE_;
    const std::vector<double> withdrawalMax_;
    const std::vector<double> pmin_;
    const std::string error_msg_start_hydro_remix;
};

} // namespace Antares::Solver::Simulation
