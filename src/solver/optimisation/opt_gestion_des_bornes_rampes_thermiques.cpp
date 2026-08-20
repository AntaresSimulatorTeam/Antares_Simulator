// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/optimisation/variables/VariableManagement.h"
#include "antares/solver/optimisation/variables/VariableManagerUtils.h"
#include "antares/solver/simulation/sim_structure_donnees.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/simulation/simulation.h"

void OPT_InitialiserLesBornesDesVariablesDuProblemeLineaireRampesThermiques(
  PROBLEME_HEBDO* problemeHebdo,
  const int PremierPdtDeLIntervalle,
  const int DernierPdtDeLIntervalle)
{
    if (!problemeHebdo->rampingEnabled)
    {
        return;
    }
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    std::vector<double>& Xmin = ProblemeAResoudre->Xmin;
    std::vector<double>& Xmax = ProblemeAResoudre->Xmax;

    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        const CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
          = problemeHebdo->CorrespondanceVarNativesVarOptim[pdtJour];

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo
                                                                  ->PaliersThermiquesDuPays[pays];
            int maxThermalPlant = PaliersThermiquesDuPays.NombreDePaliersThermiques;

            for (int index = 0; index < maxThermalPlant; index++)
            {
                if (PaliersThermiquesDuPays.maxUpwardPowerRampingRate[index] >= 0)
                {
                    const int palier = PaliersThermiquesDuPays
                                         .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

                    int var = CorrespondanceVarNativesVarOptim.powerRampingDecreaseIndex[palier];
                    Xmin[var] = 0;
                    Xmax[var] = LINFINI_ANTARES;

                    var = CorrespondanceVarNativesVarOptim.powerRampingIncreaseIndex[palier];
                    Xmin[var] = 0;
                    Xmax[var] = LINFINI_ANTARES;
                }
            }
        }
    }
}
