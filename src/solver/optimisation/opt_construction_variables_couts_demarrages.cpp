/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/opt_rename_problem.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_extern_variables_globales.h"

#include "spx_constantes_externes.h"
#include "variables/VariableManagerUtils.h"

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireCoutsDeDemarrage(
  PROBLEME_HEBDO* problemeHebdo,
  bool Simulation)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    int nombreDePasDeTempsPourUneOptimisation = problemeHebdo
                                                  ->NombreDePasDeTempsPourUneOptimisation;
    int nombreDeVariables = ProblemeAResoudre->NombreDeVariables;
    VariableNamer variableNamer(ProblemeAResoudre->NomDesVariables);
    const bool intVariables = problemeHebdo->OptimisationAvecVariablesEntieres;
    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        variableNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo
                                                              ->PaliersThermiquesDuPays[pays];

        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            const int palier = PaliersThermiquesDuPays
                                 .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
            const auto& clusterName = PaliersThermiquesDuPays.NomsDesPaliersThermiques[index];
            auto variableManager = VariableManagerFromProblemHebdo(problemeHebdo);

            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                variableNamer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
                if (Simulation)
                {
                    nombreDeVariables += 4;
                    continue;
                }

                variableManager.NumberOfDispatchableUnits(palier, pdt) = nombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[nombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;

                ProblemeAResoudre->VariablesEntieres[nombreDeVariables] = intVariables;
                variableNamer.NODU(nombreDeVariables, clusterName);
                nombreDeVariables++;

                variableManager.NumberStartingDispatchableUnits(palier, pdt) = nombreDeVariables;

                ProblemeAResoudre->TypeDeVariable[nombreDeVariables]
                  = VARIABLE_BORNEE_INFERIEUREMENT;
                ProblemeAResoudre->VariablesEntieres[nombreDeVariables] = intVariables;
                variableNamer.NumberStartingDispatchableUnits(nombreDeVariables, clusterName);
                nombreDeVariables++;

                variableManager.NumberStoppingDispatchableUnits(palier, pdt) = nombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[nombreDeVariables]
                  = VARIABLE_BORNEE_INFERIEUREMENT;
                ProblemeAResoudre->VariablesEntieres[nombreDeVariables] = intVariables;
                variableNamer.NumberStoppingDispatchableUnits(nombreDeVariables, clusterName);
                nombreDeVariables++;

                variableManager.NumberBreakingDownDispatchableUnits(palier, pdt)
                  = nombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[nombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;

                ProblemeAResoudre->VariablesEntieres[nombreDeVariables] = intVariables;
                variableNamer.NumberBreakingDownDispatchableUnits(nombreDeVariables, clusterName);
                nombreDeVariables++;
            }
        }
    }
    ProblemeAResoudre->NombreDeVariables = nombreDeVariables;
}
