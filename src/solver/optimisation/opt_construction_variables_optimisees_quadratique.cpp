// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "pi_constantes_externes.h"
#include "variables/VariableManagerUtils.h"

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeQuadratique(PROBLEME_HEBDO* problemeHebdo)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    assert(ProblemeAResoudre);

    int nombreDeVariables = 0;
    auto variableManager = VariableManagerFromProblemHebdo(problemeHebdo);

    for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
    {
        variableManager.NTCDirect(interco, 0) = nombreDeVariables;
        ProblemeAResoudre->TypeDeVariable[nombreDeVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
        nombreDeVariables++;
    }
    ProblemeAResoudre->NombreDeVariables = nombreDeVariables;
}
