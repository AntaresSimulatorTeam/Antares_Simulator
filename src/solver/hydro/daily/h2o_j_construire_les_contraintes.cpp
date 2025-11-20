/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

namespace DoneesOptimisationJournaliere
{
void H2O_J_ConstruireLesContraintes(int NbPdt,
                                    std::vector<int>& NumeroDeVariableTurbine,
                                    std::vector<int>& NumeroDeLaVariableXi,
                                    int NumeroDeLaVariableXiPlus,
                                    int NumeroDeLaVariableXiMoins,
                                    std::vector<int>& IndicesDebutDeLigne,
                                    std::vector<char>& Sens,
                                    std::vector<int>& NombreDeTermesDesLignes,
                                    std::vector<double>& CoefficientsDeLaMatriceDesContraintes,
                                    std::vector<int>& IndicesColonnes,
                                    CORRESPONDANCE_DES_CONTRAINTES& CorrespondanceDesContraintes)
{
    int NombreDeContraintes = 0;
    int NombreDeTermes = 0;
    int il = 0;

    IndicesDebutDeLigne[NombreDeContraintes] = il;
    // --- Contrainte somme des turbines = somme des cibles ---
    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = NumeroDeVariableTurbine[Pdt];
        il++;
    }

    Sens[NombreDeContraintes] = '=';
    NombreDeTermesDesLignes[NombreDeContraintes] = NbPdt;

    CorrespondanceDesContraintes.NumeroDeContrainteDEnergieMensuelle = NombreDeContraintes;
    NombreDeContraintes++;

    // --- turbine[t] + Xi >= cible[t] ---
    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = NumeroDeVariableTurbine[Pdt];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = NumeroDeLaVariableXi[Pdt];
        il++;

        Sens[NombreDeContraintes] = '>';
        NombreDeTermesDesLignes[NombreDeContraintes] = 2;

        CorrespondanceDesContraintes.NumeroDeContrainteSurXi[Pdt] = NombreDeContraintes;
        NombreDeContraintes++;
    }

    // --- cible[t] + Xi >= turbine[t]  => -turbine[t] + Xi >= -cible[t] ---
    for (int Pdt = 0; Pdt < NbPdt; ++Pdt)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
        IndicesColonnes[il++] = NumeroDeVariableTurbine[Pdt];
        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il++] = NumeroDeLaVariableXi[Pdt];

        Sens[NombreDeContraintes] = '>';
        NombreDeTermesDesLignes[NombreDeContraintes] = 2;
        CorrespondanceDesContraintes.NumeroDeContrainteSurXiSym[Pdt] = NombreDeContraintes;
        NombreDeContraintes++;
    }

    // --- turbine[t] + xi_plus >= cible[t] ---
    for (int Pdt = 0; Pdt < NbPdt; ++Pdt)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il++] = NumeroDeVariableTurbine[Pdt];
        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il++] = NumeroDeLaVariableXiPlus;

        Sens[NombreDeContraintes] = '>';
        NombreDeTermesDesLignes[NombreDeContraintes] = 2;
        CorrespondanceDesContraintes.NumeroDeContrainteSurXiPlus[Pdt] = NombreDeContraintes;
        NombreDeContraintes++;
    }

    // --- cible[t] + xi_moins >= turbine[t] => -turbine[t] + xi_moins >= -cible[t] ---
    for (int Pdt = 0; Pdt < NbPdt; ++Pdt)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
        IndicesColonnes[il++] = NumeroDeVariableTurbine[Pdt];
        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il++] = NumeroDeLaVariableXiMoins;

        Sens[NombreDeContraintes] = '>';
        NombreDeTermesDesLignes[NombreDeContraintes] = 2;
        CorrespondanceDesContraintes.NumeroDeContrainteSurXiMoins[Pdt] = NombreDeContraintes;
        NombreDeContraintes++;
    }
}
} // namespace DoneesOptimisationJournaliere
