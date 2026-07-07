// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

void OPT_InitialiserNombreMinEtMaxDeGroupesCoutsDeDemarrage(PROBLEME_HEBDO* problemeHebdo)
{
    if (!problemeHebdo->OptimisationNotFastMode)
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
