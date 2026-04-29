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

    PROBLEME_LINEAIRE_PARTIE_VARIABLE& ProblemeLineairePartieVariable
      = ProblemeHydraulique.ProblemeLineairePartieVariable[NumeroDeProbleme];

    std::vector<double>& Xmin = ProblemeLineairePartieVariable.Xmin;
    std::vector<double>& Xmax = ProblemeLineairePartieVariable.Xmax;
    std::vector<double*>& AdresseOuPlacerLaValeurDesVariablesOptimisees
      = ProblemeLineairePartieVariable.AdresseOuPlacerLaValeurDesVariablesOptimisees;

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        int Var = CorrespondanceDesVariables.NumeroDeVariableTurbine[Pdt];
        Xmax[Var] = TurbineMax[Pdt];

        Xmin[Var] = std::min(TurbineMax[Pdt], TurbineMin[Pdt]);

        AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(Turbine[Pdt]);
    }

    return;
}
} // namespace DoneesOptimisationJournaliere
