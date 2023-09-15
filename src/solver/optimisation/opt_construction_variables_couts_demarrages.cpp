/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"
#include "opt_rename_problem.h"

#include "spx_constantes_externes.h"

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireCoutsDeDemarrage(
  PROBLEME_HEBDO* problemeHebdo,
  bool Simulation)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre.get();

    int nombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
    int& nombreDeVariables = ProblemeAResoudre->NombreDeVariables;
    VariableNamer variableNamer(ProblemeAResoudre->NomDesVariables);
    const bool intVariables = problemeHebdo->OptimisationAvecVariablesEntieres;
    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        variableNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[pays];

        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            const int palier
              = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
            const auto& clusterName = PaliersThermiquesDuPays.NomsDesPaliersThermiques[index];

            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                variableNamer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
                if (Simulation)
                {
                    nombreDeVariables += 4;
                    continue;
                }
                CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
                  =  problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];

                CorrespondanceVarNativesVarOptim
                  .NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[palier]
                  = nombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[nombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;

                ProblemeAResoudre->VariablesEntieres[nombreDeVariables] = intVariables;
                variableNamer.NODU(nombreDeVariables, clusterName);
                nombreDeVariables++;

                CorrespondanceVarNativesVarOptim
                  .NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[palier]
                  = nombreDeVariables;

                ProblemeAResoudre->TypeDeVariable[nombreDeVariables]
                  = VARIABLE_BORNEE_INFERIEUREMENT;
                ProblemeAResoudre->VariablesEntieres[nombreDeVariables] = intVariables;
                variableNamer.NumberStartingDispatchableUnits(nombreDeVariables, clusterName);
                nombreDeVariables++;

                CorrespondanceVarNativesVarOptim
                  .NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[palier]
                  = nombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[nombreDeVariables]
                  = VARIABLE_BORNEE_INFERIEUREMENT;
                ProblemeAResoudre->VariablesEntieres[nombreDeVariables] = intVariables;
                variableNamer.NumberStoppingDispatchableUnits(nombreDeVariables, clusterName);
                nombreDeVariables++;

                CorrespondanceVarNativesVarOptim
                  .NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique[palier]
                  = nombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[nombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;

                ProblemeAResoudre->VariablesEntieres[nombreDeVariables] = intVariables;
                variableNamer.NumberBreakingDownDispatchableUnits(nombreDeVariables, clusterName);
                nombreDeVariables++;
            }
        }
    }
}
