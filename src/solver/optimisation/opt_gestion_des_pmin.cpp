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

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#define ZERO 1.e-2

void OPT_InitialiserLesPminHebdo(PROBLEME_HEBDO* problemeHebdo)
{
    int NombreDePasDeTempsProblemeHebdo = problemeHebdo->NombreDePasDeTemps;

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[pays];
        std::vector<PDISP_ET_COUTS_HORAIRES_PAR_PALIER>& PuissanceDisponibleEtCout
          = PaliersThermiquesDuPays.PuissanceDisponibleEtCout;
        for (int palier = 0; palier < PaliersThermiquesDuPays.NombreDePaliersThermiques; palier++)
        {
            const std::vector<double>& PuissanceMinDuPalierThermique
              = PuissanceDisponibleEtCout[palier].PuissanceMinDuPalierThermique;
            std::vector<double>& PuissanceMinDuPalierThermiqueRef
              = PuissanceDisponibleEtCout[palier].PuissanceMinDuPalierThermiqueRef;

            for (int pdt = 0; pdt < NombreDePasDeTempsProblemeHebdo; pdt++)
            {
                PuissanceMinDuPalierThermiqueRef[pdt] = PuissanceMinDuPalierThermique[pdt];
            }
        }
    }

    return;
}
