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

void H2O_J_OptimiserUnMois(DONNEES_MENSUELLES* DonneesMensuelles)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;

    int NumeroDeProbleme = -1;
    for (int i = 0; i < ProblemeHydraulique.NombreDeProblemes; i++)
    {
        if (DonneesMensuelles->NombreDeJoursDuMois == ProblemeHydraulique.NbJoursDUnProbleme[i])
        {
            NumeroDeProbleme = i;
            break;
        }
    }
    if (NumeroDeProbleme < 0)
    {
        DonneesMensuelles->ResultatsValides = EMERGENCY_SHUT_DOWN;
        return;
    }

    DonneesMensuelles->ResultatsValides = NON;

    H2O_J_InitialiserLeSecondMembre(DonneesMensuelles, NumeroDeProbleme);
    H2O_J_InitialiserLesBornesdesVariables(DonneesMensuelles, NumeroDeProbleme);
    H2O_J_ResoudreLeProblemeLineaire(DonneesMensuelles, NumeroDeProbleme);
    H2O_J_LisserLesSurTurbines(DonneesMensuelles, NumeroDeProbleme);

    return;
}
