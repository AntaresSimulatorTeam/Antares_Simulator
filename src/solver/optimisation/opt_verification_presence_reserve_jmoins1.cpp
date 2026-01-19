// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

void OPT_VerifierPresenceReserveJmoins1(PROBLEME_HEBDO* problemeHebdo)
{
    problemeHebdo->YaDeLaReserveJmoins1 = false;

    std::vector<RESERVE_JMOINS1>& ReserveJMoins1 = problemeHebdo->ReserveJMoins1;

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const std::vector<double>& ReserveHoraireJMoins1 = ReserveJMoins1[pays]
                                                             .ReserveHoraireJMoins1;
        for (uint pdt = 0; pdt < problemeHebdo->NombreDePasDeTemps; pdt++)
        {
            if (fabs(ReserveHoraireJMoins1[pdt]) > ZERO_RESERVE_J_MOINS1)
            {
                problemeHebdo->YaDeLaReserveJmoins1 = true;
                return;
            }
        }
    }
}
