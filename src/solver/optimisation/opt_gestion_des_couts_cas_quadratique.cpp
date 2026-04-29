// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/optimisation/variables/VariableManagerUtils.h"

void OPT_InitialiserLesCoutsQuadratiques(PROBLEME_HEBDO* problemeHebdo, int PdtHebdo)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    ProblemeAResoudre->CoutLineaire.assign(ProblemeAResoudre->NombreDeVariables, 0.);
    auto variableManager = VariableManagerFromProblemHebdo(problemeHebdo);

    const VALEURS_DE_NTC_ET_RESISTANCES& ValeursDeResistances = problemeHebdo
                                                                  ->ValeursDeNTC[PdtHebdo];

    for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
    {
        int var = variableManager.DirectFlow(interco, 0);
        if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
        {
            ProblemeAResoudre->CoutQuadratique[var] = ValeursDeResistances
                                                        .ResistanceApparente[interco];
        }
    }
}
