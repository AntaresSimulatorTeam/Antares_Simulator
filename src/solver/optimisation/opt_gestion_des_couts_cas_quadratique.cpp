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
#include "antares/solver/simulation/simulation.h"

#include "variables/VariableManagerUtils.h"

void OPT_InitialiserLesCoutsQuadratiques(PROBLEME_HEBDO* problemeHebdo, int PdtHebdo)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    ProblemeAResoudre->CoutLineaire.assign(ProblemeAResoudre->NombreDeVariables, 0.);
    auto variableManager = VariableManagerFromProblemHebdo(problemeHebdo);

    const VALEURS_DE_NTC_ET_RESISTANCES& ValeursDeResistances = problemeHebdo
                                                                  ->ValeursDeNTC[PdtHebdo];

    for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
    {
        int var = variableManager.NTCDirect(interco, 0);
        if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
        {
            ProblemeAResoudre->CoutQuadratique[var] = ValeursDeResistances
                                                        .ResistanceApparente[interco];
        }
    }
}
