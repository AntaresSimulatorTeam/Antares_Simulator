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

#include "h2o2_j_donnees_mensuelles.h"
#include "h2o2_j_fonctions.h"

void H2O2_J_Instanciation(DONNEES_MENSUELLES_ETENDUES& DonneesMensuellesEtendues)
{
    int i;
    int* NbJoursDUnProbleme;
    int NombreDeProblemes;
    int NbPdt;
    int j;
    int NombreDeVariables;
    int NombreDeContraintes;
    int NombreDeTermesAlloues;
    PROBLEME_HYDRAULIQUE_ETENDU* ProblemeHydrauliqueEtendu;

    CORRESPONDANCE_DES_VARIABLES_PB_ETENDU** CorrespondanceDesVariables;
    PROBLEME_LINEAIRE_ETENDU_PARTIE_FIXE** ProblemeLineaireEtenduPartieFixe;
    PROBLEME_LINEAIRE_ETENDU_PARTIE_VARIABLE** ProblemeLineaireEtenduPartieVariable;
    CORRESPONDANCE_DES_VARIABLES_PB_ETENDU* CorrVar;
    PROBLEME_LINEAIRE_ETENDU_PARTIE_FIXE* PlFixe;
    PROBLEME_LINEAIRE_ETENDU_PARTIE_VARIABLE* PlVariable;

    DonneesMensuellesEtendues.ProblemeHydrauliqueEtendu
      = (PROBLEME_HYDRAULIQUE_ETENDU*)malloc(sizeof(PROBLEME_HYDRAULIQUE_ETENDU));
    if (DonneesMensuellesEtendues.ProblemeHydrauliqueEtendu == NULL)
        return;

    ProblemeHydrauliqueEtendu = DonneesMensuellesEtendues.ProblemeHydrauliqueEtendu;

    ProblemeHydrauliqueEtendu->NombreDeProblemes = 4;

    ProblemeHydrauliqueEtendu->NbJoursDUnProbleme
      = (int*)malloc(ProblemeHydrauliqueEtendu->NombreDeProblemes * sizeof(int));
    if (ProblemeHydrauliqueEtendu->NbJoursDUnProbleme == NULL)
        return;

    NbJoursDUnProbleme = ProblemeHydrauliqueEtendu->NbJoursDUnProbleme;
    NbJoursDUnProbleme[0] = 28;
    NbJoursDUnProbleme[1] = 29;
    NbJoursDUnProbleme[2] = 30;
    NbJoursDUnProbleme[3] = 31;

    DonneesMensuellesEtendues.TurbineMax = (double*)malloc(NbJoursDUnProbleme[3] * sizeof(double));
    if (DonneesMensuellesEtendues.TurbineMax == NULL)
        return;

    DonneesMensuellesEtendues.TurbineMin = (double*)malloc(NbJoursDUnProbleme[3] * sizeof(double));
    if (DonneesMensuellesEtendues.TurbineMin == NULL)
        return;

    DonneesMensuellesEtendues.TurbineCible
      = (double*)malloc(NbJoursDUnProbleme[3] * sizeof(double));
    if (DonneesMensuellesEtendues.TurbineCible == NULL)
        return;

    DonneesMensuellesEtendues.niveauBas = (double*)malloc(NbJoursDUnProbleme[3] * sizeof(double));
    if (DonneesMensuellesEtendues.niveauBas == NULL)
        return;

    DonneesMensuellesEtendues.apports = (double*)malloc(NbJoursDUnProbleme[3] * sizeof(double));
    if (DonneesMensuellesEtendues.apports == NULL)
        return;

    DonneesMensuellesEtendues.Turbine = (double*)malloc(NbJoursDUnProbleme[3] * sizeof(double));
    if (DonneesMensuellesEtendues.Turbine == NULL)
        return;

    DonneesMensuellesEtendues.niveauxFinJours
      = (double*)malloc(NbJoursDUnProbleme[3] * sizeof(double));
    if (DonneesMensuellesEtendues.niveauxFinJours == NULL)
        return;

    DonneesMensuellesEtendues.overflows = (double*)malloc(NbJoursDUnProbleme[3] * sizeof(double));
    if (DonneesMensuellesEtendues.overflows == NULL)
        return;

    DonneesMensuellesEtendues.deviations = (double*)malloc(NbJoursDUnProbleme[3] * sizeof(double));
    if (DonneesMensuellesEtendues.deviations == NULL)
        return;

    DonneesMensuellesEtendues.violations = (double*)malloc(NbJoursDUnProbleme[3] * sizeof(double));
    if (DonneesMensuellesEtendues.violations == NULL)
        return;

    NombreDeProblemes = ProblemeHydrauliqueEtendu->NombreDeProblemes;

    ProblemeHydrauliqueEtendu->CorrespondanceDesVariables
      = (CORRESPONDANCE_DES_VARIABLES_PB_ETENDU**)malloc(
        NombreDeProblemes * sizeof(CORRESPONDANCE_DES_VARIABLES_PB_ETENDU));
    if (ProblemeHydrauliqueEtendu->CorrespondanceDesVariables == NULL)
        return;

    ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieFixe
      = (PROBLEME_LINEAIRE_ETENDU_PARTIE_FIXE**)malloc(
        NombreDeProblemes * sizeof(PROBLEME_LINEAIRE_ETENDU_PARTIE_FIXE));
    if (ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieFixe == NULL)
        return;

    ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable
      = (PROBLEME_LINEAIRE_ETENDU_PARTIE_VARIABLE**)malloc(
        NombreDeProblemes * sizeof(PROBLEME_LINEAIRE_ETENDU_PARTIE_VARIABLE));
    if (ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable == NULL)
        return;

    ProblemeHydrauliqueEtendu->ProblemeSpx = (void**)malloc(NombreDeProblemes * sizeof(void*));
    if (ProblemeHydrauliqueEtendu->ProblemeSpx == NULL)
        return;

    for (i = 0; i < NombreDeProblemes; i++)
        ProblemeHydrauliqueEtendu->ProblemeSpx[i] = NULL;

    ProblemeHydrauliqueEtendu->Probleme = NULL;

    CorrespondanceDesVariables = ProblemeHydrauliqueEtendu->CorrespondanceDesVariables;
    ProblemeLineaireEtenduPartieFixe = ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieFixe;
    ProblemeLineaireEtenduPartieVariable
      = ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable;

    for (i = 0; i < NombreDeProblemes; i++)
    {
        CorrespondanceDesVariables[i] = (CORRESPONDANCE_DES_VARIABLES_PB_ETENDU*)malloc(
          sizeof(CORRESPONDANCE_DES_VARIABLES_PB_ETENDU));
        if (CorrespondanceDesVariables[i] == NULL)
            return;

        ProblemeLineaireEtenduPartieFixe[i] = (PROBLEME_LINEAIRE_ETENDU_PARTIE_FIXE*)malloc(
          sizeof(PROBLEME_LINEAIRE_ETENDU_PARTIE_FIXE));
        if (ProblemeLineaireEtenduPartieFixe[i] == NULL)
            return;

        ProblemeLineaireEtenduPartieVariable[i] = (PROBLEME_LINEAIRE_ETENDU_PARTIE_VARIABLE*)malloc(
          sizeof(PROBLEME_LINEAIRE_ETENDU_PARTIE_VARIABLE));
        if (ProblemeLineaireEtenduPartieVariable[i] == NULL)
            return;
    }

    for (i = 0; i < NombreDeProblemes; i++)
    {
        NbPdt = NbJoursDUnProbleme[i];

        CorrVar = CorrespondanceDesVariables[i];

        CorrVar->NumeroVar_Turbine = (int*)malloc(NbPdt * sizeof(int));
        if (CorrVar->NumeroVar_Turbine == NULL)
            return;

        CorrVar->NumeroVar_niveauxFinJours = (int*)malloc(NbPdt * sizeof(int));
        if (CorrVar->NumeroVar_niveauxFinJours == NULL)
            return;

        CorrVar->NumeroVar_overflow = (int*)malloc(NbPdt * sizeof(int));
        if (CorrVar->NumeroVar_overflow == NULL)
            return;

        CorrVar->NumeroVar_deviations = (int*)malloc(NbPdt * sizeof(int));
        if (CorrVar->NumeroVar_deviations == NULL)
            return;

        CorrVar->NumeroVar_violations = (int*)malloc(NbPdt * sizeof(int));
        if (CorrVar->NumeroVar_violations == NULL)
            return;

        PlFixe = ProblemeLineaireEtenduPartieFixe[i];

        NombreDeVariables = 0;
        NombreDeVariables += NbPdt;

        NombreDeVariables += NbPdt;
        NombreDeVariables += NbPdt;
        NombreDeVariables += NbPdt;
        NombreDeVariables += NbPdt;

        NombreDeVariables += 1;
        NombreDeVariables += 1;
        NombreDeVariables += 1;

        PlFixe->NombreDeVariables = NombreDeVariables;
        PlFixe->CoutLineaire = (double*)malloc(NombreDeVariables * sizeof(double));
        if (PlFixe->CoutLineaire == NULL)
            return;

        PlFixe->TypeDeVariable = (int*)malloc(NombreDeVariables * sizeof(int));
        if (PlFixe->TypeDeVariable == NULL)
            return;

        NombreDeContraintes = 0;
        NombreDeContraintes += NbPdt;

        NombreDeContraintes += 1;
        NombreDeContraintes += NbPdt;
        NombreDeContraintes += NbPdt;
        NombreDeContraintes += NbPdt;
        NombreDeContraintes += NbPdt;
        NombreDeContraintes += NbPdt;

        PlFixe->NombreDeContraintes = NombreDeContraintes;
        PlFixe->Sens = (char*)malloc(NombreDeContraintes * sizeof(char));
        if (PlFixe->Sens == NULL)
            return;

        PlFixe->IndicesDebutDeLigne = (int*)malloc(NombreDeContraintes * sizeof(int));
        if (PlFixe->IndicesDebutDeLigne == NULL)
            return;

        PlFixe->NombreDeTermesDesLignes = (int*)malloc(NombreDeContraintes * sizeof(int));
        if (PlFixe->NombreDeTermesDesLignes == NULL)
            return;

        NombreDeTermesAlloues = 0;
        NombreDeTermesAlloues += 3;
        NombreDeTermesAlloues += 4 * (NbPdt - 1);
        NombreDeTermesAlloues += 1;
        NombreDeTermesAlloues += NbPdt;
        NombreDeTermesAlloues += 2 * NbPdt;
        NombreDeTermesAlloues += 2 * NbPdt;
        NombreDeTermesAlloues += 2 * NbPdt;
        NombreDeTermesAlloues += 2 * NbPdt;
        NombreDeTermesAlloues += 2 * NbPdt;

        PlFixe->NombreDeTermesAlloues = NombreDeTermesAlloues;
        PlFixe->CoefficientsDeLaMatriceDesContraintes
          = (double*)malloc(NombreDeTermesAlloues * sizeof(double));
        if (PlFixe->CoefficientsDeLaMatriceDesContraintes == NULL)
            return;

        PlFixe->IndicesColonnes = (int*)malloc(NombreDeTermesAlloues * sizeof(int));
        if (PlFixe->IndicesColonnes == NULL)
            return;

        PlVariable = ProblemeLineaireEtenduPartieVariable[i];
        PlVariable->Xmin = (double*)malloc(NombreDeVariables * sizeof(double));
        if (PlVariable->Xmin == NULL)
            return;

        PlVariable->Xmax = (double*)malloc(NombreDeVariables * sizeof(double));
        if (PlVariable->Xmax == NULL)
            return;

        PlVariable->SecondMembre = (double*)malloc(NombreDeContraintes * sizeof(double));
        if (PlVariable->SecondMembre == NULL)
            return;

        PlVariable->AdresseOuPlacerLaValeurDesVariablesOptimisees
          = (double**)malloc(NombreDeVariables * sizeof(double*));
        if (PlVariable->AdresseOuPlacerLaValeurDesVariablesOptimisees == NULL)
            return;

        PlVariable->X = (double*)malloc(NombreDeVariables * sizeof(double));
        if (PlVariable->X == NULL)
            return;

        for (j = 0; j < NombreDeVariables; j++)
            PlVariable->AdresseOuPlacerLaValeurDesVariablesOptimisees[j] = NULL;

        PlVariable->PositionDeLaVariable = (int*)malloc(NombreDeVariables * sizeof(int));
        if (PlVariable->PositionDeLaVariable == NULL)
            return;

        PlVariable->ComplementDeLaBase = (int*)malloc(NombreDeContraintes * sizeof(int));
        if (PlVariable->ComplementDeLaBase == NULL)
            return;

        PlVariable->CoutsReduits = (double*)malloc(NombreDeVariables * sizeof(double));
        if (PlVariable->CoutsReduits == NULL)
            return;

        PlVariable->CoutsMarginauxDesContraintes
          = (double*)malloc(NombreDeContraintes * sizeof(double));
        if (PlVariable->CoutsMarginauxDesContraintes == NULL)
            return;
    }

    for (i = 0; i < NombreDeProblemes; i++)
    {
        H2O2_j_ConstruireLesVariables(
          NbJoursDUnProbleme[i],
          &DonneesMensuellesEtendues,
          ProblemeLineaireEtenduPartieVariable[i]->Xmin,
          ProblemeLineaireEtenduPartieVariable[i]->Xmax,
          ProblemeLineaireEtenduPartieFixe[i]->TypeDeVariable,
          ProblemeLineaireEtenduPartieVariable[i]->AdresseOuPlacerLaValeurDesVariablesOptimisees,
          CorrespondanceDesVariables[i]);

        H2O2_J_ConstruireLesContraintes(
          NbJoursDUnProbleme[i],
          ProblemeLineaireEtenduPartieFixe[i]->IndicesDebutDeLigne,
          ProblemeLineaireEtenduPartieFixe[i]->Sens,
          ProblemeLineaireEtenduPartieFixe[i]->NombreDeTermesDesLignes,
          ProblemeLineaireEtenduPartieFixe[i]->CoefficientsDeLaMatriceDesContraintes,
          ProblemeLineaireEtenduPartieFixe[i]->IndicesColonnes,
          CorrespondanceDesVariables[i]);
    }
}
