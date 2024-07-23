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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#ifndef __SOLVER_H2O_J_FONCTIONS__
#define __SOLVER_H2O_J_FONCTIONS__

DONNEES_MENSUELLES* H2O_J_Instanciation(void);

void H2O_J_OptimiserUnMois(DONNEES_MENSUELLES*);
void H2O_J_Free(DONNEES_MENSUELLES*);
void H2O_J_ConstruireLesContraintes(int,
                                    std::vector<int>&,
                                    int,
                                    int,
                                    std::vector<int>&,
                                    std::vector<char>&,
                                    std::vector<int>&,
                                    std::vector<double>&,
                                    std::vector<int>&,
                                    CORRESPONDANCE_DES_CONTRAINTES&);
void H2O_j_ConstruireLesVariables(int,
                                  std::vector<int>&,
                                  std::vector<double>&,
                                  std::vector<double>&,
                                  std::vector<int>&,
                                  std::vector<double*>&,
                                  CORRESPONDANCE_DES_VARIABLES&);
void H2O_J_InitialiserLesBornesdesVariables(DONNEES_MENSUELLES*, int);
void H2O_J_InitialiserLeSecondMembre(DONNEES_MENSUELLES*, int);
void H2O_J_ResoudreLeProblemeLineaire(DONNEES_MENSUELLES*, int);
void H2O_J_LisserLesSurTurbines(DONNEES_MENSUELLES*, int);
void H2O_J_AjouterBruitAuCout(DONNEES_MENSUELLES&);

#endif /* __SOLVER_H2O_J_FONCTIONS__ */
