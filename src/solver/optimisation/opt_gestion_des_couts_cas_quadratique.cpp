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

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"

void OPT_InitialiserLesCoutsQuadratiques(PROBLEME_HEBDO* problemeHebdo, int PdtHebdo)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    ProblemeAResoudre->CoutLineaire.assign(ProblemeAResoudre->NombreDeVariables, 0.);

    const VALEURS_DE_NTC_ET_RESISTANCES& ValeursDeResistances
      = problemeHebdo->ValeursDeNTC[PdtHebdo];
    const CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
      = problemeHebdo->CorrespondanceVarNativesVarOptim[0];

    for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
    {
        int var = CorrespondanceVarNativesVarOptim.NumeroDeVariableDeLInterconnexion[interco];
        if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            ProblemeAResoudre->CoutQuadratique[var]
              = ValeursDeResistances.ResistanceApparente[interco];
    }
}
