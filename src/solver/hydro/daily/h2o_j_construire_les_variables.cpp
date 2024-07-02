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

#ifdef __CPLUSPLUS
extern "C"
{
#endif

#include "spx_constantes_externes.h"

#ifdef __CPLUSPLUS
}
#endif

#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

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

    CorrespondanceDesVariables.NumeroDeLaVariableMu = Var;
    Xmin[Var] = 0.0;
    Xmax[Var] = LINFINI;
    TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
    AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = nullptr;
    Var++;

    CorrespondanceDesVariables.NumeroDeLaVariableXi = Var;
    Xmin[Var] = 0.0;
    Xmax[Var] = LINFINI;
    TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
    AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = nullptr;
    Var++;

    return;
}
