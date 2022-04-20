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

#include "h2o_j_donnees_mensuelles.h"
#include "h2o_j_fonctions.h"

void H2O_J_Free(DONNEES_MENSUELLES* DonneesMensuelles)
{
    int i;
    int NombreDeProblemes;
    PROBLEME_SPX* ProbSpx;

    PROBLEME_HYDRAULIQUE* ProblemeHydraulique;

    ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;
    NombreDeProblemes = ProblemeHydraulique->NombreDeProblemes;

    for (i = 0; i < NombreDeProblemes; i++)
    {
        free(ProblemeHydraulique->CorrespondanceDesVariables[i]->NumeroDeVariableTurbine);
        free(ProblemeHydraulique->CorrespondanceDesVariables[i]);

        free(ProblemeHydraulique->CorrespondanceDesContraintes[i]->NumeroDeContrainteSurXi);
        free(ProblemeHydraulique->CorrespondanceDesContraintes[i]);

        free(ProblemeHydraulique->ProblemeLineairePartieFixe[i]->CoutLineaire);
        free(ProblemeHydraulique->ProblemeLineairePartieFixe[i]->TypeDeVariable);
        free(ProblemeHydraulique->ProblemeLineairePartieFixe[i]->Sens);
        free(ProblemeHydraulique->ProblemeLineairePartieFixe[i]->IndicesDebutDeLigne);
        free(ProblemeHydraulique->ProblemeLineairePartieFixe[i]->NombreDeTermesDesLignes);
        free((ProblemeHydraulique->ProblemeLineairePartieFixe[i])
               ->CoefficientsDeLaMatriceDesContraintes);
        free(ProblemeHydraulique->ProblemeLineairePartieFixe[i]->IndicesColonnes);
        free(ProblemeHydraulique->ProblemeLineairePartieFixe[i]);

        free(ProblemeHydraulique->ProblemeLineairePartieVariable[i]->Xmin);
        free(ProblemeHydraulique->ProblemeLineairePartieVariable[i]->Xmax);
        free(ProblemeHydraulique->ProblemeLineairePartieVariable[i]->SecondMembre);
        free((ProblemeHydraulique->ProblemeLineairePartieVariable[i])
               ->AdresseOuPlacerLaValeurDesVariablesOptimisees);
        free(ProblemeHydraulique->ProblemeLineairePartieVariable[i]->X);
        free(ProblemeHydraulique->ProblemeLineairePartieVariable[i]->PositionDeLaVariable);
        free(ProblemeHydraulique->ProblemeLineairePartieVariable[i]->ComplementDeLaBase);
        free(ProblemeHydraulique->ProblemeLineairePartieVariable[i]->CoutsReduits);
        free(ProblemeHydraulique->ProblemeLineairePartieVariable[i]->CoutsMarginauxDesContraintes);
        free(ProblemeHydraulique->ProblemeLineairePartieVariable[i]);

        ProbSpx = (PROBLEME_SPX*)ProblemeHydraulique->ProblemeSpx[i];
        if (ProbSpx != NULL)
        {
            SPX_LibererProbleme(ProbSpx);
        }
    }

    free(ProblemeHydraulique->NbJoursDUnProbleme);
    free(ProblemeHydraulique->CorrespondanceDesVariables);
    free(ProblemeHydraulique->CorrespondanceDesContraintes);
    free(ProblemeHydraulique->ProblemeLineairePartieFixe);
    free(ProblemeHydraulique->ProblemeLineairePartieVariable);
    free(ProblemeHydraulique->ProblemeSpx);
    free(ProblemeHydraulique->Probleme);
    free(ProblemeHydraulique);

    free(DonneesMensuelles->TurbineMax);
    free(DonneesMensuelles->TurbineCible);
    free(DonneesMensuelles->Turbine);
    free(DonneesMensuelles);

    return;
}
