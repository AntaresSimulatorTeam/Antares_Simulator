// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/daily2/h2o2_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily2/h2o2_j_fonctions.h"

void H2O2_J_OptimiserUnMois(DONNEES_MENSUELLES_ETENDUES& DonneesMensuelles)
{
    auto& ProblemeHydrauliqueEtendu = DonneesMensuelles.ProblemeHydrauliqueEtendu;

    int NumeroDeProbleme = -1;
    for (int i = 0; i < ProblemeHydrauliqueEtendu.NombreDeProblemes; i++)
    {
        if (DonneesMensuelles.NombreDeJoursDuMois
            == ProblemeHydrauliqueEtendu.NbJoursDUnProbleme[i])
        {
            NumeroDeProbleme = i;
            break;
        }
    }

    if (NumeroDeProbleme < 0)
    {
        DonneesMensuelles.ResultatsValides = EMERGENCY_SHUT_DOWN;
        return;
    }

    DonneesMensuelles.ResultatsValides = NON;

    H2O2_J_InitialiserLeSecondMembre(DonneesMensuelles, NumeroDeProbleme);
    H2O2_J_InitialiserLesBornesdesVariables(DonneesMensuelles, NumeroDeProbleme);
    H2O2_J_ResoudreLeProblemeLineaire(DonneesMensuelles, NumeroDeProbleme);

    return;
}
