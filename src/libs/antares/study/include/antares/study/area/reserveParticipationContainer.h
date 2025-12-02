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

template<typename T>
concept DerivedFromReserveParticipationBase = std::is_base_of_v<ReserveParticipationBase, T>;

/// @brief Container for reserve participation generic for each type of cluster (thermal, Short
/// term, Long term)
template<DerivedFromReserveParticipationBase T>
class ReserveParticipationContainerBase
{
    struct ReserveParticipationWithName
    {
        T* reserveParticipation;
        ReserveName reserveName;
    };

protected:
    std::map<ReserveName, T> reservesParticipations;
    std::vector<std::set<ReserveName>> reserveParticipationsSymmetries;

public:
    /// @brief Add a reserve participation to the container for a given reserve name
    /// @param reserveName name of the reserve for which the participation is added
    /// @param reserveParticipation reserve participation to add
    void addReserveParticipation(ReserveName reserveName, const T& reserveParticipation)
    {
        reservesParticipations.emplace(reserveName, reserveParticipation);
    }

    bool checkSymmetricDuplicates(const std::set<ReserveName>& names) const
    {
        if (std::ranges::any_of(reserveParticipationsSymmetries,
                                [&](const auto& sym) { return sym == names; }))
        {
            throw std::invalid_argument("Detected duplicate in reserves symmetries");
        }
    }

    void checkIfReserverIsParticipating(const std::set<ReserveName>& names) const
    {
        for (const auto& name: names)
        {
            if (!reservesParticipations.contains(name))
            {
                throw std::out_of_range("This entity is not participating to reserve " + name);
            }
        }
    }

    /// @brief Add a reserve participation symmetry to the container
    /// @param names names of the reserves for which the participation is symmetrical
    void addReserveParticipationSymmetry(const std::set<ReserveName>& names)
    {
        // TODO check if more than 2 reserves can be symmetrical
        if (names.size() != 2)
        {
            throw std::out_of_range("Must have two distinct reserves to participate to a symmetry");
        }
        checkSymmetricDuplicates(names))
        

        checkIfReserverIsParticipating(names);

        reserveParticipationsSymmetries.push_back(names);
    }

    /// @brief Get the indices of the lists that contains reserveParticipation to the reserve name
    /// @param name name of the reserve for which the participation is searched
    /// @return indices of the lists that contains reserveParticipation to the reserve name
    std::vector<int> symmetricalIndices(const ReserveName& name) const
    {
        std::vector<int> indices;

        for (auto i = 0; i < reserveParticipationsSymmetries.size(); ++i)
        {
            if (const auto& symmetry = reserveParticipationsSymmetries.at(i);
                symmetry.contains(name))
            {
                indices.push_back(i);
            }
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
    /// @param name name of the reserve
    /// @return true if cluster participates in a reserve with this name
    bool isParticipatingInReserve(ReserveName name) const
    {
        return reservesParticipations.contains(name);
    }

    auto getReservesParticipations() const noexcept
    {
        return std::views::all(reservesParticipations);
    }

    double reserveCost(ReserveName name) const
    {
        return reservesParticipations.at(name).participationCost;
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
    /// @param name name of the reserve
    /// @return the cost of participation in the reserve when the cluster is off
    double reserveCostOff(ReserveName name) const
    {
        return reservesParticipations.at(name).participationCostOff;
    }

    /// @brief Returns the maximum power that can be reserved when the cluster is off
    /// @param name name of the reserve
    /// @return the maximum power that can be reserved when the cluster is off
    double reserveMaxPowerOff(ReserveName name) const
    {
        return reservesParticipations.at(name).maxPowerOff;
    }

    /// @brief Returns the maximum power that can be reserved
    /// @param name name of the reserve
    /// @return the maximum power that can be reserved
    double reserveMaxPower(ReserveName name) const
    {
        return reservesParticipations.at(name).maxPower;
    }
};

template<>
class ReserveParticipationContainer<StorageClusterReserveParticipation>
    : public ReserveParticipationContainerBase<StorageClusterReserveParticipation>
{
public:
    /// @brief Returns the maximum release power that can be reserved
    /// @param name name of the reserve
    /// @return the maximum release power that can be reserved
    double reserveMaxRelease(ReserveName name) const
    {
        return reservesParticipations.at(name).maxRelease;
    }

    /// @brief Returns the maximum store power that can be reserved
    /// @param name name of the reserve
    /// @return the maximum store power that can be reserved
    double reserveMaxStore(ReserveName name) const
    {
        return reservesParticipations.at(name).maxStore;
    }
};
