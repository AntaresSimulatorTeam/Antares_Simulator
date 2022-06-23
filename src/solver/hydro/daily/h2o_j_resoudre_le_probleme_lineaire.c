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

#ifdef _MSC_VER
#define SNPRINTF sprintf_s
#else
#define SNPRINTF snprintf
#endif

void H2O_J_ResoudreLeProblemeLineaire(DONNEES_MENSUELLES* DonneesMensuelles, int NumeroDeProbleme)
{
    int Var;
    double* pt;
    char PremierPassage;

    PROBLEME_HYDRAULIQUE* ProblemeHydraulique;
    PROBLEME_LINEAIRE_PARTIE_VARIABLE* ProblemeLineairePartieVariable;
    PROBLEME_LINEAIRE_PARTIE_FIXE* ProblemeLineairePartieFixe;

    PROBLEME_SIMPLEXE* Probleme;
    PROBLEME_SPX* ProbSpx;

    PremierPassage = OUI;

    ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;

    ProblemeLineairePartieVariable
      = ProblemeHydraulique->ProblemeLineairePartieVariable[NumeroDeProbleme];
    ProblemeLineairePartieFixe = ProblemeHydraulique->ProblemeLineairePartieFixe[NumeroDeProbleme];

    ProbSpx = (PROBLEME_SPX*)ProblemeHydraulique->ProblemeSpx[NumeroDeProbleme];

    Probleme = (PROBLEME_SIMPLEXE*)ProblemeHydraulique->Probleme;
    if (Probleme == NULL)
    {
        Probleme = (PROBLEME_SIMPLEXE*)malloc(sizeof(PROBLEME_SIMPLEXE));
        if (Probleme == NULL)
        {
            DonneesMensuelles->ResultatsValides = EMERGENCY_SHUT_DOWN;
            return;
        }
        ProblemeHydraulique->Probleme = (void*)Probleme;
    }

RESOLUTION:

    if (ProbSpx == NULL)
    {
        Probleme->Contexte = SIMPLEXE_SEUL;
        Probleme->BaseDeDepartFournie = NON_SPX;
    }
    else
    {
        Probleme->Contexte = BRANCH_AND_BOUND_OU_CUT_NOEUD;

        Probleme->BaseDeDepartFournie = UTILISER_LA_BASE_DU_PROBLEME_SPX;

        SPX_ModifierLeVecteurSecondMembre(ProbSpx,
                                          ProblemeLineairePartieVariable->SecondMembre,
                                          ProblemeLineairePartieFixe->Sens,
                                          ProblemeLineairePartieFixe->NombreDeContraintes);
    }

    Probleme->NombreMaxDIterations = -1;
    Probleme->DureeMaxDuCalcul = -1.;

    Probleme->CoutLineaire = ProblemeLineairePartieFixe->CoutLineaire;
    Probleme->X = ProblemeLineairePartieVariable->X;
    Probleme->Xmin = ProblemeLineairePartieVariable->Xmin;
    Probleme->Xmax = ProblemeLineairePartieVariable->Xmax;
    Probleme->NombreDeVariables = ProblemeLineairePartieFixe->NombreDeVariables;
    Probleme->TypeDeVariable = ProblemeLineairePartieFixe->TypeDeVariable;

    Probleme->NombreDeContraintes = ProblemeLineairePartieFixe->NombreDeContraintes;
    Probleme->IndicesDebutDeLigne = ProblemeLineairePartieFixe->IndicesDebutDeLigne;
    Probleme->NombreDeTermesDesLignes = ProblemeLineairePartieFixe->NombreDeTermesDesLignes;
    Probleme->IndicesColonnes = ProblemeLineairePartieFixe->IndicesColonnes;
    Probleme->CoefficientsDeLaMatriceDesContraintes
      = ProblemeLineairePartieFixe->CoefficientsDeLaMatriceDesContraintes;
    Probleme->Sens = ProblemeLineairePartieFixe->Sens;
    Probleme->SecondMembre = ProblemeLineairePartieVariable->SecondMembre;

    Probleme->ChoixDeLAlgorithme = SPX_DUAL;

    Probleme->TypeDePricing = PRICING_STEEPEST_EDGE;
    Probleme->FaireDuScaling = OUI_SPX;
    Probleme->StrategieAntiDegenerescence = AGRESSIF;

    Probleme->PositionDeLaVariable = ProblemeLineairePartieVariable->PositionDeLaVariable;
    Probleme->NbVarDeBaseComplementaires = 0;
    Probleme->ComplementDeLaBase = ProblemeLineairePartieVariable->ComplementDeLaBase;

    Probleme->LibererMemoireALaFin = NON_SPX;

    Probleme->UtiliserCoutMax = NON_SPX;
    Probleme->CoutMax = 0.0;

    Probleme->CoutsMarginauxDesContraintes
      = ProblemeLineairePartieVariable->CoutsMarginauxDesContraintes;
    Probleme->CoutsReduits = ProblemeLineairePartieVariable->CoutsReduits;

#ifndef NDEBUG
    if (PremierPassage == OUI)
        Probleme->AffichageDesTraces = NON_SPX;
    else
        Probleme->AffichageDesTraces = OUI_SPX;
#else
    Probleme->AffichageDesTraces = NON_SPX;
#endif

    Probleme->NombreDeContraintesCoupes = 0;

    ProbSpx = SPX_Simplexe(Probleme, ProbSpx);

    if (ProbSpx != NULL)
    {
        ProblemeHydraulique->ProblemeSpx[NumeroDeProbleme] = (void*)ProbSpx;
    }

    ProblemeLineairePartieVariable->ExistenceDUneSolution = Probleme->ExistenceDUneSolution;

    if (ProblemeLineairePartieVariable->ExistenceDUneSolution != OUI_SPX && PremierPassage == OUI
        && ProbSpx != NULL)
    {
        if (ProblemeLineairePartieVariable->ExistenceDUneSolution != SPX_ERREUR_INTERNE)
        {
            SPX_LibererProbleme(ProbSpx);

            ProbSpx = NULL;
            PremierPassage = NON;
            goto RESOLUTION;
        }
        else
        {
            DonneesMensuelles->ResultatsValides = EMERGENCY_SHUT_DOWN;
            return;
        }
    }

    if (ProblemeLineairePartieVariable->ExistenceDUneSolution == OUI_SPX)
    {
        DonneesMensuelles->ResultatsValides = OUI;

        for (Var = 0; Var < ProblemeLineairePartieFixe->NombreDeVariables; Var++)
        {
            pt = ProblemeLineairePartieVariable->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var];
            if (pt != NULL)
            {
                *pt = ProblemeLineairePartieVariable->X[Var];
            }
        }
    }
    else
    {
        DonneesMensuelles->ResultatsValides = NON;
        return;
    }

    return;
}
