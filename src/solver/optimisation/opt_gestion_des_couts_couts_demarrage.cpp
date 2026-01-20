// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "variables/VariableManagerUtils.h"

void OPT_InitialiserLesCoutsLineaireCoutsDeDemarrage(PROBLEME_HEBDO* problemeHebdo,
                                                     const int PremierPdtDeLIntervalle,
                                                     const int DernierPdtDeLIntervalle)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    std::vector<double>& CoutLineaire = ProblemeAResoudre->CoutLineaire;
    auto variableManager = VariableManagerFromProblemHebdo(problemeHebdo);

    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo
                                                                  ->PaliersThermiquesDuPays[pays];

            for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
            {
                const int palier = PaliersThermiquesDuPays
                                     .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

                int var = variableManager.NumberOfDispatchableUnits(palier, pdtJour);
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    CoutLineaire[var] = PaliersThermiquesDuPays
                                          .CoutFixeDeMarcheDUnGroupeDuPalierThermique[index];
                }

                var = variableManager.NumberStoppingDispatchableUnits(palier, pdtJour);
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    CoutLineaire[var] = PaliersThermiquesDuPays
                                          .CoutDArretDUnGroupeDuPalierThermique[index];
                }

                var = variableManager.NumberBreakingDownDispatchableUnits(palier, pdtJour);
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    CoutLineaire[var] = 0;
                }

                var = variableManager.NumberStartingDispatchableUnits(palier, pdtJour);
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    CoutLineaire[var] = PaliersThermiquesDuPays
                                          .CoutDeDemarrageDUnGroupeDuPalierThermique[index];
                }
            }
        }
    }
}
