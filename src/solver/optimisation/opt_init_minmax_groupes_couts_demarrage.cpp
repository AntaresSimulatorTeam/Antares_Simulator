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

void OPT_InitialiserNombreMinEtMaxDeGroupesCoutsDeDemarrage(PROBLEME_HEBDO* problemeHebdo)
{
    if (!problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    {
        return;
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[pays];
        std::vector<PDISP_ET_COUTS_HORAIRES_PAR_PALIER>& PuissanceDisponibleEtCout
          = PaliersThermiquesDuPays.PuissanceDisponibleEtCout;

        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            const std::vector<double>& PuissanceDisponibleDuPalierThermiqueRef
              = PuissanceDisponibleEtCout[index].PuissanceDisponibleDuPalierThermiqueRef;
            const std::vector<double>& PuissanceMinDuPalierThermiqueRef
              = PuissanceDisponibleEtCout[index].PuissanceMinDuPalierThermiqueRef;
            std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
              = PuissanceDisponibleEtCout[index].NombreMaxDeGroupesEnMarcheDuPalierThermique;
            std::vector<int>& NombreMinDeGroupesEnMarcheDuPalierThermique
              = PuissanceDisponibleEtCout[index].NombreMinDeGroupesEnMarcheDuPalierThermique;

            double TailleUnitaireDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays.TailleUnitaireDUnGroupeDuPalierThermique[index];

            for (uint pdtHebdo = 0; pdtHebdo < problemeHebdo->NombreDePasDeTemps; pdtHebdo++)
            {
                if (TailleUnitaireDUnGroupeDuPalierThermique != 0)
                {
                    NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo] = (int)ceil(
                      PuissanceDisponibleDuPalierThermiqueRef[pdtHebdo]
                      / TailleUnitaireDUnGroupeDuPalierThermique);

                    NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo] = (int)ceil(
                      PuissanceMinDuPalierThermiqueRef[pdtHebdo]
                      / TailleUnitaireDUnGroupeDuPalierThermique);
                }
                else
                {
                    NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo] = 0;
                    NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo] = 0;
                }
            }
        }
    }
}
