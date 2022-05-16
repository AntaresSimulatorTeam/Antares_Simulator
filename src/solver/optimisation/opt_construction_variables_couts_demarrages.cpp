/*
** Copyright 2007-2018 RTE
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
  PROBLEME_HEBDO* ProblemeHebdo,
  char Simulation)
{
    int Pays;
    int Pdt;
    int Palier;
    int NombreDeVariables;
    int Index;
    int NombreDePasDeTempsPourUneOptimisation;
    char ContrainteDeReserveJMoins1ParZone;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    PALIERS_THERMIQUES* PaliersThermiquesDuPays;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

    NombreDePasDeTempsPourUneOptimisation = ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation;
    ContrainteDeReserveJMoins1ParZone = ProblemeHebdo->ContrainteDeReserveJMoins1ParZone;

    NombreDeVariables = ProblemeAResoudre->NombreDeVariables;

    for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
    {
        PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];

        for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++)
        {
            Palier
              = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];

            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                if (Simulation == OUI_ANTARES)
                {
                    NombreDeVariables += 4;
                    continue;
                }
                CorrespondanceVarNativesVarOptim
                  = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];

                CorrespondanceVarNativesVarOptim
                  ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[Palier]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                if (ProblemeHebdo->OptimisationAvecVariablesEntieres)
                    ProblemeAResoudre->VariablesEntieres[NombreDeVariables] = true;
                NombreDeVariables++;

                CorrespondanceVarNativesVarOptim
                  ->NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[Palier]
                  = NombreDeVariables;

#if SUBSTITUTION_DE_LA_VARIABLE_MPLUS == OUI_ANTARES
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
#else
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_INFERIEUREMENT;
#endif
                if (ProblemeHebdo->OptimisationAvecVariablesEntieres)
                    ProblemeAResoudre->VariablesEntieres[NombreDeVariables] = true;
                NombreDeVariables++;

                CorrespondanceVarNativesVarOptim
                  ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[Palier]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_INFERIEUREMENT;
                if (ProblemeHebdo->OptimisationAvecVariablesEntieres)
                    ProblemeAResoudre->VariablesEntieres[NombreDeVariables] = true;
                NombreDeVariables++;

                CorrespondanceVarNativesVarOptim
                  ->NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique[Palier]
                  = NombreDeVariables;
#if VARIABLES_MMOINS_MOINS_BORNEES_DES_2_COTES != OUI_ANTARES
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_INFERIEUREMENT;
#else
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
#endif
                if (ProblemeHebdo->OptimisationAvecVariablesEntieres)
                    ProblemeAResoudre->VariablesEntieres[NombreDeVariables] = true;
                NombreDeVariables++;
            }
        }
    }

#if GROSSES_VARIABLES == OUI_ANTARES
    for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
    {
        for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
        {
            CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];
            if (Simulation == OUI_ANTARES)
            {
                NombreDeVariables++;
                NombreDeVariables++;
                if (ContrainteDeReserveJMoins1ParZone == OUI_ANTARES)
                {
                    NombreDeVariables++;
                }
                continue;
            }

            CorrespondanceVarNativesVarOptim->NumeroDeGrosseVariableDefaillancePositive[Pays]
              = NombreDeVariables;
            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_NON_BORNEE;
            ProblemeAResoudre->VariablesEntieres[NombreDeVariables] = false;
            NombreDeVariables++;

            CorrespondanceVarNativesVarOptim->NumeroDeGrosseVariableDefaillanceNegative[Pays]
              = NombreDeVariables;
            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_NON_BORNEE;
            ProblemeAResoudre->VariablesEntieres[NombreDeVariables] = false;
            NombreDeVariables++;

            if (ContrainteDeReserveJMoins1ParZone == OUI_ANTARES)
            {
                CorrespondanceVarNativesVarOptim->NumeroDeGrosseVariableDefaillanceEnReserve[Pays]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_NON_BORNEE;
                ProblemeAResoudre->VariablesEntieres[NombreDeVariables] = false;
                NombreDeVariables++;
            }
        }
    }
#endif

    ProblemeAResoudre->NombreDeVariables = NombreDeVariables;

    return;
}
