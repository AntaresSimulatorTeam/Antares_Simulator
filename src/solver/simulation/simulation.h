/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __SOLVER_SIMULATION_H__
#define __SOLVER_SIMULATION_H__

#include "../config.h"
#include <antares/study/study.h>
#include "sim_structure_donnees.h"
#include "sim_structure_probleme_economique.h"
#include "../variable/state.h"

#define OUI_ANTARES 1
#define NON_ANTARES 0
#define LINFINI_ANTARES 1.e+80
#define LINFINI_ENTIER 20000000

#define OPTIMISATION_LINEAIRE 1
#define OPTIMISATION_QUADRATIQUE 2

#define ZERO_PMAX 0.0
#define ZERO_PMIN 1.e-2

#define ZERO_RESERVE_J_MOINS1 1.e-2

void SIM_AllocationTableaux(void);

/*!
** \brief Alloue toutes les donnees d'un probleme hebdo
*/
void SIM_AllocationProblemeHebdo(PROBLEME_HEBDO& problem, int NombreDePasDeTemps);

/*!
** \brief Alloue et initialise un probleme hebdo
*/
void SIM_InitialisationProblemeHebdo(Antares::Data::Study& study,
                                     PROBLEME_HEBDO& problem,
                                     int NombreDePasDeTemps,
                                     uint numSpace);

void SIM_PreallocationTableaux(void);

void SIM_RenseignementProblemeHebdo(PROBLEME_HEBDO& problem,
                                    Antares::Solver::Variable::State& state,
                                    uint numSpace,
                                    const int);

void SIM_RenseignementProblemeHoraireAdequation(long, uint);

void SIM_RenseignementValeursPourTouteLAnnee(const Antares::Data::Study& study, uint numSpace);

void SIM_CalculFlotHoraireAdequation(void);

void SIM_InitialisationProblemeHoraireAdequation(void);

void SIM_AllocationProblemeHoraireAdequation(void);

void SIM_AlgorithmeDeFlot(void);

void SIM_InitialisationChainagePourAdequation(void);

void SIM_DesallocationProblemeHoraireAdequation(void);

void SIM_DesallocationTableaux(void);

void SIM_DesallocationProblemeHebdo(PROBLEME_HEBDO& problem);

void SIM_InitialisationResultats(void);

#endif /* __SOLVER_SIMULATION_H__ */
