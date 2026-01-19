// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

void OPT_ChainagesDesIntercoPartantDUnNoeud(PROBLEME_HEBDO* problemeHebdo)
{
    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        problemeHebdo->IndexDebutIntercoOrigine[pays] = -1;
        problemeHebdo->IndexDebutIntercoExtremite[pays] = -1;
    }

    for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
    {
        int noeudOrigine = problemeHebdo->PaysOrigineDeLInterconnexion[interco];
        int index = problemeHebdo->IndexDebutIntercoOrigine[noeudOrigine];
        problemeHebdo->IndexDebutIntercoOrigine[noeudOrigine] = interco;
        problemeHebdo->IndexSuivantIntercoOrigine[interco] = index;

        int noeudExtremite = problemeHebdo->PaysExtremiteDeLInterconnexion[interco];
        index = problemeHebdo->IndexDebutIntercoExtremite[noeudExtremite];
        problemeHebdo->IndexDebutIntercoExtremite[noeudExtremite] = interco;
        problemeHebdo->IndexSuivantIntercoExtremite[interco] = index;
    }

    return;
}
