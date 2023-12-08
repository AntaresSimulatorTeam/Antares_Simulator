/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
**
** SPDX-License-Identifier: MPL 2.0
*/
#ifndef __SOLVER_SIMULATION_H__
#define __SOLVER_SIMULATION_H__

#include "../config.h"
#include "sim_structure_donnees.h"
#include <antares/study/study.h>
#include "../solver/hydro/management.h"

struct PROBLEME_HEBDO;

void SIM_AllocationTableaux(const Antares::Data::Study& study);

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
                                     int NombreDePasDeTemps,
                                     uint numSpace);

void SIM_PreallocationTableaux(void);

void SIM_RenseignementProblemeHebdo(const Antares::Data::Study& study,
                                    PROBLEME_HEBDO& problem,
                                    uint weekInTheYear,
                                    uint numSpace,
                                    const int,
                                    const HYDRO_VENTILATION_RESULTS&);

void SIM_RenseignementProblemeHoraireAdequation(uint);

void SIM_CalculFlotHoraireAdequation(void);

void SIM_InitialisationProblemeHoraireAdequation(void);

void SIM_AllocationProblemeHoraireAdequation(void);

void SIM_InitialisationChainagePourAdequation(void);

void SIM_DesallocationProblemeHoraireAdequation(void);

#endif /* __SOLVER_SIMULATION_H__ */
