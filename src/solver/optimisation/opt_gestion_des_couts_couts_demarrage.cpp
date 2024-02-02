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

#include <antares/study/area/scratchpad.h>
#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"

void OPT_InitialiserLesCoutsLineaireCoutsDeDemarrage(PROBLEME_HEBDO* problemeHebdo,
                                                     const int PremierPdtDeLIntervalle,
                                                     const int DernierPdtDeLIntervalle)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    std::vector<double>& CoutLineaire = ProblemeAResoudre->CoutLineaire;

    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        const CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
          =  problemeHebdo->CorrespondanceVarNativesVarOptim[pdtJour];

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays
              = problemeHebdo->PaliersThermiquesDuPays[pays];

            for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
            {
                const int palier
                  = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

                int var = CorrespondanceVarNativesVarOptim
                            .NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[palier];
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    CoutLineaire[var]
                      = PaliersThermiquesDuPays.CoutFixeDeMarcheDUnGroupeDuPalierThermique[index];
                }

                var = CorrespondanceVarNativesVarOptim
                        .NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[palier];
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    CoutLineaire[var]
                      = PaliersThermiquesDuPays.CoutDArretDUnGroupeDuPalierThermique[index];
                }

                var
                  = CorrespondanceVarNativesVarOptim
                      .NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique[palier];
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                    CoutLineaire[var] = 0;

                var = CorrespondanceVarNativesVarOptim
                        .NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[palier];
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    CoutLineaire[var]
                      = PaliersThermiquesDuPays.CoutDeDemarrageDUnGroupeDuPalierThermique[index];
                }
            }
        }
    }
}
