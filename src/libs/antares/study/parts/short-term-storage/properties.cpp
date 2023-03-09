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

#include "properties.h"

namespace Antares::Data::ShortTermStorage
{

const std::map<std::string, enum Group> Properties::stStoragePropertyGroupEnum =
{
    {"PSP_open", Group::PSP_open},
    {"PSP_closed", Group::PSP_closed},
    {"Pondage", Group::Pondage},
    {"Battery", Group::Battery},
    {"Other", Group::Other}
};

bool Properties::loadKey(const IniFile::Property* p)
{
    if (p->key == "injectionnominalcapacity")
        return p->value.to<double>(this->injectionCapacity);

    if (p->key == "withdrawalnominalcapacity")
        return p->value.to<double>(this->withdrawalCapacity);

    if (p->key == "reservoircapacity")
        return p->value.to<double>(this->capacity);

    if (p->key == "initiallevel")
        return p->value.to<double>(this->initialLevel);

    if (p->key == "efficiency")
        return p->value.to<double>(this->efficiencyFactor);

    if (p->key == "name")
        return p->value.to<std::string>(this->name);

    if (p->key == "storagecycle")
        return p->value.to<unsigned int>(this->cycleDuration);

    if (p->key == "group")
    {
        if (auto it = Properties::stStoragePropertyGroupEnum.find(p->value.c_str());
                it !=  Properties::stStoragePropertyGroupEnum.end())
        {
            this->group = it->second;
            return true;
        }
        return false;
    }

    return false;
}

} // namespace Antares::Data::ShortTermStorage
