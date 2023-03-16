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

#include <optional>
#pragma once

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
    bool validate();
    bool loadKey(const IniFile::Property* p);
    // Injection nominal capacity, >= 0
    double injectionCapacity;
    // Withdrawal nominal capacity, >= 0
    double withdrawalCapacity;
    // Reservoir capacity in MWh, >= 0
    double capacity;
    // Initial level, <= capacity
    std::optional<double> initialLevel;
    // Efficiency factor between 0 and 1
    double efficiencyFactor;
    // Cycle duration, 1 <= cycleDuration <= 168
    unsigned int cycleDuration;
    // Used to sort outputs
    Group group;
    // cluster name
    std::string name;

    static const std::map<std::string, enum Group> stStoragePropertyGroupEnum;
};
} // namespace Antares::Data::ShortTermStorage
