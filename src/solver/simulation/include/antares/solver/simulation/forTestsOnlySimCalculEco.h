// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once
#include <antares/solver/simulation/sim_structure_probleme_economique.h>
#include <antares/study/area/area.h>

void importCapacityReservations(Antares::Data::AreaList& areas, PROBLEME_HEBDO& problem);

void importHydroReserves(Antares::Data::AreaList& areas, PROBLEME_HEBDO& problem);
