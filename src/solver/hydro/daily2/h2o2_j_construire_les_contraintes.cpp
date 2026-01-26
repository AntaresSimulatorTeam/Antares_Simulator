// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/daily2/h2o2_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily2/h2o2_j_fonctions.h"

void H2O2_J_ConstruireLesContraintes(
  int NbPdt,
  std::vector<int>& IndicesDebutDeLigne,
  std::vector<char>& Sens,
  std::vector<int>& NombreDeTermesDesLignes,
  std::vector<double>& CoefficientsDeLaMatriceDesContraintes,
  std::vector<int>& IndicesColonnes,
  CORRESPONDANCE_DES_VARIABLES_PB_ETENDU& CorrespondanceDesVariables)
{
    int NombreDeContraintes = 0;
    int il = 0;

    IndicesDebutDeLigne[NombreDeContraintes] = il;

    CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
    IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_Turbine[0];
    il++;
    CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
    IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_niveauxFinJours[0];
    il++;
    CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
    IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_overflow[0];
    il++;

    Sens[NombreDeContraintes] = '=';
    NombreDeTermesDesLignes[NombreDeContraintes] = 3;

    NombreDeContraintes++;

    for (int Pdt = 1; Pdt < NbPdt; Pdt++)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_Turbine[Pdt];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_niveauxFinJours[Pdt];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
        IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_niveauxFinJours[Pdt - 1];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_overflow[Pdt];
        il++;

        Sens[NombreDeContraintes] = '=';
        NombreDeTermesDesLignes[NombreDeContraintes] = 4;
        NombreDeContraintes++;
    }

    IndicesDebutDeLigne[NombreDeContraintes] = il;

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_Turbine[Pdt];
        il++;
    }

    CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
    IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_waste;
    il++;

    Sens[NombreDeContraintes] = '=';
    NombreDeTermesDesLignes[NombreDeContraintes] = NbPdt + 1;
    NombreDeContraintes++;

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_Turbine[Pdt];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
        IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_deviations[Pdt];
        il++;

        Sens[NombreDeContraintes] = '<';
        NombreDeTermesDesLignes[NombreDeContraintes] = 2;
        NombreDeContraintes++;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_Turbine[Pdt];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_deviations[Pdt];
        il++;

        Sens[NombreDeContraintes] = '>';
        NombreDeTermesDesLignes[NombreDeContraintes] = 2;
        NombreDeContraintes++;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_deviations[Pdt];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
        IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_deviationMax;
        il++;

        Sens[NombreDeContraintes] = '<';
        NombreDeTermesDesLignes[NombreDeContraintes] = 2;
        NombreDeContraintes++;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_niveauxFinJours[Pdt];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_violations[Pdt];
        il++;

        Sens[NombreDeContraintes] = '>';
        NombreDeTermesDesLignes[NombreDeContraintes] = 2;
        NombreDeContraintes++;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_violations[Pdt];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
        IndicesColonnes[il] = CorrespondanceDesVariables.NumeroVar_violationMax;
        il++;

        Sens[NombreDeContraintes] = '<';
        NombreDeTermesDesLignes[NombreDeContraintes] = 2;
        NombreDeContraintes++;
    }

    return;
}
