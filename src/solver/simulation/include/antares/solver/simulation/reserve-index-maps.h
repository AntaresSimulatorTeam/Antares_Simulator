// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <map>
#include <string>
#include <type_traits>
#include <utility>

#include <boost/bimap.hpp>

#include <antares/study/fwd.h>

namespace Antares::Solver::Simulation
{
//! Lookup tables mapping reserve identifiers to their solver-side participation indices.
//! Built from a weekly problem (PROBLEME_HEBDO) and consumed by the output variables.
struct ReserveIndexMaps
{
    //! Name of the reserve
    using ReserveName = std::string;
    //! ID of a reserve, obtained by transforming its name
    using ReserveID = std::string;

    struct AreaReserveIndexMap
    {
        boost::bimap<std::pair<ReserveID, std::string>, int> thermalClusters;
        boost::bimap<std::pair<ReserveID, std::string>, int> STStorageClusters;
        boost::bimap<ReserveID, int> Hydro;

        AreaReserveIndexMap() = default;
        AreaReserveIndexMap(const AreaReserveIndexMap&) = default;
        AreaReserveIndexMap& operator=(const AreaReserveIndexMap&) = default;

        // boost::bimap does not guarantee a noexcept move, so we swap into a
        // default-constructed instance instead of relying on its move ctor/assign.
        AreaReserveIndexMap(AreaReserveIndexMap&& other) noexcept
        {
            thermalClusters.swap(other.thermalClusters);
            STStorageClusters.swap(other.STStorageClusters);
            Hydro.swap(other.Hydro);
        }

        AreaReserveIndexMap& operator=(AreaReserveIndexMap&& other) noexcept
        {
            thermalClusters.swap(other.thermalClusters);
            STStorageClusters.swap(other.STStorageClusters);
            Hydro.swap(other.Hydro);
            return *this;
        }
    };

    //! Reserve participation indices, per area
    std::map<Data::AreaName, AreaReserveIndexMap> participationIndexMaps;
    //! Reserve display name, per reserve ID
    std::map<ReserveID, ReserveName> idToName;

    ReserveIndexMaps() = default;
    ReserveIndexMaps(const ReserveIndexMaps&) = default;
    ReserveIndexMaps& operator=(const ReserveIndexMaps&) = default;
    ReserveIndexMaps(ReserveIndexMaps&&) noexcept = default;
    ReserveIndexMaps& operator=(ReserveIndexMaps&&) noexcept = default;
};

static_assert(std::is_nothrow_move_constructible_v<ReserveIndexMaps>,
              "ReserveIndexMaps must be nothrow move constructible");
static_assert(std::is_nothrow_move_assignable_v<ReserveIndexMaps>,
              "ReserveIndexMaps must be nothrow move assignable");

} // namespace Antares::Solver::Simulation
