// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/daily2/h2o2_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily2/h2o2_j_donnees_optimisation.h"
#include "antares/solver/hydro/daily2/h2o2_j_fonctions.h"

void H2O2_J_apply_costs(const Hydro_problem_costs& h2o2_costs, DONNEES_MENSUELLES_ETENDUES& problem)
{
    int NombreDeProblemes = problem.ProblemeHydrauliqueEtendu.NombreDeProblemes;
    auto& ProblemeLineaireEtenduPartieFixe = problem.ProblemeHydrauliqueEtendu
                                               .ProblemeLineaireEtenduPartieFixe;
    auto& CorrespondanceDesVariables = problem.ProblemeHydrauliqueEtendu.CorrespondanceDesVariables;
    const auto& NbJoursDUnProbleme = problem.ProblemeHydrauliqueEtendu.NbJoursDUnProbleme;

    for (int pb_num = 0; pb_num < NombreDeProblemes; pb_num++)
    {
        int NbPdt = NbJoursDUnProbleme[pb_num];

        for (int var = 0; var < ProblemeLineaireEtenduPartieFixe[pb_num].NombreDeVariables; var++)
        {
            ProblemeLineaireEtenduPartieFixe[pb_num].CoutLineaire[var] = 0.0;
        }

        for (int Pdt = 0; Pdt < NbPdt; Pdt++)
        {
            ProblemeLineaireEtenduPartieFixe[pb_num]
              .CoutLineaire[CorrespondanceDesVariables[pb_num].NumeroVar_niveauxFinJours[Pdt]]
              = h2o2_costs.get_end_days_levels_cost();
        }

        for (int Pdt = 0; Pdt < NbPdt; Pdt++)
        {
            ProblemeLineaireEtenduPartieFixe[pb_num]
              .CoutLineaire[CorrespondanceDesVariables[pb_num].NumeroVar_overflow[Pdt]]
              = h2o2_costs.get_overflow_cost();
        }

        for (int Pdt = 0; Pdt < NbPdt; Pdt++)
        {
            ProblemeLineaireEtenduPartieFixe[pb_num]
              .CoutLineaire[CorrespondanceDesVariables[pb_num].NumeroVar_deviations[Pdt]]
              = h2o2_costs.get_deviations_cost();
        }

        for (int Pdt = 0; Pdt < NbPdt; Pdt++)
        {
            ProblemeLineaireEtenduPartieFixe[pb_num]
              .CoutLineaire[CorrespondanceDesVariables[pb_num].NumeroVar_violations[Pdt]]
              = h2o2_costs.get_violations_cost();
        }

        ProblemeLineaireEtenduPartieFixe[pb_num]
          .CoutLineaire[CorrespondanceDesVariables[pb_num].NumeroVar_waste]
          = h2o2_costs.get_waste_cost();

        ProblemeLineaireEtenduPartieFixe[pb_num]
          .CoutLineaire[CorrespondanceDesVariables[pb_num].NumeroVar_deviationMax]
          = h2o2_costs.get_deviationMax_cost();

        ProblemeLineaireEtenduPartieFixe[pb_num]
          .CoutLineaire[CorrespondanceDesVariables[pb_num].NumeroVar_violationMax]
          = h2o2_costs.get_violationMax_cost();
    }
}
