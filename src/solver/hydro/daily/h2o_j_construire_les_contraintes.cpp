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

#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

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
    int                    il = 0;

    IndicesDebutDeLigne[NombreDeContraintes]                     = il;
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

    for (int Pdt = 0; Pdt < NbPdt; Pdt++){
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
