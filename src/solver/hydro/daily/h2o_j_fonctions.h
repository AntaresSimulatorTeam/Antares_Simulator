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

#ifndef __SOLVER_H2O_J_FONCTIONS__
#define __SOLVER_H2O_J_FONCTIONS__

#ifdef __cplusplus
extern "C"
{
#endif

    DONNEES_MENSUELLES* H2O_J_Instanciation(void);
    void H2O_J_OptimiserUnMois(DONNEES_MENSUELLES*);
    char H2O_J_EcrireJeuDeDonneesLineaireAuFormatMPS(DONNEES_MENSUELLES*, FILE*);
    void H2O_J_Free(DONNEES_MENSUELLES*);
    void H2O_J_ConstruireLesContraintes(int,
                                        int*,
                                        int,
                                        int,
                                        int*,
                                        char*,
                                        int*,
                                        double*,
                                        int*,
                                        CORRESPONDANCE_DES_CONTRAINTES*);
    void H2O_j_ConstruireLesVariables(int,
                                      int*,
                                      double*,
                                      double*,
                                      int*,
                                      double**,
                                      CORRESPONDANCE_DES_VARIABLES*);
    void H2O_J_InitialiserLesBornesdesVariables(DONNEES_MENSUELLES*, int);
    void H2O_J_InitialiserLeSecondMembre(DONNEES_MENSUELLES*, int);
    void H2O_J_ResoudreLeProblemeLineaire(DONNEES_MENSUELLES*, int);
    void H2O_J_LisserLesSurTurbines(DONNEES_MENSUELLES*, int);

#ifdef __cplusplus
}
#endif

#endif /* __SOLVER_H2O_J_FONCTIONS__ */
