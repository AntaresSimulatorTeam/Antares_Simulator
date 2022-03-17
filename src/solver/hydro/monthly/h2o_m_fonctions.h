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
#ifndef __SOLVER_H2O_M_FONCTIONS__
#define __SOLVER_H2O_M_FONCTIONS__

#ifdef __cplusplus
extern "C"
{
#endif

    DONNEES_ANNUELLES* H2O_M_Instanciation(int);
    void H2O_M_OptimiserUneAnnee(DONNEES_ANNUELLES*, int);
    void H2O_M_Free(DONNEES_ANNUELLES*);

    void H2O_M_ConstruireLesContraintes(DONNEES_ANNUELLES*);
    void H2O_M_ConstruireLesVariables(DONNEES_ANNUELLES*);
    void H2O_M_InitialiserBornesEtCoutsDesVariables(DONNEES_ANNUELLES*);
    void H2O_M_InitialiserLeSecondMembre(DONNEES_ANNUELLES*);
    void H2O_M_ResoudreLeProblemeLineaire(DONNEES_ANNUELLES*, int);
    void H2O_M_AjouterBruitAuCout(DONNEES_ANNUELLES*);
#ifdef __cplusplus
}
#endif

#endif /* __SOLVER_H2O_M_FONCTIONS__ */
