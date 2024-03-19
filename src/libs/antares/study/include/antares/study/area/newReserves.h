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
#ifndef __ANTARES_LIBS_STUDY_NEW_RESERVES_H__
#define __ANTARES_LIBS_STUDY_NEW_RESERVES_H__

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>
#include <yuni/core/string.h>
#include <vector>
#include <optional>

/// @brief Represents an area reserve using it's name, it's failure cost and it's spillage cost
struct AreaReserve
{
    std::string name;
    float failureCost = 0;
    float spillageCost = 0;
};

/// @brief Stores all the Reserves in two vectors for the up and down reserves
struct NewReserves
{
    std::map<std::string, AreaReserve> areaReservesUp;
    std::map<std::string, AreaReserve> areaReservesDown;

    /// @brief Check if the reserve name already exist in both the up and down reserves
    /// @param name
    /// @return true if the reserve already existed
    bool contains(std::string name)
    {
        return areaReservesUp.contains(name) || areaReservesDown.contains(name);
    }

    /// @brief Get a reserve from both the up and down reserves using its name
    /// @param name
    /// @return an optional of the reserve reference if the reserve was found, and a nullopt
    /// otherwise
    std::optional<std::reference_wrapper<AreaReserve>> getReserveFromName(std::string name)
    {
        if (areaReservesUp.contains(name))
        {
            return areaReservesUp.at(name);
        }
        else if (areaReservesDown.contains(name))
        {
            return areaReservesDown.at(name);
        }
        return std::nullopt;
    }
};

/// @brief Represents the cluster reserve participation to a given reserve
struct ClusterReserveParticipation
{
    AreaReserve& clusterReserve;
    float maxPower = 0;
    float participationCost = 0;

    ClusterReserveParticipation(AreaReserve& reserve, float power, float cost) :
     clusterReserve(reserve), maxPower(power), participationCost(cost)
    {
    }

    ClusterReserveParticipation& operator=(const ClusterReserveParticipation& other)
    {
        // Check for self-assignment
        if (this != &other)
        {
            // Copy the values from the other object
            clusterReserve = other.clusterReserve;
            maxPower = other.maxPower;
            participationCost = other.participationCost;
        }
        return *this;
    }
};

#endif // __ANTARES_LIBS_STUDY_LINKS_H__