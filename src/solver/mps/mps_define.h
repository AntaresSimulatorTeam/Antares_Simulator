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
#ifndef __SOLVER_MPS_DEFINE_H__
#define __SOLVER_MPS_DEFINE_H__

#define OUI_MPS 1
#define NON_MPS 0

#define REEL 1
#define ENTIER 2

#define NON_DEFINI 128

typedef struct
{
    long NentreesVar;
    long NentreesCnt;

    long NbVar;
    long NbCnt;

    long CoeffHaschCodeContraintes;
    long SeuilHaschCodeContraintes;
    long CoeffHaschCodeVariables;
    long SeuilHaschCodeVariables;

    long* Mdeb;
    double* A;
    long* Nuvar;
    long* Msui;
    long* Mder;
    long* NbTerm;
    double* B;
    char* SensDeLaContrainte;
    double* VariablesDualesDesContraintes;

    char** LabelDeLaContrainte;
    char** LabelDuSecondMembre;
    char* LabelDeLObjectif;

    char** LabelDeLaVariable;

    long* TypeDeVariable;
    long* TypeDeBorneDeLaVariable;
    double* U;
    double* L;
    double* Umin;
    double* Umax;

    long* FirstNomCnt;
    long* NomCntSuivant;

    long* FirstNomVar;
    long* NomVarSuivant;

} PROBLEME_MPS;

#endif /* __SOLVER_MPS_DEFINE_H__ */
