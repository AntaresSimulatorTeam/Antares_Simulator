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
#include "antares/solver/simulation/sim_structure_donnees.h"
#include "antares/solver/simulation/simulation.h"

#include "spx_fonctions.h"

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

    return;
}
