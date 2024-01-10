/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "h2o_j_donnees_mensuelles.h"
#include "h2o_j_fonctions.h"

#ifdef _MSC_VER
#define SNPRINTF sprintf_s
#else
#define SNPRINTF snprintf
#endif

#include <memory>

void H2O_J_ResoudreLeProblemeLineaire(DONNEES_MENSUELLES* DonneesMensuelles, int NumeroDeProbleme)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;

    PROBLEME_LINEAIRE_PARTIE_VARIABLE& ProblemeLineairePartieVariable
        = ProblemeHydraulique.ProblemeLineairePartieVariable[NumeroDeProbleme];

    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe
        = ProblemeHydraulique.ProblemeLineairePartieFixe[NumeroDeProbleme];

    PROBLEME_SPX* ProbSpx = ProblemeHydraulique.ProblemeSpx[NumeroDeProbleme];
    std::unique_ptr<PROBLEME_SIMPLEXE> Probleme = std::make_unique<PROBLEME_SIMPLEXE>();

    bool PremierPassage = true;

RESOLUTION:

    if (!ProbSpx)
    {
        Probleme->Contexte = SIMPLEXE_SEUL;
        Probleme->BaseDeDepartFournie = NON_SPX;
    }
    else
    {
        Probleme->Contexte = BRANCH_AND_BOUND_OU_CUT_NOEUD;

        Probleme->BaseDeDepartFournie = UTILISER_LA_BASE_DU_PROBLEME_SPX;

        SPX_ModifierLeVecteurSecondMembre(ProbSpx,
                                          ProblemeLineairePartieVariable.SecondMembre.data(),
                                          ProblemeLineairePartieFixe.Sens.data(),
                                          ProblemeLineairePartieFixe.NombreDeContraintes);
    }

    Probleme->NombreMaxDIterations = -1;
    Probleme->DureeMaxDuCalcul = -1.;

    Probleme->CoutLineaire = ProblemeLineairePartieFixe.CoutLineaire.data();
    Probleme->X = ProblemeLineairePartieVariable.X.data();
    Probleme->Xmin = ProblemeLineairePartieVariable.Xmin.data();
    Probleme->Xmax = ProblemeLineairePartieVariable.Xmax.data();
    Probleme->NombreDeVariables = ProblemeLineairePartieFixe.NombreDeVariables;
    Probleme->TypeDeVariable = ProblemeLineairePartieFixe.TypeDeVariable.data();

    Probleme->NombreDeContraintes = ProblemeLineairePartieFixe.NombreDeContraintes;
    Probleme->IndicesDebutDeLigne = ProblemeLineairePartieFixe.IndicesDebutDeLigne.data();
    Probleme->NombreDeTermesDesLignes = ProblemeLineairePartieFixe.NombreDeTermesDesLignes.data();
    Probleme->IndicesColonnes = ProblemeLineairePartieFixe.IndicesColonnes.data();
    Probleme->CoefficientsDeLaMatriceDesContraintes
      = ProblemeLineairePartieFixe.CoefficientsDeLaMatriceDesContraintes.data();
    Probleme->Sens = ProblemeLineairePartieFixe.Sens.data();
    Probleme->SecondMembre = ProblemeLineairePartieVariable.SecondMembre.data();

    Probleme->ChoixDeLAlgorithme = SPX_DUAL;

    Probleme->TypeDePricing = PRICING_STEEPEST_EDGE;
    Probleme->FaireDuScaling = OUI_SPX;
    Probleme->StrategieAntiDegenerescence = AGRESSIF;

    Probleme->PositionDeLaVariable = ProblemeLineairePartieVariable.PositionDeLaVariable.data();
    Probleme->NbVarDeBaseComplementaires = 0;
    Probleme->ComplementDeLaBase = ProblemeLineairePartieVariable.ComplementDeLaBase.data();

    Probleme->LibererMemoireALaFin = NON_SPX;

    Probleme->UtiliserCoutMax = NON_SPX;
    Probleme->CoutMax = 0.0;

    Probleme->CoutsMarginauxDesContraintes
      = ProblemeLineairePartieVariable.CoutsMarginauxDesContraintes.data();
    Probleme->CoutsReduits = ProblemeLineairePartieVariable.CoutsReduits.data();

#ifndef NDEBUG
    if (PremierPassage)
        Probleme->AffichageDesTraces = NON_SPX;
    else
        Probleme->AffichageDesTraces = OUI_SPX;
#else
    Probleme->AffichageDesTraces = NON_SPX;
#endif

    Probleme->NombreDeContraintesCoupes = 0;

    ProbSpx = SPX_Simplexe(Probleme.get(), ProbSpx);

    if (ProbSpx)
        ProblemeHydraulique.ProblemeSpx[NumeroDeProbleme] = ProbSpx;

    ProblemeLineairePartieVariable.ExistenceDUneSolution = Probleme->ExistenceDUneSolution;

    if (ProblemeLineairePartieVariable.ExistenceDUneSolution != OUI_SPX && PremierPassage && ProbSpx)
    {
        if (ProblemeLineairePartieVariable.ExistenceDUneSolution != SPX_ERREUR_INTERNE)
        {
            SPX_LibererProbleme(ProbSpx);

            ProbSpx = NULL;
            PremierPassage = false;
            goto RESOLUTION;
        }
        else
        {
            DonneesMensuelles->ResultatsValides = EMERGENCY_SHUT_DOWN;
            return;
        }
    }

    if (ProblemeLineairePartieVariable.ExistenceDUneSolution == OUI_SPX)
    {
        DonneesMensuelles->ResultatsValides = OUI;

        for (int Var = 0; Var < ProblemeLineairePartieFixe.NombreDeVariables; Var++)
        {
            double* pt = ProblemeLineairePartieVariable.AdresseOuPlacerLaValeurDesVariablesOptimisees[Var];
            if (pt)
                *pt = ProblemeLineairePartieVariable.X[Var];
        }
    }
    else
    {
        DonneesMensuelles->ResultatsValides = NON;
        return;
    }

    return;
}
