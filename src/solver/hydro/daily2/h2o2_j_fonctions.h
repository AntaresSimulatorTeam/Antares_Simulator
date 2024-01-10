/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
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
*/

#ifndef __SOLVER_H2O2_J_FONCTIONS__
#define __SOLVER_H2O2_J_FONCTIONS__

DONNEES_MENSUELLES_ETENDUES H2O2_J_Instanciation();
void H2O2_J_OptimiserUnMois(DONNEES_MENSUELLES_ETENDUES&);
void H2O2_J_Free(DONNEES_MENSUELLES_ETENDUES&);
void H2O2_J_ConstruireLesContraintes(int,
                                     std::vector<int>&,
                                     std::vector<char>&,
                                     std::vector<int>&,
                                     std::vector<double>&,
                                     std::vector<int>&,
                                     CORRESPONDANCE_DES_VARIABLES_PB_ETENDU&);
void H2O2_j_ConstruireLesVariables(int,
                                   DONNEES_MENSUELLES_ETENDUES&,
                                   std::vector<double>&,
                                   std::vector<double>&,
                                   std::vector<int>&,
                                   std::vector<double*>&,
                                   CORRESPONDANCE_DES_VARIABLES_PB_ETENDU&);
void H2O2_J_apply_costs(const Hydro_problem_costs&, DONNEES_MENSUELLES_ETENDUES&);
void H2O2_J_InitialiserLesBornesdesVariables(DONNEES_MENSUELLES_ETENDUES&, int);
void H2O2_J_InitialiserLeSecondMembre(DONNEES_MENSUELLES_ETENDUES&, int);
void H2O2_J_ResoudreLeProblemeLineaire(DONNEES_MENSUELLES_ETENDUES&, int);

#endif /* __SOLVER_H2O2_J_FONCTIONS__ */
