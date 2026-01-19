// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/study/study.h>
#include "antares/config/config.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

void SIM_AllocationProblemeDonneesGenerales(PROBLEME_HEBDO& problem,
                                            const Antares::Data::Study& study,
                                            unsigned NombreDePasDeTemps);

void SIM_AllocationProblemePasDeTemps(PROBLEME_HEBDO& problem,
                                      const Antares::Data::Study& study,
                                      unsigned NombreDePasDeTemps);

void SIM_AllocationLinks(PROBLEME_HEBDO& problem,
                         const uint linkCount,
                         unsigned NombreDePasDeTemps);

void SIM_AllocationConstraints(PROBLEME_HEBDO& problem,
                               const Antares::Data::Study& study,
                               unsigned NombreDePasDeTemps);
void SIM_AllocationShortermStorageCumulation(PROBLEME_HEBDO& problem,
                                             const Antares::Data::Study& study);

void SIM_AllocateAreas(PROBLEME_HEBDO& problem,
                       const Antares::Data::Study& study,
                       unsigned NombreDePasDeTemps);
