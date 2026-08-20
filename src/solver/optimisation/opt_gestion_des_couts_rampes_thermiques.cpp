// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/optimisation/variables/VariableManagerUtils.h"
#include "antares/solver/simulation/sim_structure_donnees.h"
#include "antares/solver/simulation/simulation.h"

void OPT_InitialiserLesCoutsLineaireRampesThermiques(PROBLEME_HEBDO* problemeHebdo,
                                                     const int PremierPdtDeLIntervalle,
                                                     const int DernierPdtDeLIntervalle)
{
    if (!problemeHebdo->rampingEnabled)
    {
        return;
    }
    
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    
    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        const CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
          = problemeHebdo->CorrespondanceVarNativesVarOptim[pdtJour];

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; ++pays)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo
                                                                  ->PaliersThermiquesDuPays[pays];
            int var;

            for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
            {
                if (PaliersThermiquesDuPays.maxUpwardPowerRampingRate[index] >= 0)
                {
                    int palier = PaliersThermiquesDuPays
                                   .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

                    var = CorrespondanceVarNativesVarOptim.powerRampingDecreaseIndex[palier];
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                    {
                        ProblemeAResoudre->CoutLineaire[var] = PaliersThermiquesDuPays
                                                                 .downwardRampingCost[index];
                    }

                    var = CorrespondanceVarNativesVarOptim.powerRampingIncreaseIndex[palier];
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                    {
                        ProblemeAResoudre->CoutLineaire[var] = PaliersThermiquesDuPays
                                                                 .upwardRampingCost[index];
                    }
                }
            }
        }
    }
}
