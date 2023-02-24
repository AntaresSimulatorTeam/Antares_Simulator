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

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"
#include <math.h>

#include "spx_constantes_externes.h"

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireCoutsDeDemarrage(
  PROBLEME_HEBDO* problemeHebdo,
  bool Simulation)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    int nombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
    int nombreDeVariables = ProblemeAResoudre->NombreDeVariables;

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES* PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[pays];

        for (int index = 0; index < PaliersThermiquesDuPays->NombreDePaliersThermiques; index++)
        {
            const int palier
              = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                if (Simulation == OUI_ANTARES)
                {
                    nombreDeVariables += 4;
                    continue;
                }
                CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim
                  = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];

                CorrespondanceVarNativesVarOptim
                  ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[palier]
                  = nombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[nombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                nombreDeVariables++;

                CorrespondanceVarNativesVarOptim
                  ->NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[palier]
                  = nombreDeVariables;

                ProblemeAResoudre->TypeDeVariable[nombreDeVariables]
                  = VARIABLE_BORNEE_INFERIEUREMENT;
                nombreDeVariables++;

                CorrespondanceVarNativesVarOptim
                  ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[palier]
                  = nombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[nombreDeVariables]
                  = VARIABLE_BORNEE_INFERIEUREMENT;
                nombreDeVariables++;

                CorrespondanceVarNativesVarOptim
                  ->NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique[palier]
                  = nombreDeVariables;
#if VARIABLES_MMOINS_MOINS_BORNEES_DES_2_COTES != OUI_ANTARES
                ProblemeAResoudre->TypeDeVariable[nombreDeVariables]
                  = VARIABLE_BORNEE_INFERIEUREMENT;
#else
                ProblemeAResoudre->TypeDeVariable[nombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
#endif
                nombreDeVariables++;
            }
        }
    }

    ProblemeAResoudre->NombreDeVariables = nombreDeVariables;

    return;
}
