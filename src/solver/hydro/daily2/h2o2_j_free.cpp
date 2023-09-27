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

#ifdef __CPLUSPLUS
extern "C"
{
#endif

#include "spx_definition_arguments.h"
#include "spx_fonctions.h"

#ifdef __CPLUSPLUS
}
#endif

#include "h2o2_j_donnees_mensuelles.h"
#include "h2o2_j_fonctions.h"

void H2O2_J_Free(DONNEES_MENSUELLES_ETENDUES* DonneesMensuelles)
{
    PROBLEME_HYDRAULIQUE_ETENDU* ProblemeHydrauliqueEtendu = DonneesMensuelles->ProblemeHydrauliqueEtendu;
    int NombreDeProblemes = ProblemeHydrauliqueEtendu->NombreDeProblemes;

    for (int i = 0; i < NombreDeProblemes; i++)
    {
        PROBLEME_SPX* ProbSpx = (PROBLEME_SPX*)ProblemeHydrauliqueEtendu->ProblemeSpx[i];
        if (ProbSpx)
            SPX_LibererProbleme(ProbSpx);
    }

    free(ProblemeHydrauliqueEtendu->Probleme);
    free(ProblemeHydrauliqueEtendu);

    free(DonneesMensuelles);

    return;
}
