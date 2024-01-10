/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
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

#include <optional>
#include <string>
#include <map>

#include <antares/inifile/inifile.h>

namespace Antares::Data::ShortTermStorage
{
enum class Group
{
    PSP_open,
    PSP_closed,
    Pondage,
    Battery,
    Other1,
    Other2,
    Other3,
    Other4,
    Other5
};

unsigned int groupIndex(Group group);

class Properties
{
public:
    bool validate();
    bool loadKey(const IniFile::Property* p);
    bool saveToFolder(const std::string& folder) const;

    // Not optional   Injection nominal capacity, >= 0
    std::optional<double> injectionNominalCapacity;
    // Not optional   Withdrawal nominal capacity, >= 0
    std::optional<double> withdrawalNominalCapacity;
    // Not optional   Reservoir capacity in MWh, >= 0
    std::optional<double> reservoirCapacity;
    // Initial level, <= 1
    double initialLevel = initiallevelDefault;
    // Bool to optimise or not initial level
    bool initialLevelOptim = false;
    // Efficiency factor between 0 and 1
    double efficiencyFactor = 1;
    // Used to sort outputs
    Group group = Group::Other1;
    // cluster name
    std::string name;

    static const std::map<std::string, enum Group> ST_STORAGE_PROPERTY_GROUP_ENUM;
private:
    static constexpr double initiallevelDefault = .5;
};
} // namespace Antares::Data::ShortTermStorage
