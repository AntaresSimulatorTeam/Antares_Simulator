/*
** Copyright 2007-2023 RTE
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
#ifndef __SOLVER_SIMULATION_EXTERN_H__
#define __SOLVER_SIMULATION_EXTERN_H__

#include "sim_structure_donnees.h"
#include "sim_structure_probleme_adequation.h"
#include "sim_structure_probleme_economique.h"

/* Valeurs generees de maniere aleatoire */
extern std::vector<std::vector<VALEURS_GENEREES_PAR_PAYS>> ValeursGenereesParPays;
extern std::vector<std::vector<NUMERO_CHRONIQUES_TIREES_PAR_PAYS*>> NumeroChroniquesTireesParPays;
extern NUMERO_CHRONIQUES_TIREES_PAR_INTERCONNEXION** NumeroChroniquesTireesParInterconnexion;
extern std::vector<std::map<std::string, unsigned>> NumeroChroniquesTireesParGroup;

/* Optimisation */
/*-Economique-*/
// extern PROBLEME_HEBDO                        problemeHebdo;
/*-Adequation-*/
extern PROBLEME_HORAIRE_ADEQUATION ProblemeHoraireAdequation;
extern VALEURS_ANNUELLES** ValeursAnnuellesAdequation;

/* Resultats */
/*-Economique-*/
extern std::vector<RESULTATS_PAR_INTERCONNEXION> ResultatsParInterconnexion;

#endif /* __SOLVER_SIMULATION_EXTERN_H__ */
