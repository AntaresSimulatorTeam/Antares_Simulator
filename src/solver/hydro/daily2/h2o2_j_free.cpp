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

#include "spx_definition_arguments.h"
#include "spx_fonctions.h"

#ifdef __CPLUSPLUS
}
#endif

#include "antares/solver/hydro/daily2/h2o2_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily2/h2o2_j_fonctions.h"

void H2O2_J_Free(DONNEES_MENSUELLES_ETENDUES& DonneesMensuelles)
{
    auto& ProblemeHydrauliqueEtendu = DonneesMensuelles.ProblemeHydrauliqueEtendu;
    int NombreDeProblemes = ProblemeHydrauliqueEtendu.NombreDeProblemes;

    for (int i = 0; i < NombreDeProblemes; i++)
    {
        auto ProbSpx = ProblemeHydrauliqueEtendu.ProblemeSpx[i];
        if (ProbSpx)
        {
            SPX_LibererProbleme(ProbSpx);
        }
    }

    return;
}
