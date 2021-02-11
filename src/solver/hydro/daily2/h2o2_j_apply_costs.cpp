/*
** Copyright 2007-2018 RTE
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
#include "h2o2_j_donnees_optimisation.h"
#include "h2o2_j_fonctions.h"

void H2O2_J_apply_costs(const Hydro_problem_costs& h2o2_costs, DONNEES_MENSUELLES_ETENDUES& problem)
{
    int NombreDeProblemes;
    PROBLEME_LINEAIRE_ETENDU_PARTIE_FIXE** ProblemeLineaireEtenduPartieFixe;
    CORRESPONDANCE_DES_VARIABLES_PB_ETENDU** CorrespondanceDesVariables;
    int NbPdt;
    int* NbJoursDUnProbleme;

    NombreDeProblemes = problem.ProblemeHydrauliqueEtendu->NombreDeProblemes;
    ProblemeLineaireEtenduPartieFixe
      = problem.ProblemeHydrauliqueEtendu->ProblemeLineaireEtenduPartieFixe;
    CorrespondanceDesVariables = problem.ProblemeHydrauliqueEtendu->CorrespondanceDesVariables;
    NbJoursDUnProbleme = problem.ProblemeHydrauliqueEtendu->NbJoursDUnProbleme;

    for (int pb_num = 0; pb_num < NombreDeProblemes; pb_num++)
    {
        NbPdt = NbJoursDUnProbleme[pb_num];
        
        for (int var = 0; var < ProblemeLineaireEtenduPartieFixe[pb_num]->NombreDeVariables; var++)
            ProblemeLineaireEtenduPartieFixe[pb_num]->CoutLineaire[var] = 0.0;

        for (int Pdt = 0; Pdt < NbPdt; Pdt++)
            ProblemeLineaireEtenduPartieFixe[pb_num]
              ->CoutLineaire[CorrespondanceDesVariables[pb_num]->NumeroVar_niveauxFinJours[Pdt]]
              = h2o2_costs.get_end_days_levels_cost();

        for (int Pdt = 0; Pdt < NbPdt; Pdt++)
            ProblemeLineaireEtenduPartieFixe[pb_num]
              ->CoutLineaire[CorrespondanceDesVariables[pb_num]->NumeroVar_overflow[Pdt]]
              = h2o2_costs.get_overflow_cost();

        for (int Pdt = 0; Pdt < NbPdt; Pdt++)
            ProblemeLineaireEtenduPartieFixe[pb_num]
              ->CoutLineaire[CorrespondanceDesVariables[pb_num]->NumeroVar_deviations[Pdt]]
              = h2o2_costs.get_deviations_cost();

        for (int Pdt = 0; Pdt < NbPdt; Pdt++)
            ProblemeLineaireEtenduPartieFixe[pb_num]
              ->CoutLineaire[CorrespondanceDesVariables[pb_num]->NumeroVar_violations[Pdt]]
              = h2o2_costs.get_violations_cost();

        ProblemeLineaireEtenduPartieFixe[pb_num]
          ->CoutLineaire[CorrespondanceDesVariables[pb_num]->NumeroVar_waste]
          = h2o2_costs.get_waste_cost();

        ProblemeLineaireEtenduPartieFixe[pb_num]
          ->CoutLineaire[CorrespondanceDesVariables[pb_num]->NumeroVar_deviationMax]
          = h2o2_costs.get_deviationMax_cost();

        ProblemeLineaireEtenduPartieFixe[pb_num]
          ->CoutLineaire[CorrespondanceDesVariables[pb_num]->NumeroVar_violationMax]
          = h2o2_costs.get_violationMax_cost();
    }
}
