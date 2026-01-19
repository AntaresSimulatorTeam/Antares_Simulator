// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

#include "spx_constantes_externes.h"

namespace DoneesOptimisationJournaliere
{
void H2O_j_ConstruireLesVariables(
  int NbPdt,
  std::vector<int>& NumeroDeVariableTurbine,
  std::vector<double>& Xmin,
  std::vector<double>& Xmax,
  std::vector<int>& TypeDeVariable,
  std::vector<double*>& AdresseOuPlacerLaValeurDesVariablesOptimisees,
  CORRESPONDANCE_DES_VARIABLES& CorrespondanceDesVariables)
{
    int Var = 0;

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        NumeroDeVariableTurbine[Pdt] = Var;
        Xmin[Var] = 0.0;
        Xmax[Var] = 0.0;
        TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
        Var++;
    }

    // is there any reason why this loop is separate from the previous one? (equivalent solution?)
    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CorrespondanceDesVariables.NumeroDeLaVariableXi[Pdt] = Var;
        Xmin[Var] = 0.0;
        Xmax[Var] = LINFINI;
        TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
        AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = nullptr;
        Var++;
    }

    // --- Variables globales xi_plus et xi_moins ---
    CorrespondanceDesVariables.NumeroDeLaVariableXiPlus = Var;
    Xmin[Var] = 0.0;
    Xmax[Var] = LINFINI;
    TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
    AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = nullptr;
    Var++;

    CorrespondanceDesVariables.NumeroDeLaVariableXiMoins = Var;
    Xmin[Var] = 0.0;
    Xmax[Var] = LINFINI;
    TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
    AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = nullptr;
}
} // namespace DoneesOptimisationJournaliere
