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
    int i;
    int NombreDeProblemes;
    PROBLEME_SPX* ProbSpx;

    PROBLEME_HYDRAULIQUE_ETENDU* ProblemeHydrauliqueEtendu;

    ProblemeHydrauliqueEtendu = DonneesMensuelles->ProblemeHydrauliqueEtendu;
    NombreDeProblemes = ProblemeHydrauliqueEtendu->NombreDeProblemes;

    for (i = 0; i < NombreDeProblemes; i++)
    {
        free(ProblemeHydrauliqueEtendu->CorrespondanceDesVariables[i]->NumeroVar_Turbine);
        free(ProblemeHydrauliqueEtendu->CorrespondanceDesVariables[i]->NumeroVar_niveauxFinJours);
        free(ProblemeHydrauliqueEtendu->CorrespondanceDesVariables[i]->NumeroVar_overflow);
        free(ProblemeHydrauliqueEtendu->CorrespondanceDesVariables[i]->NumeroVar_deviations);
        free(ProblemeHydrauliqueEtendu->CorrespondanceDesVariables[i]->NumeroVar_violations);
        free(ProblemeHydrauliqueEtendu->CorrespondanceDesVariables[i]);

        free(ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieFixe[i]->CoutLineaire);
        free(ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieFixe[i]->TypeDeVariable);
        free(ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieFixe[i]->Sens);
        free(ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieFixe[i]->IndicesDebutDeLigne);
        free((ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieFixe[i])
               ->NombreDeTermesDesLignes);
        free((ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieFixe[i])
               ->CoefficientsDeLaMatriceDesContraintes);
        free(ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieFixe[i]->IndicesColonnes);
        free(ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieFixe[i]);

        free(ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable[i]->Xmin);
        free(ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable[i]->Xmax);
        free(ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable[i]->SecondMembre);
        free((ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable[i])
               ->AdresseOuPlacerLaValeurDesVariablesOptimisees);
        free(ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable[i]->X);
        free((ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable[i])
               ->PositionDeLaVariable);
        free(
          ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable[i]->ComplementDeLaBase);
        free(ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable[i]->CoutsReduits);
        free((ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable[i])
               ->CoutsMarginauxDesContraintes);
        free(ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable[i]);

        ProbSpx = (PROBLEME_SPX*)ProblemeHydrauliqueEtendu->ProblemeSpx[i];
        if (ProbSpx != NULL)
        {
            SPX_LibererProbleme(ProbSpx);
        }
    }

    free(ProblemeHydrauliqueEtendu->NbJoursDUnProbleme);
    free(ProblemeHydrauliqueEtendu->CorrespondanceDesVariables);
    free(ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieFixe);
    free(ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable);
    free(ProblemeHydrauliqueEtendu->ProblemeSpx);
    free(ProblemeHydrauliqueEtendu->Probleme);
    free(ProblemeHydrauliqueEtendu);

    free(DonneesMensuelles->TurbineMax);
    free(DonneesMensuelles->TurbineCible);
    free(DonneesMensuelles->Turbine);

    free(DonneesMensuelles->niveauBas);
    free(DonneesMensuelles->niveauxFinJours);
    free(DonneesMensuelles->apports);
    free(DonneesMensuelles->overflows);
    free(DonneesMensuelles->deviations);
    free(DonneesMensuelles->violations);

    free(DonneesMensuelles);

    return;
}
