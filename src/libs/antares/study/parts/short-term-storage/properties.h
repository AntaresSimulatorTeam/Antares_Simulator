/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#pragma once

#include <optional>
#include <string>
#include <map>

#include <antares/inifile.h>

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
    bool validate(bool simplexIsWeek);
    bool loadKey(const IniFile::Property* p);
    // Mandatory   Injection nominal capacity, >= 0
    std::optional<double> injectionCapacity;
    // Mandatory   Withdrawal nominal capacity, >= 0
    std::optional<double> withdrawalCapacity;
    // Mandatory   Reservoir capacity in MWh, >= 0
    std::optional<double> capacity;
    // Initial level, between 0 and 1
    std::optional<double> initialLevel;
    // Efficiency factor between 0 and 1
    double efficiencyFactor = 1;
    // Mandatory   Cycle duration, 1 <= storagecycle <= 168
    std::optional<unsigned int> storagecycle;
    // Used to sort outputs
    Group group = Group::Other1;
    // cluster name
    std::string name;

private:
    static const std::map<std::string, enum Group> ST_STORAGE_PROPERTY_GROUP_ENUM;
};
} // namespace Antares::Data::ShortTermStorage
