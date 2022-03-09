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
#ifndef __SOLVER_SIMULATION_EXTERN_H__
#define __SOLVER_SIMULATION_EXTERN_H__

#include "sim_structure_donnees.h"
#include "sim_structure_probleme_adequation.h"
#include "sim_structure_probleme_economique.h"

/* Structures Donnees lues a partir de fichiers */
extern DONNEES_PAR_PAYS** DonneesParPays;

/* Donnees simples lues a partir du fichier donnees_generales.txt */
extern char AppelEnModeSimulateur;

/* Valeurs generees de maniere aleatoire */
extern VALEURS_GENEREES_PAR_PAYS*** ValeursGenereesParPays;
extern NUMERO_CHRONIQUES_TIREES_PAR_PAYS*** NumeroChroniquesTireesParPays;
extern NUMERO_CHRONIQUES_TIREES_PAR_INTERCONNEXION** NumeroChroniquesTireesParInterconnexion;

/* Optimisation */
/*-Economique-*/
// extern PROBLEME_HEBDO                        ProblemeHebdo;
/*-Adequation-*/
extern PROBLEME_HORAIRE_ADEQUATION ProblemeHoraireAdequation;
extern VALEURS_ANNUELLES** ValeursAnnuellesAdequation;

/* Resultats */
/*-Economique-*/
extern RESULTATS_PAR_INTERCONNEXION** ResultatsParInterconnexion;
extern RESULTATS_PAR_CONTRAINTE_COUPLANTE** ResultatsParContrainteCouplante;

#endif /* __SOLVER_SIMULATION_EXTERN_H__ */
