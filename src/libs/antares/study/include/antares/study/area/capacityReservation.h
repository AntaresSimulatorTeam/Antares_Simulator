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
#include <antares/series/series.h>
#include <antares/array/matrix.h>

/// @brief Represents an area capacity reservation using it's name, it's failure cost and it's spillage cost
struct CapacityReservation
{
    CapacityReservation() : failureCost(0), spillageCost(0), need(timeseriesNumbers) {}
    float failureCost;
    float spillageCost;
    Antares::Data::TimeSeries need;

private:
    Data::TimeSeriesNumbers timeseriesNumbers;
};

/// @brief Stores all the Capacity reservations in two vectors for the up and down reserves
struct AllCapacityReservations
{
    std::map<Data::ReserveName, CapacityReservation> areaCapacityReservationsUp;
    std::map<Data::ReserveName, CapacityReservation> areaCapacityReservationsDown;

    /// @brief Check if the capacity reservation name already exist in both the up and down reserves
    /// @param name
    /// @return true if the capacity reservation already existed
    bool contains(Data::ReserveName name)
    {
        return areaCapacityReservationsUp.contains(name) || areaCapacityReservationsDown.contains(name);
    }

    /// @brief Get a capacity reservation from both the up and down reserves using its name
    /// @param name
    /// @return an optional of the capacity reservation reference if the reserve was found, and a nullopt
    /// otherwise
    std::optional<std::reference_wrapper<CapacityReservation>> getReserveByName(Yuni::ShortString256 name)
    {
        if (areaCapacityReservationsUp.contains(name))
        {
            return areaCapacityReservationsUp.at(name);
        }
        else if (areaCapacityReservationsDown.contains(name))
        {
            return areaCapacityReservationsDown.at(name);
        }
        return std::nullopt;
    }

    /// @brief Get the number of capacityReservations in the area
    /// @return the number of capacityReservations in the area
    size_t size(){
        return areaCapacityReservationsUp.size() + areaCapacityReservationsDown.size();
    }

    /// @brief Returns lower case, no space string
    /// @param name
    /// @return A string usable for file naming
    static std::string toFilename(std::string name)
    {
        std::string file_name = name;
        std::replace(file_name.begin(), file_name.end(), ' ', '_');
        std::transform(file_name.begin(), file_name.end(), file_name.begin(), ::tolower);
        return file_name;
    }
};

/// @brief Represents the thermal cluster reserve participation to a given reserve
struct ThermalClusterReserveParticipation
{
    std::reference_wrapper<CapacityReservation> capacityReservation;
    float maxPower = 0;
    float participationCost = 0;
    float maxPowerOff = 0;
    float participationCostOff = 0;

    ThermalClusterReserveParticipation(std::reference_wrapper<CapacityReservation> reserve,
                                       float power,
                                       float cost,
                                       float powerOff,
                                       float costOff) :
     capacityReservation(reserve),
     maxPower(power),
     participationCost(cost),
     maxPowerOff(powerOff),
     participationCostOff(costOff)
    {
    }

    ThermalClusterReserveParticipation& operator=(const ThermalClusterReserveParticipation& other)
    {
        // Check for self-assignment
        if (this != &other)
        {
            // Copy the values from the other object
            capacityReservation = other.capacityReservation;
            maxPower = other.maxPower;
            participationCost = other.participationCost;
            maxPowerOff = other.maxPowerOff;
            participationCostOff = other.participationCostOff;
        }
        return *this;
    }
};

/// @brief Represents the Short Term Storage cluster reserve participation to a given reserve
struct STStorageClusterReserveParticipation
{
    std::reference_wrapper<CapacityReservation> capacityReservation;
    float maxTurbining = 0;
    float maxPumping = 0;
    float participationCost = 0;

    STStorageClusterReserveParticipation(std::reference_wrapper<CapacityReservation> reserve,
                                         float turbining,
                                         float pumping,
                                         float cost) :
     capacityReservation(reserve),
     maxTurbining(turbining),
     maxPumping(pumping),
     participationCost(cost)
    {
    }

    STStorageClusterReserveParticipation& operator=(
      const STStorageClusterReserveParticipation& other)
    {
        // Check for self-assignment
        if (this != &other)
        {
            // Copy the values from the other object
            capacityReservation = other.capacityReservation;
            maxTurbining = other.maxTurbining;
            maxPumping = other.maxPumping;
            participationCost = other.participationCost;
        }
        return *this;
    }
};

/// @brief Represents the Long Term Storage cluster reserve participation to a given reserve
struct LTStorageClusterReserveParticipation
{
    std::reference_wrapper<CapacityReservation> capacityReservation;
    float maxTurbining = 0;
    float maxPumping = 0;
    float participationCost = 0;

    LTStorageClusterReserveParticipation(std::reference_wrapper<CapacityReservation> reserve,
                                         float turbining,
                                         float pumping,
                                         float cost) :
     capacityReservation(reserve),
     maxTurbining(turbining),
     maxPumping(pumping),
     participationCost(cost)
    {
    }

    LTStorageClusterReserveParticipation& operator=(
      const LTStorageClusterReserveParticipation& other)
    {
        if (this != &other)
        {
            capacityReservation = other.capacityReservation;
            maxTurbining = other.maxTurbining;
            maxPumping = other.maxPumping;
            participationCost = other.participationCost;
        }
        return *this;
    }
};

#endif // __ANTARES_LIBS_STUDY_LINKS_H__