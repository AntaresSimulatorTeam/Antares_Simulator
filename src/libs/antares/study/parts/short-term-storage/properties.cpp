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

#include <antares/logs.h>
#include <stdexcept>

#include "properties.h"

namespace Antares::Data::ShortTermStorage
{
const std::map<std::string, enum Group> Properties::ST_STORAGE_PROPERTY_GROUP_ENUM
  = {{"PSP_open", Group::PSP_open},
     {"PSP_closed", Group::PSP_closed},
     {"Pondage", Group::Pondage},
     {"Battery", Group::Battery},
     {"Other1", Group::Other1},
     {"Other2", Group::Other2},
     {"Other3", Group::Other3},
     {"Other4", Group::Other4},
     {"Other5", Group::Other5}};

unsigned int groupIndex(Group group)
{
    switch (group)
    {
    case Group::PSP_open:
        return 0;
    case Group::PSP_closed:
        return 1;
    case Group::Pondage:
        return 2;
    case Group::Battery:
        return 3;
    case Group::Other1:
        return 4;
    case Group::Other2:
        return 5;
    case Group::Other3:
        return 6;
    case Group::Other4:
        return 7;
    case Group::Other5:
        return 8;
    default:
        throw std::invalid_argument("Group not recognized");
    }
}

bool Properties::loadKey(const IniFile::Property* p)
{
    auto valueForOptional = [p](auto& opt) {
        if (double tmp; p->value.to<double>(tmp))
        {
            opt = tmp;
            return true;
        }
        return false;
    };

    if (p->key == "injectionnominalcapacity")
        return valueForOptional(this->injectionCapacity);

    if (p->key == "withdrawalnominalcapacity")
        return valueForOptional(this->withdrawalCapacity);

    if (p->key == "reservoircapacity")
        return valueForOptional(this->capacity);

    if (p->key == "efficiency")
        return p->value.to<double>(this->efficiencyFactor);

    if (p->key == "name")
        return p->value.to<std::string>(this->name);

    if (p->key == "storagecycle")
        return valueForOptional(this->storagecycle);

    if (p->key == "initiallevel")
    {
        if (p->value == "optim")
            return true;

        return valueForOptional(this->initialLevel);
    }

    if (p->key == "group")
    {
        if (auto it = Properties::ST_STORAGE_PROPERTY_GROUP_ENUM.find(p->value.c_str());
            it != Properties::ST_STORAGE_PROPERTY_GROUP_ENUM.end())
        {
            this->group = it->second;
            return true;
        }
        return false;
    }

    return false;
}

bool Properties::validate()
{
    auto checkMandatory = [this](const auto& prop, const std::string& label) {
        if (!prop.has_value())
        {
            logs.error() << "Property " << label << " is mandatory for short term storage "
                         << this->name;
            return false;
        }
        return true;
    };

    if (!checkMandatory(injectionCapacity, "injectionnominalcapacity"))
        return false;

    if (!checkMandatory(withdrawalCapacity, "withdrawalnominalcapacity"))
        return false;

    if (!checkMandatory(capacity, "reservoircapacity"))
        return false;

    if (!checkMandatory(storagecycle, "storagecycle"))
        return false;

    if (injectionCapacity < 0)
    {
        logs.error() << "Property injectionnominalcapacity must be >= 0 "
                     << "for short term storage " << name;
        return false;
    }

    if (withdrawalCapacity < 0)
    {
        logs.error() << "Property withdrawalnominalcapacity must be >= 0 "
                     << "for short term storage " << name;
        return false;
    }

    if (capacity < 0)
    {
        logs.error() << "Property reservoircapacity must be >= 0 "
                     << "for short term storage " << name;
        return false;
    }

    if (efficiencyFactor < 0)
    {
        logs.warning() << "Property efficiency must be >= 0 "
                       << "for short term storage " << name;
        efficiencyFactor = 0;
    }

    if (efficiencyFactor > 1)
    {
        logs.warning() << "Property efficiency must be <= 1 "
                       << "for short term storage " << name;
        efficiencyFactor = 1;
    }

    if (initialLevel.has_value())
    {
        if (initialLevel < 0)
        {
            logs.warning() << "initiallevel for cluster: " << name << " should be positive";
            initialLevel = 0;
        }

        if (initialLevel > capacity)
        {
            logs.warning() << "initiallevel for cluster: " << name
                           << " should be inferior to reservoir capacity: " << capacity.value();
            initialLevel = capacity;
        }
    }

    if (storagecycle.value() > 168 || storagecycle.value() < 1)
    {
        logs.warning() << "storagecycle for cluster: " << name << " should be <= 168 and >= 1";
        storagecycle = 168;
    }

    return true;
}

} // namespace Antares::Data::ShortTermStorage
