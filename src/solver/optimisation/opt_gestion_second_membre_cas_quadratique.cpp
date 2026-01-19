// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

void OPT_InitialiserLeSecondMembreDuProblemeQuadratique(PROBLEME_HEBDO* problemeHebdo, int PdtHebdo)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays - 1; pays++)
    {
        int cnt = problemeHebdo->NumeroDeContrainteDeSoldeDEchange[pays];
        ProblemeAResoudre->SecondMembre[cnt] = problemeHebdo->SoldeMoyenHoraire[PdtHebdo]
                                                 .SoldeMoyenDuPays[pays];
    }
}
