// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "cumulation-constraint-for-remix.h"
#include "storage-for-remix-no-levels.h"

namespace Antares::Solver::Simulation
{

class StorageForRemixWithLevels: public StorageForRemixNoLevels
{
public:
    StorageForRemixWithLevels(std::vector<double>& withdrawal,
                              std::vector<double>& unsupE,
                              std::vector<double>& levels,
                              const std::vector<double> withdrawalMax,
                              const std::vector<double> Pmin,
                              const std::vector<double> inflows,
                              const std::vector<double> overflow,
                              std::vector<double>& injection,
                              const std::vector<double> injectionMax,
                              const std::vector<double> lowRuleCurve,
                              const std::vector<double> upRuleCurve,
                              const double initLevel,
                              const double withdrawalEff,
                              const double injectionEff,
                              const std::string& name = "",
                              std::vector<CumulationConstraintForRemix> cumulationConstraints = {});

    double maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) const override;
    void applyExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen, double amount) override;
    void update() override;
    const std::vector<double>& initInjection() const override;

private:
    void checkLevels();
    void checkInput(size_t size) override;
    double maxWithdrawalExchangeWithLevels(unsigned hourOfMaxGen, unsigned hourOfMinGen) const;
    double maxInjectionExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) const;
    // Max level variation allowed on hours between hourOfMinGen and hourOfMaxGen
    double levelMargin(unsigned hourOfMaxGen, unsigned hourOfMinGen) const;
    // Max amount exchangeable with the given lever under all cumulation constraints
    double cumulationBound(RemixLever lever, unsigned hourOfMaxGen, unsigned hourOfMinGen) const;
    void checkCumulationConstraints() const;

    std::vector<double>& levels_;
    const std::vector<double> inflows_;
    const std::vector<double> overflow_;
    std::vector<double>& injection_;
    const std::vector<double> initInjection_;
    const std::vector<double> injectionMax_;

    const std::vector<double> ruleCurveLow_;
    const std::vector<double> ruleCurveUp_;

    const double initLevel_;
    const double withdrawalEff_;
    const double injectionEff_;

    const std::vector<CumulationConstraintForRemix> cumulationConstraints_;
};

} // namespace Antares::Solver::Simulation
