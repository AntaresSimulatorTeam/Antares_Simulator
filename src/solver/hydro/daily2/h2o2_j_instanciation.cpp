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

DONNEES_MENSUELLES_ETENDUES* H2O2_J_Instanciation()
{
    int i;
    int NombreDeProblemes;
    int NbPdt;
    int j;
    int NombreDeVariables;
    int NombreDeContraintes;
    int NombreDeTermesAlloues;
    DONNEES_MENSUELLES_ETENDUES* DonneesMensuellesEtendues = new DONNEES_MENSUELLES_ETENDUES;
    PROBLEME_HYDRAULIQUE_ETENDU* ProblemeHydrauliqueEtendu;

    if (DonneesMensuellesEtendues == NULL)
        return (NULL);

    DonneesMensuellesEtendues->ProblemeHydrauliqueEtendu = new PROBLEME_HYDRAULIQUE_ETENDU;

    ProblemeHydrauliqueEtendu = DonneesMensuellesEtendues->ProblemeHydrauliqueEtendu;

    ProblemeHydrauliqueEtendu->NombreDeProblemes = 4;

    auto& NbJoursDUnProbleme = ProblemeHydrauliqueEtendu->NbJoursDUnProbleme;
    NbJoursDUnProbleme.assign(ProblemeHydrauliqueEtendu->NombreDeProblemes, 0);

    NbJoursDUnProbleme[0] = 28;
    NbJoursDUnProbleme[1] = 29;
    NbJoursDUnProbleme[2] = 30;
    NbJoursDUnProbleme[3] = 31;

    DonneesMensuellesEtendues->TurbineMax.assign(NbJoursDUnProbleme[3], 0.);
    DonneesMensuellesEtendues->TurbineMin.assign(NbJoursDUnProbleme[3], 0.);
    DonneesMensuellesEtendues->TurbineCible.assign(NbJoursDUnProbleme[3], 0.);

    DonneesMensuellesEtendues->niveauBas.assign(NbJoursDUnProbleme[3], 0.);
    DonneesMensuellesEtendues->apports.assign(NbJoursDUnProbleme[3], 0.);

    DonneesMensuellesEtendues->Turbine.assign(NbJoursDUnProbleme[3], 0.);
    DonneesMensuellesEtendues->niveauxFinJours.assign(NbJoursDUnProbleme[3], 0.);
    DonneesMensuellesEtendues->overflows.assign(NbJoursDUnProbleme[3], 0.);
    DonneesMensuellesEtendues->deviations.assign(NbJoursDUnProbleme[3], 0.);
    DonneesMensuellesEtendues->violations.assign(NbJoursDUnProbleme[3], 0.);

    NombreDeProblemes = ProblemeHydrauliqueEtendu->NombreDeProblemes;

    ProblemeHydrauliqueEtendu->CorrespondanceDesVariables.resize(NombreDeProblemes);
    ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieFixe.resize(NombreDeProblemes);
    ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable.resize(NombreDeProblemes);

    ProblemeHydrauliqueEtendu->ProblemeSpx.assign(NombreDeProblemes, nullptr);
    ProblemeHydrauliqueEtendu->Probleme = NULL;

    auto& CorrespondanceDesVariables = ProblemeHydrauliqueEtendu->CorrespondanceDesVariables;
    auto& ProblemeLineaireEtenduPartieFixe
        = ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieFixe;
    auto& ProblemeLineaireEtenduPartieVariable
        = ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieVariable;

    for (i = 0; i < NombreDeProblemes; i++)
    {
        NbPdt = NbJoursDUnProbleme[i];

        auto& CorrVar = CorrespondanceDesVariables[i];

        CorrVar.NumeroVar_Turbine.assign(NbPdt, 0);
        CorrVar.NumeroVar_niveauxFinJours.assign(NbPdt, 0);
        CorrVar.NumeroVar_overflow.assign(NbPdt, 0);
        CorrVar.NumeroVar_deviations.assign(NbPdt, 0);
        CorrVar.NumeroVar_violations.assign(NbPdt, 0);

        auto& PlFixe = ProblemeLineaireEtenduPartieFixe[i];

        NombreDeVariables = 0;
        NombreDeVariables += NbPdt;

        NombreDeVariables += NbPdt;
        NombreDeVariables += NbPdt;
        NombreDeVariables += NbPdt;
        NombreDeVariables += NbPdt;

        NombreDeVariables += 1;
        NombreDeVariables += 1;
        NombreDeVariables += 1;

        PlFixe.NombreDeVariables = NombreDeVariables;
        PlFixe.CoutLineaire.assign(NombreDeVariables, 0);

        PlFixe.TypeDeVariable.assign(NombreDeVariables, 0);

        NombreDeContraintes = 0;
        NombreDeContraintes += NbPdt;

        NombreDeContraintes += 1;
        NombreDeContraintes += NbPdt;
        NombreDeContraintes += NbPdt;
        NombreDeContraintes += NbPdt;
        NombreDeContraintes += NbPdt;
        NombreDeContraintes += NbPdt;

        PlFixe.NombreDeContraintes = NombreDeContraintes;
        PlFixe.Sens.assign(NombreDeContraintes, 0);

        PlFixe.IndicesDebutDeLigne.assign(NombreDeContraintes, 0.);
        PlFixe.NombreDeTermesDesLignes.assign(NombreDeContraintes, 0.);

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

        PlFixe.NombreDeTermesAlloues = NombreDeTermesAlloues;
        PlFixe.CoefficientsDeLaMatriceDesContraintes.assign(NombreDeTermesAlloues, 0.);

        PlFixe.IndicesColonnes.assign(NombreDeTermesAlloues, 0);

        auto& PlVariable = ProblemeLineaireEtenduPartieVariable[i];

        PlVariable.X.assign(NombreDeVariables, 0.);
        PlVariable.Xmin.assign(NombreDeVariables, 0.);
        PlVariable.Xmax.assign(NombreDeVariables, 0.);
        PlVariable.SecondMembre.assign(NombreDeContraintes, 0.);

        PlVariable.AdresseOuPlacerLaValeurDesVariablesOptimisees.assign(NombreDeVariables, nullptr);

        PlVariable.PositionDeLaVariable.assign(NombreDeVariables, 0);
        PlVariable.ComplementDeLaBase.assign(NombreDeContraintes, 0);

        PlVariable.CoutsReduits.assign(NombreDeVariables, 0.);
        PlVariable.CoutsMarginauxDesContraintes.assign(NombreDeContraintes, 0.);
    }

    for (i = 0; i < NombreDeProblemes; i++)
    {
        H2O2_j_ConstruireLesVariables(
          NbJoursDUnProbleme[i],
          DonneesMensuellesEtendues,
          ProblemeLineaireEtenduPartieVariable[i].Xmin,
          ProblemeLineaireEtenduPartieVariable[i].Xmax,
          ProblemeLineaireEtenduPartieFixe[i].TypeDeVariable,
          ProblemeLineaireEtenduPartieVariable[i].AdresseOuPlacerLaValeurDesVariablesOptimisees,
          CorrespondanceDesVariables[i]);

        H2O2_J_ConstruireLesContraintes(
          NbJoursDUnProbleme[i],
          ProblemeLineaireEtenduPartieFixe[i].IndicesDebutDeLigne,
          ProblemeLineaireEtenduPartieFixe[i].Sens,
          ProblemeLineaireEtenduPartieFixe[i].NombreDeTermesDesLignes,
          ProblemeLineaireEtenduPartieFixe[i].CoefficientsDeLaMatriceDesContraintes,
          ProblemeLineaireEtenduPartieFixe[i].IndicesColonnes,
          CorrespondanceDesVariables[i]);
    }

    return (DonneesMensuellesEtendues);
}
