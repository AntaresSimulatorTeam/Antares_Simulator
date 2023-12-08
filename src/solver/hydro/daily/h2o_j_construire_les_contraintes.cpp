/*
** Copyright 2007-2023 RTE
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
** SPDX-License-Identifier: MPL 2.0
*/

#include "h2o_j_donnees_mensuelles.h"
#include "h2o_j_fonctions.h"

void H2O_J_ConstruireLesContraintes(int NbPdt,
                                    std::vector<int>& NumeroDeVariableTurbine,
                                    int NumeroDeLaVariableMu,
                                    int NumeroDeLaVariableXi,
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
    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = NumeroDeVariableTurbine[Pdt];
        il++;
        NombreDeTermes++;
    }
    CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
    IndicesColonnes[il] = NumeroDeLaVariableMu;
    il++;
    NombreDeTermes++;

    Sens[NombreDeContraintes] = '=';
    NombreDeTermesDesLignes[NombreDeContraintes] = NombreDeTermes;

    CorrespondanceDesContraintes.NumeroDeContrainteDEnergieMensuelle = NombreDeContraintes;
    NombreDeContraintes++;

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        NombreDeTermes = 0;
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = NumeroDeVariableTurbine[Pdt];
        il++;
        NombreDeTermes++;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = NumeroDeLaVariableXi;
        il++;
        NombreDeTermes++;

        Sens[NombreDeContraintes] = '>';
        NombreDeTermesDesLignes[NombreDeContraintes] = NombreDeTermes;

        CorrespondanceDesContraintes.NumeroDeContrainteSurXi[Pdt] = NombreDeContraintes;
        NombreDeContraintes++;
    }

    return;
}
