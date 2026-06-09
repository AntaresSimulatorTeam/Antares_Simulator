// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <vector>

#include <antares/logs/logs.h>
#include <antares/study/fwd.h>
#include <antares/utils/utils.h>

namespace fs = std::filesystem;
//! ID of a reserve, obtained by transforming its name
using ReserveID = std::string;

namespace Antares::Data
{

void inline errorIfNegativeValue(const std::string& propertyName,
                                 double value,
                                 const std::string& areaName,
                                 const std::optional<std::string>& clusterName = std::nullopt,
                                 const std::optional<std::string>& reserveID = std::nullopt)
{
    if (value < 0)
    {
        logs.error() << areaName << " : invalid " << propertyName << " can not be negative"
                     << (clusterName.has_value() ? ", for cluster " + clusterName.value() : "")
                     << (reserveID.has_value() ? ", for reserve " + reserveID.value() : "");
    }
}

/// @brief Represents an area capacity reservation using it's name, it's failure cost and it's
/// spillage cost
struct CapacityReservation
{
    ReserveType type{ReserveType::DOWN};
    double unsuppliedCost = 0.;
    double spillageCost = 0.;
    double powerActivationRatio = 0.;
    double energyActivationRatio = 1.;
    int referenceActivationDuration = 1.;
    std::vector<double> need = {};

    void loadNeedFromFile(const fs::path& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            throw std::runtime_error("Could not open " + path.string());
        }
        double x;
        std::vector<double> tmp;

        while (file >> x)
        {
            tmp.push_back(x);
        }

        if (!file.eof())
        {
            logs.error() << "Invalid numeric data in " << path.string();
            throw std::runtime_error("Invalid data in " + path.string());
        }

        need = std::move(tmp);
    }

    void setName(const AnyString& newname)
    {
        pName = newname;
        pID.clear();
        pID = transformNameIntoID(pName);
    }

    const std::string& id() const
    {
        return pID;
    }

    const std::string& name() const
    {
        return pName;
    }

protected:
    std::string pName;
    std::string pID;
};

/// @brief Stores all the Capacity reservations in two maps for the up and down reserves
struct AllCapacityReservations
{
    ReserveTypeData<double> maxGlobalEnergyActivationRatio{1., 1.};
    ReserveTypeData<int> referenceGlobalActivationDuration{1, 1};

    std::map<ReserveID, std::set<std::string /*name of the group*/>> reserveGroupPartThermal;
    std::map<ReserveID, std::set<std::string /*name of the group*/>> reserveGroupPartSTS;

    std::map<ReserveID, CapacityReservation> areaCapacityReservations;

    /// @brief Check if the capacity reservation name already exist in the reserves
    /// @param name
    /// @return true if the capacity reservation already existed
    bool contains(const ReserveID& id) const
    {
        return areaCapacityReservations.contains(id);
    }

    /// @brief Get a capacity reservation from both the up and down reserves using its ID
    /// @param id the ID of the reserve to get
    /// @return the capacity reservation reference if the reserve was found, and a nullptr
    /// otherwise
    const CapacityReservation* getReserveByID(const ReserveID& id) const
    {
        if (auto it = areaCapacityReservations.find(id); it != areaCapacityReservations.end())
        {
            return &it->second;
        }

        return nullptr;
    }

    /// @brief Get the number of capacityReservations in the area
    /// @return the number of capacityReservations in the area
    size_t size() const
    {
        return areaCapacityReservations.size();
    }

    /// @brief Return the name of the reserve at index i
    /// @param index the index of the reserve spilled/unsupplied variable
    /// @return the capacity reservation name and type if the reserve was found throw otherwise
    std::pair<UnsuppliedSpilled, ReserveID> reserveParticipationUnsuppliedSpilledAt(
      unsigned int index) const
    {
        unsigned int column = 0;
        for (const auto& reserveID: areaCapacityReservations | std::views::keys)
        {
            for (int indexUnsuppliedSpilled = 0;
                 indexUnsuppliedSpilled < static_cast<int>(UnsuppliedSpilled::Count);
                 indexUnsuppliedSpilled++)
            {
                if (column == index)
                {
                    return {static_cast<UnsuppliedSpilled>(indexUnsuppliedSpilled), reserveID};
                }
                column++;
            }
        }

        throw std::out_of_range("This reserve status index has not been found in all the "
                                "reserve participations");
    }

    /// @brief Gets the reserve name at the specified index in the map.
    /// @param index The zero-based position of the reserve in the map
    /// @return Reference to the reserve name at the specified index
    const ReserveID& getReserveAtIndex(size_t index) const
    {
        auto it = std::next(areaCapacityReservations.begin(), index);
        return it->first;
    }
};
} // namespace Antares::Data
