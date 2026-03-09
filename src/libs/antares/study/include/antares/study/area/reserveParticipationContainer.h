// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
    std::map<ReserveName, T> reservesParticipations;
    std::vector<std::set<ReserveName>> reserveParticipationsSymmetries;

public:
    /// @brief Add a reserve participation to the container for a given reserve name
    /// @param reserveName name of the reserve for which the participation is added
    /// @param reserveParticipation reserve participation to add
    void addReserveParticipation(const ReserveName& reserveName, const T& reserveParticipation)
    {
        reservesParticipations.emplace(reserveName, reserveParticipation);
    }

    /// @brief Add a reserve participation symmetry to the container
    /// @param names names of the reserves for which the participation is symmetrical
    void addReserveParticipationSymmetry(std::set<ReserveName> names)
    {
        if (names.size() < 2)
        {
            logs.error() << "Must have at least two distinct reserves to participate to a symmetry";
        }

        // Check if entity is participating to reserves
        for (const auto& name: names)
        {
            if (!reservesParticipations.contains(name))
            {
                logs.error() << "This entity is not participating to reserve " << name;
            }
        }

        // Check if symmetry already existed
        if (std::find(reserveParticipationsSymmetries.begin(),
                      reserveParticipationsSymmetries.end(),
                      names)
            != reserveParticipationsSymmetries.end())
        {
            logs.error() << "Detected duplicate in reserves symmetries";
        }
        reserveParticipationsSymmetries.push_back(names);
    }

    /// @brief Get the indices of the lists that contains reserveParticipation to the reserve name
    /// @param name name of the reserve for which the participation is searched
    /// @return indices of the lists that contains reserveParticipation to the reserve name
    std::vector<int> symmetricalIndices(const ReserveName& name) const
    {
        std::vector<int> indices;

        for (int i = 0; const auto& list: reserveParticipationsSymmetries)
        {
            if (list.contains(name))
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
    /// @param name name of the reserve
    /// @return true if cluster participates in a reserve with this name
    bool isParticipatingInReserve(const ReserveName& name) const
    {
        return reservesParticipations.contains(name);
    }

    auto getReservesParticipations() const noexcept
    {
        return std::views::all(reservesParticipations);
    }

    double reserveCost(const ReserveName& name) const
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
    double reserveCostOff(const ReserveName& name) const
    {
        return reservesParticipations.at(name).participationCostOff;
    }

    /// @brief Returns the maximum power that can be reserved when the cluster is off
    /// @param name name of the reserve
    /// @return the maximum power that can be reserved when the cluster is off
    double reserveMaxPowerOff(const ReserveName& name) const
    {
        return reservesParticipations.at(name).maxPowerOff;
    }

    /// @brief Returns the maximum power that can be reserved
    /// @param name name of the reserve
    /// @return the maximum power that can be reserved
    double reserveMaxPower(const ReserveName& name) const
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
    double reserveMaxRelease(const ReserveName& name) const
    {
        return reservesParticipations.at(name).maxRelease;
    }

    /// @brief Returns the maximum store power that can be reserved
    /// @param name name of the reserve
    /// @return the maximum store power that can be reserved
    double reserveMaxStore(const ReserveName& name) const
    {
        return reservesParticipations.at(name).maxStore;
    }
};

}; // namespace Antares::Data
