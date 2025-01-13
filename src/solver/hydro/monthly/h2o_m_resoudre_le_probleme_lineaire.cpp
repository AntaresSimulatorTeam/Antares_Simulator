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

#include <memory>

#include "antares/solver/hydro/monthly/h2o_m_donnees_annuelles.h"
#include "antares/solver/hydro/monthly/h2o_m_fonctions.h"

void H2O_M_ResoudreLeProblemeLineaire(DONNEES_ANNUELLES& DonneesAnnuelles, int NumeroDeReservoir)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesAnnuelles.ProblemeHydraulique;
    PROBLEME_LINEAIRE_PARTIE_VARIABLE& ProblemeLineairePartieVariable
      = ProblemeHydraulique.ProblemeLineairePartieVariable;
    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe = ProblemeHydraulique
                                                                  .ProblemeLineairePartieFixe;

    PROBLEME_SPX* ProbSpx = ProblemeHydraulique.ProblemeSpx[NumeroDeReservoir];
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

    Probleme->CoutLineaire = ProblemeLineairePartieFixe.CoutLineaireBruite.data();
    Probleme->X = ProblemeLineairePartieVariable.X.data();
    Probleme->Xmin = ProblemeLineairePartieVariable.Xmin.data();
    Probleme->Xmax = ProblemeLineairePartieVariable.Xmax.data();
    Probleme->NombreDeVariables = ProblemeLineairePartieFixe.NombreDeVariables;
    Probleme->TypeDeVariable = ProblemeLineairePartieFixe.TypeDeVariable.data();

    Probleme->NombreDeContraintes = ProblemeLineairePartieFixe.NombreDeContraintes;
    Probleme->IndicesDebutDeLigne = ProblemeLineairePartieFixe.IndicesDebutDeLigne.data();
    Probleme->NombreDeTermesDesLignes = ProblemeLineairePartieFixe.NombreDeTermesDesLignes.data();
    Probleme->IndicesColonnes = ProblemeLineairePartieFixe.IndicesColonnes.data();
    Probleme->CoefficientsDeLaMatriceDesContraintes = ProblemeLineairePartieFixe
                                                        .CoefficientsDeLaMatriceDesContraintes
                                                        .data();
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

    Probleme->CoutsMarginauxDesContraintes = ProblemeLineairePartieVariable
                                               .CoutsMarginauxDesContraintes.data();
    Probleme->CoutsReduits = ProblemeLineairePartieVariable.CoutsReduits.data();

#ifndef NDEBUG
    if (PremierPassage)
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
        ProblemeHydraulique.ProblemeSpx[NumeroDeReservoir] = ProbSpx;
    }

    ProblemeLineairePartieVariable.ExistenceDUneSolution = Probleme->ExistenceDUneSolution;

    if (ProblemeLineairePartieVariable.ExistenceDUneSolution != OUI_SPX && PremierPassage
        && ProbSpx)
    {
        if (ProblemeLineairePartieVariable.ExistenceDUneSolution != SPX_ERREUR_INTERNE)
        {
            SPX_LibererProbleme(ProbSpx);

            ProbSpx = nullptr;
            PremierPassage = false;
            goto RESOLUTION;
        }
        else
        {
            DonneesAnnuelles.ResultatsValides = EMERGENCY_SHUT_DOWN;
            return;
        }
    }

    if (ProblemeLineairePartieVariable.ExistenceDUneSolution == OUI_SPX)
    {
        ProblemeHydraulique.CoutDeLaSolution = 0.0;
        for (int var = 0; var < Probleme->NombreDeVariables; var++)
        {
            ProblemeHydraulique.CoutDeLaSolution += ProblemeLineairePartieFixe.CoutLineaire[var]
                                                    * Probleme->X[var];
        }

        ProblemeHydraulique.CoutDeLaSolutionBruite = 0.0;
        for (int var = 0; var < Probleme->NombreDeVariables; var++)
        {
            ProblemeHydraulique.CoutDeLaSolutionBruite += ProblemeLineairePartieFixe
                                                            .CoutLineaireBruite[var]
                                                          * Probleme->X[var];
        }

        DonneesAnnuelles.ResultatsValides = OUI;

        for (int var = 0; var < ProblemeLineairePartieFixe.NombreDeVariables; var++)
        {
            double* pt = ProblemeLineairePartieVariable
                           .AdresseOuPlacerLaValeurDesVariablesOptimisees[var];
            if (pt)
            {
                *pt = ProblemeLineairePartieVariable.X[var];
            }
        }
    }
    else
    {
        DonneesAnnuelles.ResultatsValides = NON;
        return;
    }

    return;
}
