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

#include "h2o_j_donnees_mensuelles.h"
#include "h2o_j_fonctions.h"

DONNEES_MENSUELLES* H2O_J_Instanciation(void)
{
    int i;
    int* NbJoursDUnProbleme;
    int NombreDeProblemes;
    int NbPdt;
    int j;
    int NombreDeVariables;
    int NombreDeContraintes;
    int NombreDeTermesAlloues;
    DONNEES_MENSUELLES* DonneesMensuelles;
    PROBLEME_HYDRAULIQUE* ProblemeHydraulique;

    CORRESPONDANCE_DES_VARIABLES** CorrespondanceDesVariables;
    CORRESPONDANCE_DES_CONTRAINTES** CorrespondanceDesContraintes;
    PROBLEME_LINEAIRE_PARTIE_FIXE** ProblemeLineairePartieFixe;
    PROBLEME_LINEAIRE_PARTIE_VARIABLE** ProblemeLineairePartieVariable;
    CORRESPONDANCE_DES_VARIABLES* CorrVar;
    CORRESPONDANCE_DES_CONTRAINTES* CorrCnt;
    PROBLEME_LINEAIRE_PARTIE_FIXE* PlFixe;
    PROBLEME_LINEAIRE_PARTIE_VARIABLE* PlVariable;

    DonneesMensuelles = (DONNEES_MENSUELLES*)malloc(sizeof(DONNEES_MENSUELLES));
    if (DonneesMensuelles == NULL)
    {
        return (NULL);
    }

    DonneesMensuelles->ProblemeHydraulique
      = (PROBLEME_HYDRAULIQUE*)malloc(sizeof(PROBLEME_HYDRAULIQUE));
    if (DonneesMensuelles->ProblemeHydraulique == NULL)
    {
        return (NULL);
    }
    ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;

    ProblemeHydraulique->NombreDeProblemes = 4;

    ProblemeHydraulique->NbJoursDUnProbleme
      = (int*)malloc(ProblemeHydraulique->NombreDeProblemes * sizeof(int));
    if (ProblemeHydraulique->NbJoursDUnProbleme == NULL)
    {
        return (0);
    }
    NbJoursDUnProbleme = ProblemeHydraulique->NbJoursDUnProbleme;
    NbJoursDUnProbleme[0] = 28;
    NbJoursDUnProbleme[1] = 29;
    NbJoursDUnProbleme[2] = 30;
    NbJoursDUnProbleme[3] = 31;

    DonneesMensuelles->TurbineMax = (double*)malloc(NbJoursDUnProbleme[3] * sizeof(double));
    if (DonneesMensuelles->TurbineMax == NULL)
    {
        return (NULL);
    }
    DonneesMensuelles->TurbineMin = (double*)malloc(NbJoursDUnProbleme[3] * sizeof(double));
    if (DonneesMensuelles->TurbineMin == NULL)
    {
        return (NULL);
    }    
    DonneesMensuelles->TurbineCible = (double*)malloc(NbJoursDUnProbleme[3] * sizeof(double));
    if (DonneesMensuelles->TurbineCible == NULL)
    {
        return (NULL);
    }
    DonneesMensuelles->Turbine = (double*)malloc(NbJoursDUnProbleme[3] * sizeof(double));
    if (DonneesMensuelles->Turbine == NULL)
    {
        return (NULL);
    }

    NombreDeProblemes = ProblemeHydraulique->NombreDeProblemes;

    ProblemeHydraulique->CorrespondanceDesVariables = (CORRESPONDANCE_DES_VARIABLES**)malloc(
      NombreDeProblemes * sizeof(CORRESPONDANCE_DES_VARIABLES));
    if (ProblemeHydraulique->CorrespondanceDesVariables == NULL)
    {
        return (0);
    }
    ProblemeHydraulique->CorrespondanceDesContraintes = (CORRESPONDANCE_DES_CONTRAINTES**)malloc(
      NombreDeProblemes * sizeof(CORRESPONDANCE_DES_CONTRAINTES));
    if (ProblemeHydraulique->CorrespondanceDesContraintes == NULL)
    {
        return (0);
    }
    ProblemeHydraulique->ProblemeLineairePartieFixe = (PROBLEME_LINEAIRE_PARTIE_FIXE**)malloc(
      NombreDeProblemes * sizeof(PROBLEME_LINEAIRE_PARTIE_FIXE));
    if (ProblemeHydraulique->ProblemeLineairePartieFixe == NULL)
    {
        return (0);
    }
    ProblemeHydraulique->ProblemeLineairePartieVariable
      = (PROBLEME_LINEAIRE_PARTIE_VARIABLE**)malloc(NombreDeProblemes
                                                    * sizeof(PROBLEME_LINEAIRE_PARTIE_VARIABLE));
    if (ProblemeHydraulique->ProblemeLineairePartieVariable == NULL)
    {
        return (0);
    }

    ProblemeHydraulique->ProblemeSpx = (void**)malloc(NombreDeProblemes * sizeof(void*));
    if (ProblemeHydraulique->ProblemeSpx == NULL)
    {
        return (0);
    }
    for (i = 0; i < NombreDeProblemes; i++)
    {
        ProblemeHydraulique->ProblemeSpx[i] = NULL;
    }

    ProblemeHydraulique->Probleme = NULL;

    CorrespondanceDesVariables = ProblemeHydraulique->CorrespondanceDesVariables;
    CorrespondanceDesContraintes = ProblemeHydraulique->CorrespondanceDesContraintes;
    ProblemeLineairePartieFixe = ProblemeHydraulique->ProblemeLineairePartieFixe;
    ProblemeLineairePartieVariable = ProblemeHydraulique->ProblemeLineairePartieVariable;

    for (i = 0; i < NombreDeProblemes; i++)
    {
        CorrespondanceDesVariables[i]
          = (CORRESPONDANCE_DES_VARIABLES*)malloc(sizeof(CORRESPONDANCE_DES_VARIABLES));
        if (CorrespondanceDesVariables[i] == NULL)
        {
            return (0);
        }
        CorrespondanceDesContraintes[i]
          = (CORRESPONDANCE_DES_CONTRAINTES*)malloc(sizeof(CORRESPONDANCE_DES_CONTRAINTES));
        if (CorrespondanceDesContraintes[i] == NULL)
        {
            return (0);
        }
        ProblemeLineairePartieFixe[i]
          = (PROBLEME_LINEAIRE_PARTIE_FIXE*)malloc(sizeof(PROBLEME_LINEAIRE_PARTIE_FIXE));
        if (ProblemeLineairePartieFixe[i] == NULL)
        {
            return (0);
        }
        ProblemeLineairePartieVariable[i]
          = (PROBLEME_LINEAIRE_PARTIE_VARIABLE*)malloc(sizeof(PROBLEME_LINEAIRE_PARTIE_VARIABLE));
        if (ProblemeLineairePartieVariable[i] == NULL)
        {
            return (0);
        }
    }

    for (i = 0; i < NombreDeProblemes; i++)
    {
        NbPdt = NbJoursDUnProbleme[i];
        CorrVar = CorrespondanceDesVariables[i];
        CorrVar->NumeroDeVariableTurbine = (int*)malloc(NbPdt * sizeof(int));
        if (CorrVar->NumeroDeVariableTurbine == NULL)
        {
            return (0);
        }

        CorrCnt = CorrespondanceDesContraintes[i];
        CorrCnt->NumeroDeContrainteSurXi = (int*)malloc(NbPdt * sizeof(int));
        if (CorrCnt->NumeroDeContrainteSurXi == NULL)
        {
            return (0);
        }
        PlFixe = ProblemeLineairePartieFixe[i];

        NombreDeVariables = 0;
        NombreDeVariables += NbPdt;
        NombreDeVariables += 1;
        NombreDeVariables += 1;

        PlFixe->NombreDeVariables = NombreDeVariables;
        PlFixe->CoutLineaire = (double*)malloc(NombreDeVariables * sizeof(double));
        if (PlFixe->CoutLineaire == NULL)
        {
            return (0);
        }
        PlFixe->TypeDeVariable = (int*)malloc(NombreDeVariables * sizeof(int));
        if (PlFixe->TypeDeVariable == NULL)
        {
            return (0);
        }

        NombreDeContraintes = 0;
        NombreDeContraintes += 1;
        NombreDeContraintes += NbPdt;

        PlFixe->NombreDeContraintes = NombreDeContraintes;
        PlFixe->Sens = (char*)malloc(NombreDeContraintes * sizeof(char));
        if (PlFixe->Sens == NULL)
        {
            return (0);
        }
        PlFixe->IndicesDebutDeLigne = (int*)malloc(NombreDeContraintes * sizeof(int));
        if (PlFixe->IndicesDebutDeLigne == NULL)
        {
            return (0);
        }
        PlFixe->NombreDeTermesDesLignes = (int*)malloc(NombreDeContraintes * sizeof(int));
        if (PlFixe->NombreDeTermesDesLignes == NULL)
        {
            return (0);
        }

        NombreDeTermesAlloues = 0;
        NombreDeTermesAlloues += NbPdt;
        NombreDeTermesAlloues += 1;
        NombreDeTermesAlloues += (2 * NbPdt);

        PlFixe->NombreDeTermesAlloues = NombreDeTermesAlloues;
        PlFixe->CoefficientsDeLaMatriceDesContraintes
          = (double*)malloc(NombreDeTermesAlloues * sizeof(double));
        if (PlFixe->CoefficientsDeLaMatriceDesContraintes == NULL)
        {
            return (0);
        }
        PlFixe->IndicesColonnes = (int*)malloc(NombreDeTermesAlloues * sizeof(int));
        if (PlFixe->IndicesColonnes == NULL)
        {
            return (0);
        }

        PlVariable = ProblemeLineairePartieVariable[i];
        PlVariable->Xmin = (double*)malloc(NombreDeVariables * sizeof(double));
        if (PlVariable->Xmin == NULL)
        {
            return (0);
        }
        PlVariable->Xmax = (double*)malloc(NombreDeVariables * sizeof(double));
        if (PlVariable->Xmax == NULL)
        {
            return (0);
        }
        PlVariable->SecondMembre = (double*)malloc(NombreDeContraintes * sizeof(double));
        if (PlVariable->SecondMembre == NULL)
        {
            return (0);
        }
        PlVariable->AdresseOuPlacerLaValeurDesVariablesOptimisees
          = (double**)malloc(NombreDeVariables * sizeof(double*));
        if (PlVariable->AdresseOuPlacerLaValeurDesVariablesOptimisees == NULL)
        {
            return (0);
        }
        PlVariable->X = (double*)malloc(NombreDeVariables * sizeof(double));
        if (PlVariable->X == NULL)
        {
            return (0);
        }
        for (j = 0; j < NombreDeVariables; j++)
        {
            PlVariable->AdresseOuPlacerLaValeurDesVariablesOptimisees[j] = NULL;
        }
        PlVariable->PositionDeLaVariable = (int*)malloc(NombreDeVariables * sizeof(int));
        if (PlVariable->PositionDeLaVariable == NULL)
        {
            return (0);
        }
        PlVariable->ComplementDeLaBase = (int*)malloc(NombreDeContraintes * sizeof(int));
        if (PlVariable->ComplementDeLaBase == NULL)
        {
            return (0);
        }
        PlVariable->CoutsReduits = (double*)malloc(NombreDeVariables * sizeof(double));
        if (PlVariable->CoutsReduits == NULL)
        {
            return (0);
        }
        PlVariable->CoutsMarginauxDesContraintes
          = (double*)malloc(NombreDeContraintes * sizeof(double));
        if (PlVariable->CoutsMarginauxDesContraintes == NULL)
        {
            return (0);
        }
    }

    for (i = 0; i < NombreDeProblemes; i++)
    {
        H2O_j_ConstruireLesVariables(
          NbJoursDUnProbleme[i],
          CorrespondanceDesVariables[i]->NumeroDeVariableTurbine,
          ProblemeLineairePartieVariable[i]->Xmin,
          ProblemeLineairePartieVariable[i]->Xmax,
          ProblemeLineairePartieFixe[i]->TypeDeVariable,
          ProblemeLineairePartieVariable[i]->AdresseOuPlacerLaValeurDesVariablesOptimisees,
          CorrespondanceDesVariables[i]);

        H2O_J_ConstruireLesContraintes(
          NbJoursDUnProbleme[i],
          CorrespondanceDesVariables[i]->NumeroDeVariableTurbine,
          CorrespondanceDesVariables[i]->NumeroDeLaVariableMu,
          CorrespondanceDesVariables[i]->NumeroDeLaVariableXi,
          ProblemeLineairePartieFixe[i]->IndicesDebutDeLigne,
          ProblemeLineairePartieFixe[i]->Sens,
          ProblemeLineairePartieFixe[i]->NombreDeTermesDesLignes,
          ProblemeLineairePartieFixe[i]->CoefficientsDeLaMatriceDesContraintes,
          ProblemeLineairePartieFixe[i]->IndicesColonnes,
          CorrespondanceDesContraintes[i]);

        for (j = 0; j < ProblemeLineairePartieFixe[i]->NombreDeVariables; j++)
        {
            ProblemeLineairePartieFixe[i]->CoutLineaire[j] = 0.0;
        }

        ProblemeLineairePartieFixe[i]
          ->CoutLineaire[CorrespondanceDesVariables[i]->NumeroDeLaVariableMu]
          = 1.0;
        ProblemeLineairePartieFixe[i]
          ->CoutLineaire[CorrespondanceDesVariables[i]->NumeroDeLaVariableXi]
          = 1.0;
    }

    return (DonneesMensuelles);
}
