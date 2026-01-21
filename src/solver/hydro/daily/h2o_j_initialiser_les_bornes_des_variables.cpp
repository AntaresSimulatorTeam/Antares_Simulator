// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>

#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

namespace DoneesOptimisationJournaliere
{
void H2O_J_InitialiserLesBornesdesVariables(DONNEES_MENSUELLES* DonneesMensuelles,
                                            int NumeroDeProbleme)
{
    const std::vector<double>& TurbineMax = DonneesMensuelles->TurbineMax;
    const std::vector<double>& TurbineMin = DonneesMensuelles->TurbineMin;
    std::vector<double>& Turbine = DonneesMensuelles->Turbine;

    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;

    const int NbPdt = ProblemeHydraulique.NbJoursDUnProbleme[NumeroDeProbleme];

    const CORRESPONDANCE_DES_VARIABLES& CorrespondanceDesVariables = ProblemeHydraulique
                                                                       .CorrespondanceDesVariables
                                                                         [NumeroDeProbleme];

    // Bounds and result pointers are now initialised directly when creating
    // MPSolver variables (H2O_J_MPSolver_CreateVariables). This function is
    // kept for compatibility but no longer performs any work.
    (void)ProblemeHydraulique;
    (void)CorrespondanceDesVariables;
    (void)TurbineMax;
    (void)TurbineMin;
    (void)Turbine;
    (void)NbPdt;

    return;
}
} // namespace DoneesOptimisationJournaliere
