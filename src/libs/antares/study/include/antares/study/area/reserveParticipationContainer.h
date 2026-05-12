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

#include <ranges>

#include <antares/study/area/reserveParticipation.h>

namespace Antares::Data
{
template<typename T>
concept DerivedFromReserveParticipationBase = std::is_base_of_v<ReserveParticipationBase, T>;

/// @brief Container for reserve participation generic for each type of cluster (thermal, Short
/// term, Long term)
template<DerivedFromReserveParticipationBase T>
class ReserveParticipationContainerBase
{
protected:
    std::map<ReserveID, T> reservesParticipations;
    std::vector<std::set<ReserveID>> reserveParticipationsSymmetries;

public:
    /// @brief Add a reserve participation to the container for a given reserve ID
    /// @param reserveID ID of the reserve for which the participation is added
    /// @param reserveParticipation reserve participation to add
    void addReserveParticipation(const ReserveID& reserveID, const T& reserveParticipation)
    {
        reservesParticipations.emplace(reserveID, reserveParticipation);
    }

    /// @brief Add a reserve participation symmetry to the container
    /// @param IDs IDs of the reserves for which the participation is symmetrical
    void addReserveParticipationSymmetry(std::set<ReserveID> reserveIDs)
    {
        if (reserveIDs.size() < 2)
        {
            logs.error() << "Must have at least two distinct reserves to participate to a symmetry";
        }

        // Check if entity is participating to reserves
        for (const auto& reserveID: reserveIDs)
        {
            if (!reservesParticipations.contains(reserveID))
            {
                logs.error() << "This entity is not participating to reserve " << reserveID;
            }
        }

        // Check if symmetry already existed
        if (std::find(reserveParticipationsSymmetries.begin(),
                      reserveParticipationsSymmetries.end(),
                      reserveIDs)
            != reserveParticipationsSymmetries.end())
        {
            logs.error() << "Detected duplicate in reserves symmetries";
        }
        reserveParticipationsSymmetries.push_back(reserveIDs);
    }

    /// @brief Get the indices of the lists that contains reserveParticipation to the reserve ID
    /// @param ID ID of the reserve for which the participation is searched
    /// @return indices of the lists that contains reserveParticipation to the reserve ID
    std::vector<int> symmetricalIndices(const ReserveID& ID) const
    {
        std::vector<int> indices;

        for (int i = 0; const auto& list: reserveParticipationsSymmetries)
        {
            if (list.contains(ID))
            {
                indices.push_back(i);
            }
            ++i;
        }

        return indices;
    }

    /// @brief Returns the number of symmetrical groups
    /// @return the number of symmetrical groups
    int getNbSymGroups() const
    {
        return reserveParticipationsSymmetries.size();
    }

    /// @brief Returns the number of reserves participations in the container
    std::size_t reserveParticipationsCount() const
    {
        return reservesParticipations.size();
    }

    /// @brief Returns true if cluster participates in a reserve with this name
    /// @param ID ID of the reserve
    /// @return true if cluster participates in a reserve with this name
    bool isParticipatingInReserve(const ReserveID& ID) const
    {
        return reservesParticipations.contains(ID);
    }

    auto getReservesParticipations() const noexcept
    {
        return std::views::all(reservesParticipations);
    }

    double reserveCost(const ReserveID& ID) const
    {
        return reservesParticipations.at(ID).participationCost;
    }
};

/// @brief Container for reserve participation specific to a cluster type
/// @tparam T type of the cluster
template<typename T>
class ReserveParticipationContainer: public ReserveParticipationContainerBase<T>
{
};

template<>
class ReserveParticipationContainer<ThermalClusterReserveParticipation>
    : public ReserveParticipationContainerBase<ThermalClusterReserveParticipation>
{
public:
    /// @brief Returns the cost of participation in a reserve when the cluster is off
    /// @param ID ID of the reserve
    /// @return the cost of participation in the reserve when the cluster is off
    double reserveCostOff(const ReserveID& ID) const
    {
        return reservesParticipations.at(ID).participationCostOff;
    }

    /// @brief Returns the maximum power that can be reserved when the cluster is off
    /// @param ID ID of the reserve
    /// @return the maximum power that can be reserved when the cluster is off
    double reserveMaxPowerOff(const ReserveID& ID) const
    {
        return reservesParticipations.at(ID).maxPowerOff;
    }

    /// @brief Returns the maximum power that can be reserved
    /// @param ID ID of the reserve
    /// @return the maximum power that can be reserved
    double reserveMaxPower(const ReserveID& ID) const
    {
        return reservesParticipations.at(ID).maxPower;
    }
};

template<>
class ReserveParticipationContainer<StorageClusterReserveParticipation>
    : public ReserveParticipationContainerBase<StorageClusterReserveParticipation>
{
public:
    /// @brief Returns the maximum release power that can be reserved
    /// @param ID ID of the reserve
    /// @return the maximum release power that can be reserved
    double reserveMaxRelease(const ReserveID& ID) const
    {
        return reservesParticipations.at(ID).maxRelease;
    }

    /// @brief Returns the maximum store power that can be reserved
    /// @param ID ID of the reserve
    /// @return the maximum store power that can be reserved
    double reserveMaxStore(const ReserveID& ID) const
    {
        return reservesParticipations.at(ID).maxStore;
    }
};

}; // namespace Antares::Data
