// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/daily2/h2o2_j_donnees_mensuelles.h"

#include "spx_constantes_externes.h"

void H2O2_j_ConstruireLesVariables(
  int NbPdt,
  DONNEES_MENSUELLES_ETENDUES& DonneesMensuellesEtendues,
  std::vector<double>& Xmin,
  std::vector<double>& Xmax,
  std::vector<int>& TypeDeVariable,
  std::vector<double*>& AdresseOuPlacerLaValeurDesVariablesOptimisees,
  CORRESPONDANCE_DES_VARIABLES_PB_ETENDU& CorrespondanceDesVariables)
{
    int Var = 0;

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CorrespondanceDesVariables.NumeroVar_Turbine[Pdt] = Var;
        Xmin[Var] = 0.0;
        Xmax[Var] = 0.0;
        TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
        AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(
          DonneesMensuellesEtendues.Turbine[Pdt]);
        Var++;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CorrespondanceDesVariables.NumeroVar_niveauxFinJours[Pdt] = Var;
        Xmin[Var] = 0.0;
        Xmax[Var] = 1.0;
        TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
        AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(
          DonneesMensuellesEtendues.niveauxFinJours[Pdt]);
        Var++;
    }

    CorrespondanceDesVariables.NumeroVar_waste = Var;
    Xmin[Var] = 0.0;
    Xmax[Var] = LINFINI;
    TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
    AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(DonneesMensuellesEtendues.waste);
    Var++;

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CorrespondanceDesVariables.NumeroVar_overflow[Pdt] = Var;
        Xmin[Var] = 0.0;
        Xmax[Var] = LINFINI;
        TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
        AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(
          DonneesMensuellesEtendues.overflows[Pdt]);
        Var++;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CorrespondanceDesVariables.NumeroVar_deviations[Pdt] = Var;
        Xmin[Var] = 0.0;
        Xmax[Var] = LINFINI;
        TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
        AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(
          DonneesMensuellesEtendues.deviations[Pdt]);
        Var++;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CorrespondanceDesVariables.NumeroVar_violations[Pdt] = Var;
        Xmin[Var] = 0.0;
        Xmax[Var] = LINFINI;
        TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
        AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(
          DonneesMensuellesEtendues.violations[Pdt]);
        Var++;
    }

    CorrespondanceDesVariables.NumeroVar_deviationMax = Var;
    Xmin[Var] = 0.0;
    Xmax[Var] = LINFINI;
    TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
    AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(DonneesMensuellesEtendues.deviationMax);
    Var++;

    CorrespondanceDesVariables.NumeroVar_violationMax = Var;
    Xmin[Var] = 0.0;
    Xmax[Var] = LINFINI;
    TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
    AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(DonneesMensuellesEtendues.violationMax);
    Var++;

    return;
}
