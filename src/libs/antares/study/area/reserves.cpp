/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include <boost/algorithm/string/case_conv.hpp>

#include <antares/study/area/reservesList.h>
#include "antares/study/area/area.h"

namespace Antares::Data
{
namespace // anonymous
{

bool readReservesAreaParameters(Area& area, const IniFile::Section& section)
{
    bool ret = true;
    for (auto* p = section.firstProperty; p; p = p->next)
    {
        std::string key = p->key;
        boost::algorithm::to_lower(key);

        if (key == "energy-activation-ratio-up")
        {
            if (!p->value.to<double>(
                  area.allCapacityReservations.value().maxGlobalEnergyActivationRatio.up))
            {
                logs.warning() << area.name
                               << " : invalid maximum energy activation ratio for UP reserves";
                ret = false;
            }
        }
        else if (key == "energy-activation-ratio-down")
        {
            if (!p->value.to<double>(
                  area.allCapacityReservations.value().maxGlobalEnergyActivationRatio.down))
            {
                logs.warning() << area.name
                               << " : invalid maximum energy activation ratio for "
                                  "DOWN reserves";
                ret = false;
            }
        }
        else if (key == "reference-activation-duration-up")
        {
            if (!p->value.to<int>(
                  area.allCapacityReservations.value().referenceGlobalActivationDuration.up))
            {
                logs.warning() << area.name
                               << " : invalid reference energy activation duration "
                                  "for UP reserves";
                ret = false;
            }
        }
        else if (key == "reference-activation-duration-down")
        {
            if (!p->value.to<int>(
                  area.allCapacityReservations.value().referenceGlobalActivationDuration.down))
            {
                logs.warning() << area.name
                               << " : invalid reference energy activation duration "
                                  "for DOWN reserves";
                ret = false;
            }
        }
        else
        {
            logs.warning() << area.name << " : invalid key " << key
                           << " inside global reserve parameters";
            ret = false;
        }
    }
    return ret;
}

bool readReserveParameters(const fs::path& folderInput, Area& area, const IniFile::Section& section)
{
    bool ret = true;
    if (area.allCapacityReservations.value().contains(section.name))
    {
        logs.error() << area.name << " : reserve name already exists for reserve " << section.name;
        return false;
    }

    CapacityReservation capacityReservation;

    for (auto* p = section.firstProperty; p; p = p->next)
    {
        std::string key = p->key;
        boost::algorithm::to_lower(key);

        if (key == "failure-cost")
        {
            if (!p->value.to<double>(capacityReservation.unsuppliedCost))
            {
                logs.warning() << area.name << " : invalid failure cost for reserve "
                               << section.name;
                ret = false;
            }
        }
        else if (key == "spillage-cost")
        {
            if (!p->value.to<double>(capacityReservation.spillageCost))
            {
                logs.warning() << area.name << " : invalid spillage cost for reserve "
                               << section.name;
                ret = false;
            }
        }
        else if (key == "power-activation-ratio")
        {
            if (!p->value.to<double>(capacityReservation.powerActivationRatio))
            {
                logs.warning() << area.name << " : invalid maximum activation ratio for reserve "
                               << section.name;
                ret = false;
            }
        }
        else if (key == "energy-activation-ratio")
        {
            if (!p->value.to<double>(capacityReservation.energyActivationRatio))
            {
                logs.warning() << area.name << " : invalid energy activation ratio for reserve "
                               << section.name;
                ret = false;
            }
        }
        else if (key == "reference-activation-duration")
        {
            if (!p->value.to<int>(capacityReservation.referenceActivationDuration))
            {
                logs.warning() << area.name
                               << " : invalid reference activation duration for reserve "
                               << section.name;
                ret = false;
            }
        }
        else if (key == "type")
        {
            if (p->value == "up")
            {
                capacityReservation.type = ReserveType::UP;
            }
            else if (p->value == "down")
            {
                capacityReservation.type = ReserveType::DOWN;
            }
            else
            {
                logs.warning() << area.name << " : invalid type for reserve " << section.name;
                ret = false;
            }
        }
        else
        {
            logs.warning() << area.name << " : invalid key " << key
                           << " inside reserve parameters for " << section.name;
            ret = false;
        }
    }
    fs::path filePath = folderInput / "reserves" / area.id.to<std::string>()
                        / (section.name.to<std::string>() + ".txt");
    capacityReservation.loadNeedFromFile(filePath);
    area.allCapacityReservations.value().areaCapacityReservations.emplace(section.name,
                                                                          capacityReservation);
    return ret;
}

} // anonymous namespace

namespace AccessForTests
{

void validateCapacityReservations(const Area& area)
{
    if (area.allCapacityReservations)
    {
        errorIfNegativeValue("maxGlobalEnergyActivationRatio up",
                             area.allCapacityReservations.value().maxGlobalEnergyActivationRatio.up,
                             area.name);
        errorIfNegativeValue(
          "maxGlobalEnergyActivationRatio down",
          area.allCapacityReservations.value().maxGlobalEnergyActivationRatio.down,
          area.name);
        errorIfNegativeValue(
          "referenceGlobalActivationDuration up",
          area.allCapacityReservations.value().referenceGlobalActivationDuration.up,
          area.name);
        errorIfNegativeValue(
          "referenceGlobalActivationDuration down",
          area.allCapacityReservations.value().referenceGlobalActivationDuration.down,
          area.name);
        for (const auto& [resName, capacityRes]:
             area.allCapacityReservations.value().areaCapacityReservations)
        {
            errorIfNegativeValue("energyActivationRatio",
                                 capacityRes.energyActivationRatio,
                                 area.name,
                                 std::nullopt,
                                 resName);
            errorIfNegativeValue("powerActivationRatio",
                                 capacityRes.powerActivationRatio,
                                 area.name,
                                 std::nullopt,
                                 resName);
            errorIfNegativeValue("referenceActivationDuration",
                                 capacityRes.referenceActivationDuration,
                                 area.name,
                                 std::nullopt,
                                 resName);
        }
    }
}

} // namespace AccessForTests

bool loadReservesParameters(fs::path& folderInput, Area& area)
{
    bool ret = true;
    fs::path reservesIni = folderInput / "reserves" / area.id.to<std::string>() / "reserves.ini";
    IniFile ini;
    area.allCapacityReservations.emplace();
    if (ini.open(reservesIni, false))
    {
        ini.each(
          [&](const IniFile::Section& section)
          {
              if (section.name == "globalparameters")
              {
                  ret = readReservesAreaParameters(area, section) && ret;
              }
              else
              {
                  ret = readReserveParameters(folderInput, area, section) && ret;
              }
          });

        AccessForTests::validateCapacityReservations(area);
    }
    return ret;
}
} // namespace Antares::Data
