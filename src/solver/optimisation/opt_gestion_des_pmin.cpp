// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
