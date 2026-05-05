// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <vector>

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

/*!
 * \brief Deep-copy \p src into a new PROBLEME_HEBDO, then override the initial
 *        hydro reservoir level for each area with the precomputed value for this
 *        specific week.  The returned struct is fully independent of \p src and
 *        safe to use from a separate thread.
 *
 * \param src                      Source PROBLEME_HEBDO (fully initialised).
 * \param weekIndex                Zero-based week index within the year.
 * \param precomputedInitialLevels One entry per area: the initial reservoir
 *                                 level (in MWh) to inject for this week.
 * \return A deep copy of \p src with \c previousSimulationFinalLevel overridden
 *         and \c weekInTheYear set to \p weekIndex.
 */
PROBLEME_HEBDO cloneProblemHebdoForWeek(
    const PROBLEME_HEBDO& src,
    uint weekIndex,
    const std::vector<double>& precomputedInitialLevels);
