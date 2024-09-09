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

void H2O2_J_InitialiserLeSecondMembre(DONNEES_MENSUELLES_ETENDUES& DonneesMensuelles,
                                      int NumeroDeProbleme)
{
    auto& ProblemeHydrauliqueEtendu = DonneesMensuelles.ProblemeHydrauliqueEtendu;

    auto& ProblemeLineairePartieVariable = ProblemeHydrauliqueEtendu
                                             .ProblemeLineaireEtenduPartieVariable
                                               [NumeroDeProbleme];
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
