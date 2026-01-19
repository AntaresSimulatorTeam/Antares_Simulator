// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

void OPT_NumeroDeJourDuPasDeTemps(PROBLEME_HEBDO* problemeHebdo)
{
    problemeHebdo->NombreDeJours = problemeHebdo->NombreDePasDeTemps
                                   / problemeHebdo->NombreDePasDeTempsDUneJournee;
    for (uint pdtHebdo = 0; pdtHebdo < problemeHebdo->NombreDePasDeTemps; pdtHebdo++)
    {
        double X = pdtHebdo / problemeHebdo->NombreDePasDeTempsDUneJournee;
        problemeHebdo->NumeroDeJourDuPasDeTemps[pdtHebdo] = (int)floor(X);
    }
}

void OPT_NumeroDIntervalleOptimiseDuPasDeTemps(PROBLEME_HEBDO* problemeHebdo)
{
    for (uint pdtHebdo = 0; pdtHebdo < problemeHebdo->NombreDePasDeTemps; pdtHebdo++)
    {
        double X = pdtHebdo / problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
        problemeHebdo->NumeroDIntervalleOptimiseDuPasDeTemps[pdtHebdo] = (int)floor(X);
    }
}
