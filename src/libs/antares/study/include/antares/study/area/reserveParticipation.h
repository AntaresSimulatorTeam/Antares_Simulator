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
