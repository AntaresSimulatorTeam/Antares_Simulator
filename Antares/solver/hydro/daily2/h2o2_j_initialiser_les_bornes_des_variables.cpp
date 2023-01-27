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

#include "h2o2_j_donnees_mensuelles.h"

void H2O2_J_InitialiserLesBornesdesVariables(DONNEES_MENSUELLES_ETENDUES* DonneesMensuelles,
                                             int NumeroDeProbleme)
{
    int Pdt;
    int NbPdt;
    int Var;
    double* Xmax;

    int* NumeroVar_Turbine;

    double* TurbineMax = DonneesMensuelles->TurbineMax;

    PROBLEME_HYDRAULIQUE_ETENDU* ProblemeHydrauliqueEtendu;
    CORRESPONDANCE_DES_VARIABLES_PB_ETENDU* CorrespondanceDesVariables;
    PROBLEME_LINEAIRE_ETENDU_PARTIE_VARIABLE* ProblemeLineaireEtenduPartieVariable;

    ProblemeHydrauliqueEtendu = DonneesMensuelles->ProblemeHydrauliqueEtendu;

    NbPdt = ProblemeHydrauliqueEtendu->NbJoursDUnProbleme[NumeroDeProbleme];

    CorrespondanceDesVariables
      = ProblemeHydrauliqueEtendu->CorrespondanceDesVariables[NumeroDeProbleme];
    ProblemeLineaireEtenduPartieVariable
      = ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable[NumeroDeProbleme];

    NumeroVar_Turbine = CorrespondanceDesVariables->NumeroVar_Turbine;

    Xmax = ProblemeLineaireEtenduPartieVariable->Xmax;
    for (Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        Var = NumeroVar_Turbine[Pdt];
        Xmax[Var] = TurbineMax[Pdt];
    }

    return;
}
