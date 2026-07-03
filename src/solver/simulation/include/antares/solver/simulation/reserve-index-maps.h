// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <map>
#include <string>
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
    };

    //! Reserve participation indices, per area
    std::map<Data::AreaName, AreaReserveIndexMap> participationIndexMaps;
    //! Reserve display name, per reserve ID
    std::map<ReserveID, ReserveName> idToName;
};

} // namespace Antares::Solver::Simulation
