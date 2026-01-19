// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
