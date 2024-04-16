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

#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_donnees.h"

void
OPT_ChargerLaContrainteDansLaMatriceDesContraintes(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
                                                   std::vector<double>& Pi,
                                                   std::vector<int>& Colonne,
                                                   int NombreDeTermesDeLaContrainte,
                                                   char SensContrainte)
{
    int& nombreDeTermes = ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes;
    int& nombreDeContraintes = ProblemeAResoudre->NombreDeContraintes;

    ProblemeAResoudre->IndicesDebutDeLigne[nombreDeContraintes] = nombreDeTermes;
    for (int i = 0; i < NombreDeTermesDeLaContrainte; i++)
    {
        ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes[nombreDeTermes] = Pi[i];
        ProblemeAResoudre->IndicesColonnes[nombreDeTermes] = Colonne[i];
        nombreDeTermes++;
        if (nombreDeTermes == ProblemeAResoudre->NombreDeTermesAllouesDansLaMatriceDesContraintes)
        {
            OPT_AugmenterLaTailleDeLaMatriceDesContraintes(ProblemeAResoudre);
        }
    }
    ProblemeAResoudre->NombreDeTermesDesLignes[nombreDeContraintes] = NombreDeTermesDeLaContrainte;

    ProblemeAResoudre->Sens[nombreDeContraintes] = SensContrainte;
    nombreDeContraintes++;

    return;
}
