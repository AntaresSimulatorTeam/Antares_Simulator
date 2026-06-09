// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <antares/study/area/capacityReservation.h>

namespace Antares::Data
{

/// @brief Base struct for reserve participation
struct ReserveParticipationBase
{
    const CapacityReservation* capacityReservation;
    double participationCost = 0;
};

/// @brief Represents the thermal cluster reserve participation to a given reserve
struct ThermalClusterReserveParticipation: ReserveParticipationBase
{
    double maxPower = 0;
    double maxPowerOff = 0;
    double participationCostOff = 0;
};

/// @brief Represents the Short Term Storage cluster or hydro reserve participation to a given
/// reserve
struct StorageClusterReserveParticipation: ReserveParticipationBase
{
    double maxRelease = 0;
    double maxStore = 0;
};
} // namespace Antares::Data
