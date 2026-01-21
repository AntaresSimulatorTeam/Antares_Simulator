// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

namespace DoneesOptimisationJournaliere
{
void H2O_j_ConstruireLesVariables(
  int NbPdt,
  std::vector<int>& NumeroDeVariableTurbine,
  std::vector<double>& /*Xmin*/,
  std::vector<double>& /*Xmax*/,
  std::vector<int>& /*TypeDeVariable*/,
  std::vector<double*>& /*AdresseOuPlacerLaValeurDesVariablesOptimisees*/,
  CORRESPONDANCE_DES_VARIABLES& CorrespondanceDesVariables)
{
    // Only the index mapping is still used by the daily heuristic.
    int Var = 0;

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        NumeroDeVariableTurbine[Pdt] = Var;
        Var++;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CorrespondanceDesVariables.NumeroDeLaVariableXi[Pdt] = Var;
        Var++;
    }

    CorrespondanceDesVariables.NumeroDeLaVariableXiPlus = Var;
    Var++;

    CorrespondanceDesVariables.NumeroDeLaVariableXiMoins = Var;
}
} // namespace DoneesOptimisationJournaliere
