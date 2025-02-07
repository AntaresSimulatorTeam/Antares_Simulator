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
void SIM_AllocationProblemeHebdo(const Antares::Data::Study& study,
                                 PROBLEME_HEBDO& problem,
                                 uint NombreDePasDeTemps);

/*!
** \brief Alloue et initialise un probleme hebdo
*/
void SIM_InitialisationProblemeHebdo(Antares::Data::Study& study,
                                     PROBLEME_HEBDO& problem,
                                     unsigned int NombreDePasDeTemps,
                                     uint numspace);

void SIM_RenseignementProblemeHebdo(const Antares::Data::Study& study,
                                    PROBLEME_HEBDO& problem,
                                    uint weekInTheYear,
                                    const int,
                                    const Antares::HYDRO_VENTILATION_RESULTS&,
                                    const Antares::Data::Area::ScratchMap&);

#endif /* __SOLVER_SIMULATION_H__ */
