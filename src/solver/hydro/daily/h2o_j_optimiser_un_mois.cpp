// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

namespace DoneesOptimisationJournaliere
{
void H2O_J_OptimiserUnMois(DONNEES_MENSUELLES* DonneesMensuelles, const std::string& areaName)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;

    int NumeroDeProbleme = -1;
    for (int i = 0; i < ProblemeHydraulique.NombreDeProblemes; i++)
    {
        if (DonneesMensuelles->NombreDeJoursDuMois == ProblemeHydraulique.NbJoursDUnProbleme[i])
        {
            NumeroDeProbleme = i;
            break;
        }
    }
    if (NumeroDeProbleme < 0)
    {
        DonneesMensuelles->ResultatsValides = EMERGENCY_SHUT_DOWN;
        return;
    }

    DonneesMensuelles->ResultatsValides = NON;

    H2O_J_InitialiserLeSecondMembre(DonneesMensuelles, NumeroDeProbleme, areaName);
    H2O_J_InitialiserLesBornesdesVariables(DonneesMensuelles, NumeroDeProbleme);
    H2O_J_ResoudreLeProblemeLineaire(DonneesMensuelles, NumeroDeProbleme);

    return;
}
} // namespace DoneesOptimisationJournaliere
