/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include <map>
#include <optional>
#include <string>

#include <antares/inifile/inifile.h>

namespace Antares::Data::ShortTermStorage
{
class Properties
{
public:
    bool validate();
    bool loadKey(const IniFile::Property* p);
    void save(IniFile& ini) const;

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

    // Used to sort outputs
    std::string groupName = "OTHER1";
    /// cluster name
    std::string name;

    bool penalizeVariationWithdrawal = false;
    bool penalizeVariationInjection = false;

    /// Enabled ?
    bool enabled = true;

private:
    static constexpr double initiallevelDefault = .5;
};
} // namespace Antares::Data::ShortTermStorage
