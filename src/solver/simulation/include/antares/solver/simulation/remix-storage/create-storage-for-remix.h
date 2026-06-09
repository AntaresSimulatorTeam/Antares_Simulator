// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "storage-for-remix-no-levels.h"

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
                                                    const std::string& name = "");

std::shared_ptr<IStorageForRemix> makeSTSforRemix(std::vector<double>& withdrawal,
                                                  std::vector<double>& unsupE,
                                                  std::vector<double>& levels,
                                                  const std::vector<double>& pmax,
                                                  const std::vector<double>& inflows,
                                                  const std::vector<double>& overflows,
                                                  const std::vector<double>& injection,
                                                  const std::vector<double>& lowRuleCurve,
                                                  const std::vector<double>& upRuleCurve,
                                                  const double initLevel,
                                                  const double withdrawalEff,
                                                  const double injectionEff,
                                                  const std::string& name = "");

} // namespace Antares::Solver::Simulation
