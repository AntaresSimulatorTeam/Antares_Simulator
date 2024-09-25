/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/hydro/daily2/h2o2_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily2/h2o2_j_fonctions.h"

void H2O2_J_ResoudreLeProblemeLineaire(DONNEES_MENSUELLES_ETENDUES& DonneesMensuelles,
                                       int NumeroDeProbleme)
{
    auto& ProblemeHydrauliqueEtendu = DonneesMensuelles.ProblemeHydrauliqueEtendu;

    auto& ProblemeLineaireEtenduPartieVariable = ProblemeHydrauliqueEtendu
                                                   .ProblemeLineaireEtenduPartieVariable
                                                     [NumeroDeProbleme];
    auto& ProblemeLineaireEtenduPartieFixe = ProblemeHydrauliqueEtendu
                                               .ProblemeLineaireEtenduPartieFixe[NumeroDeProbleme];

    PROBLEME_SPX* ProbSpx = ProblemeHydrauliqueEtendu.ProblemeSpx[NumeroDeProbleme];
    auto Probleme = std::make_unique<PROBLEME_SIMPLEXE>();

    bool premierPassage = true;

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
                                          ProblemeLineaireEtenduPartieVariable.SecondMembre.data(),
                                          ProblemeLineaireEtenduPartieFixe.Sens.data(),
                                          ProblemeLineaireEtenduPartieFixe.NombreDeContraintes);
    }

    Probleme->NombreMaxDIterations = -1;
    Probleme->DureeMaxDuCalcul = -1.;

    Probleme->CoutLineaire = ProblemeLineaireEtenduPartieFixe.CoutLineaire.data();
    Probleme->X = ProblemeLineaireEtenduPartieVariable.X.data();
    Probleme->Xmin = ProblemeLineaireEtenduPartieVariable.Xmin.data();
    Probleme->Xmax = ProblemeLineaireEtenduPartieVariable.Xmax.data();
    Probleme->NombreDeVariables = ProblemeLineaireEtenduPartieFixe.NombreDeVariables;
    Probleme->TypeDeVariable = ProblemeLineaireEtenduPartieFixe.TypeDeVariable.data();

    Probleme->NombreDeContraintes = ProblemeLineaireEtenduPartieFixe.NombreDeContraintes;
    Probleme->IndicesDebutDeLigne = ProblemeLineaireEtenduPartieFixe.IndicesDebutDeLigne.data();
    Probleme->NombreDeTermesDesLignes = ProblemeLineaireEtenduPartieFixe.NombreDeTermesDesLignes
                                          .data();
    Probleme->IndicesColonnes = ProblemeLineaireEtenduPartieFixe.IndicesColonnes.data();
    Probleme->CoefficientsDeLaMatriceDesContraintes = ProblemeLineaireEtenduPartieFixe
                                                        .CoefficientsDeLaMatriceDesContraintes
                                                        .data();
    Probleme->Sens = ProblemeLineaireEtenduPartieFixe.Sens.data();
    Probleme->SecondMembre = ProblemeLineaireEtenduPartieVariable.SecondMembre.data();

    Probleme->ChoixDeLAlgorithme = SPX_DUAL;

    Probleme->TypeDePricing = PRICING_STEEPEST_EDGE;
    Probleme->FaireDuScaling = OUI_SPX;
    Probleme->StrategieAntiDegenerescence = AGRESSIF;

    Probleme->PositionDeLaVariable = ProblemeLineaireEtenduPartieVariable.PositionDeLaVariable
                                       .data();
    Probleme->NbVarDeBaseComplementaires = 0;
    Probleme->ComplementDeLaBase = ProblemeLineaireEtenduPartieVariable.ComplementDeLaBase.data();

    Probleme->LibererMemoireALaFin = NON_SPX;

    Probleme->UtiliserCoutMax = NON_SPX;
    Probleme->CoutMax = 0.0;

    Probleme->CoutsMarginauxDesContraintes = ProblemeLineaireEtenduPartieVariable
                                               .CoutsMarginauxDesContraintes.data();
    Probleme->CoutsReduits = ProblemeLineaireEtenduPartieVariable.CoutsReduits.data();

#ifndef NDEBUG
    if (premierPassage)
    {
        Probleme->AffichageDesTraces = NON_SPX;
    }
    else
    {
        Probleme->AffichageDesTraces = OUI_SPX;
    }
#else
    Probleme->AffichageDesTraces = NON_SPX;
#endif

    Probleme->NombreDeContraintesCoupes = 0;

    ProbSpx = SPX_Simplexe(Probleme.get(), ProbSpx);

    if (ProbSpx)
    {
        ProblemeHydrauliqueEtendu.ProblemeSpx[NumeroDeProbleme] = ProbSpx;
    }

    ProblemeLineaireEtenduPartieVariable.ExistenceDUneSolution = Probleme->ExistenceDUneSolution;

    if (ProblemeLineaireEtenduPartieVariable.ExistenceDUneSolution != OUI_SPX && premierPassage
        && ProbSpx)
    {
        if (ProblemeLineaireEtenduPartieVariable.ExistenceDUneSolution != SPX_ERREUR_INTERNE)
        {
            SPX_LibererProbleme(ProbSpx);

            ProbSpx = nullptr;
            premierPassage = false;
            goto RESOLUTION;
        }
        else
        {
            DonneesMensuelles.ResultatsValides = EMERGENCY_SHUT_DOWN;
            return;
        }
    }

    if (ProblemeLineaireEtenduPartieVariable.ExistenceDUneSolution == OUI_SPX)
    {
        DonneesMensuelles.CoutSolution = 0.0;
        for (int Var = 0; Var < Probleme->NombreDeVariables; Var++)
        {
            DonneesMensuelles.CoutSolution += Probleme->CoutLineaire[Var] * Probleme->X[Var];
        }

        DonneesMensuelles.ResultatsValides = OUI;

        for (int Var = 0; Var < ProblemeLineaireEtenduPartieFixe.NombreDeVariables; Var++)
        {
            double* pt = ProblemeLineaireEtenduPartieVariable
                           .AdresseOuPlacerLaValeurDesVariablesOptimisees[Var];
            if (pt)
            {
                *pt = ProblemeLineaireEtenduPartieVariable.X[Var];
            }
        }
    }
    else
    {
        DonneesMensuelles.ResultatsValides = NON;
        return;
    }

    return;
}
