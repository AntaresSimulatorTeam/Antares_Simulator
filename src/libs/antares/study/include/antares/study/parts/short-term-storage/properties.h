// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <optional>
#include <string>

#include <antares/inifile/inifile.h>

namespace Antares::Data::ShortTermStorage
{
class Properties final
{
public:
    bool validate();
    bool loadKey(const IniFile::Property* p);

    /// Not optional   Injection nominal capacity, >= 0
    std::optional<double> injectionNominalCapacity;
    /// Not optional   Withdrawal nominal capacity, >= 0
    std::optional<double> withdrawalNominalCapacity;
    /// Not optional   Reservoir capacity in MWh, >= 0
    std::optional<double> reservoirCapacity;

    /// Initial level, <= 1
    double initialLevel = initiallevelDefault;
    /// Bool to optimise or not initial level
    bool initialLevelOptim = false;

    /// Efficiency factor for injection between 0 and 1
    double injectionEfficiency = 1;
    /// Efficiency factor for withdrawal between 0 and 1
    double withdrawalEfficiency = 1;

    /// Used to sort outputs
    std::string groupName = "OTHER1";
    /// cluster name
    std::string name;

    bool penalizeVariationWithdrawal = false;
    bool penalizeVariationInjection = false;

    /// Cluster global index (across areas)
    int clusterGlobalIndex;

    /// Enabled ?
    bool enabled = true;

    bool allowOverflow = false;

private:
    static constexpr double initiallevelDefault = .5;
};
} // namespace Antares::Data::ShortTermStorage
