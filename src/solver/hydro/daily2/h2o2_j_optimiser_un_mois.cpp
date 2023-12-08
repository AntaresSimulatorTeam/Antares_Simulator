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
** SPDX-License-Identifier: MPL 2.0
*/

#include "h2o2_j_donnees_mensuelles.h"
#include "h2o2_j_fonctions.h"

void H2O2_J_OptimiserUnMois(DONNEES_MENSUELLES_ETENDUES& DonneesMensuelles)
{
    auto& ProblemeHydrauliqueEtendu = DonneesMensuelles.ProblemeHydrauliqueEtendu;

    int NumeroDeProbleme = -1;
    for (int i = 0; i < ProblemeHydrauliqueEtendu.NombreDeProblemes; i++)
    {
        if (DonneesMensuelles.NombreDeJoursDuMois
            == ProblemeHydrauliqueEtendu.NbJoursDUnProbleme[i])
        {
            NumeroDeProbleme = i;
            break;
        }
    }

    if (NumeroDeProbleme < 0)
    {
        DonneesMensuelles.ResultatsValides = EMERGENCY_SHUT_DOWN;
        return;
    }

    DonneesMensuelles.ResultatsValides = NON;

    H2O2_J_InitialiserLeSecondMembre(DonneesMensuelles, NumeroDeProbleme);
    H2O2_J_InitialiserLesBornesdesVariables(DonneesMensuelles, NumeroDeProbleme);
    H2O2_J_ResoudreLeProblemeLineaire(DonneesMensuelles, NumeroDeProbleme);

    return;
}
