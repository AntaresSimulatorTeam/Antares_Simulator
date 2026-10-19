// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_SIMULATION_H__
#define __SOLVER_SIMULATION_H__

#include <antares/study/study.h>
#include "antares/config/config.h"
#include "antares/solver/hydro/management/management.h"

#include "sim_structure_donnees.h"

struct PROBLEME_HEBDO;

/*!
** \brief Alloue toutes les donnees d'un probleme hebdo
*/
void SIM_AllocationProblemeHebdo(const Study& study,
                                 PROBLEME_HEBDO& problem,
                                 uint NombreDePasDeTemps);

/*!
** \brief Alloue et initialise un probleme hebdo
*/
void SIM_InitialisationProblemeHebdo(const Study& study,
                                     PROBLEME_HEBDO& problem,
                                     unsigned int NombreDePasDeTemps,
                                     uint numspace);

void SIM_RenseignementProblemeHebdo(const Study& study,
                                    PROBLEME_HEBDO& problem,
                                    uint weekInTheYear,
                                    const int,
                                    const Antares::HYDRO_VENTILATION_RESULTS&,
                                    const Area::ScratchMap&);

/*!
** \brief Copy the week's input-only generation series into the problem
**
** Fills \c problem.InputGenerationOfArea with one entry per simulation-table
** component (aggregated wind/solar or renewable clusters, run-of-river and
** misc-gen). Exposed for unit testing of the component naming.
*/
void fillInputGenerationSeries(const Study& study, PROBLEME_HEBDO& problem, int PasDeTempsDebut);

#endif /* __SOLVER_SIMULATION_H__ */
