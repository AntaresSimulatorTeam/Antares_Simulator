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
** SPDX-License-Identifier: MPL 2.0
*/

#include "h2o2_j_donnees_mensuelles.h"

void H2O2_J_InitialiserLeSecondMembre(DONNEES_MENSUELLES_ETENDUES& DonneesMensuelles,
                                      int NumeroDeProbleme)
{
    auto& ProblemeHydrauliqueEtendu = DonneesMensuelles.ProblemeHydrauliqueEtendu;

    auto& ProblemeLineairePartieVariable
        = ProblemeHydrauliqueEtendu.ProblemeLineaireEtenduPartieVariable[NumeroDeProbleme];
    auto& SecondMembre = ProblemeLineairePartieVariable.SecondMembre;

    int NbPdt = ProblemeHydrauliqueEtendu.NbJoursDUnProbleme[NumeroDeProbleme];
    int Cnt = 0;

    SecondMembre[Cnt] = DonneesMensuelles.NiveauInitialDuMois + DonneesMensuelles.apports[0];
    Cnt++;

    for (int Pdt = 1; Pdt < NbPdt; Pdt++)
    {
        SecondMembre[Cnt] = DonneesMensuelles.apports[Pdt];
        Cnt++;
    }

    SecondMembre[Cnt] = DonneesMensuelles.TurbineDuMois;
    Cnt++;

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        SecondMembre[Cnt] = DonneesMensuelles.TurbineCible[Pdt];
        Cnt++;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        SecondMembre[Cnt] = DonneesMensuelles.TurbineCible[Pdt];
        Cnt++;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        SecondMembre[Cnt] = 0.;
        Cnt++;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        SecondMembre[Cnt] = DonneesMensuelles.niveauBas[Pdt];
        Cnt++;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        SecondMembre[Cnt] = 0.;
        Cnt++;
    }

    return;
}
