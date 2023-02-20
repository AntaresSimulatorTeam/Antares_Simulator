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

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaire(PROBLEME_HEBDO* problemeHebdo)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    int NombreDePasDeTempsPourUneOptimisation = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
    int NombreDeVariables = 0;

    for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim
            = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];

        for (int interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
        {
            CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[interco]
              = NombreDeVariables;
            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            NombreDeVariables++;

            COUTS_DE_TRANSPORT* CoutDeTransport = problemeHebdo->CoutDeTransport[interco];
            if (CoutDeTransport->IntercoGereeAvecDesCouts == OUI_ANTARES)
            {
                CorrespondanceVarNativesVarOptim
                  ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[interco]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                NombreDeVariables++;
                CorrespondanceVarNativesVarOptim
                  ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[interco]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                NombreDeVariables++;
            }
        }

        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            PALIERS_THERMIQUES* PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[pays];
            for (int index = 0; index < PaliersThermiquesDuPays->NombreDePaliersThermiques; index++)
            {
                const int palier
                  = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
                CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[palier]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                NombreDeVariables++;
            }

            CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[pays]
              = NombreDeVariables;

            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            NombreDeVariables++;

            CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[pays]
              = NombreDeVariables;

            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_INFERIEUREMENT;
            NombreDeVariables++;
        }

        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            if (problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDHydrauliqueModulable
                == OUI_ANTARES)
            {
                CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[pays]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                NombreDeVariables++;
            }
            else
                CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[pays] = -1;

            CorrespondanceVarNativesVarOptim->NumeroDeVariablesVariationHydALaBaisse[pays] = -1;
            CorrespondanceVarNativesVarOptim->NumeroDeVariablesVariationHydALaHausse[pays] = -1;
            if (problemeHebdo->TypeDeLissageHydraulique
                == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
            {
                if (problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDHydrauliqueModulable
                    == OUI_ANTARES)
                {
                    CorrespondanceVarNativesVarOptim->NumeroDeVariablesVariationHydALaBaisse[pays]
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_INFERIEUREMENT;
                    NombreDeVariables++;
                    CorrespondanceVarNativesVarOptim->NumeroDeVariablesVariationHydALaHausse[pays]
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_INFERIEUREMENT;
                    NombreDeVariables++;
                }
            }
            else if (problemeHebdo->TypeDeLissageHydraulique
                     == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
            {
                if (problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDHydrauliqueModulable
                    == OUI_ANTARES)
                {
                    if (pdt == 0)
                    {
                        CorrespondanceVarNativesVarOptim
                          ->NumeroDeVariablesVariationHydALaBaisse[pays]
                          = NombreDeVariables;

                        ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                          = VARIABLE_BORNEE_DES_DEUX_COTES;
                        NombreDeVariables++;
                        CorrespondanceVarNativesVarOptim
                          ->NumeroDeVariablesVariationHydALaHausse[pays]
                          = NombreDeVariables;

                        ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                          = VARIABLE_BORNEE_DES_DEUX_COTES;
                        NombreDeVariables++;
                    }
                }
            }

            if (problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDePompageModulable
                == OUI_ANTARES)
            {
                CorrespondanceVarNativesVarOptim->NumeroDeVariablesDePompage[pays]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                NombreDeVariables++;
            }
            else
                CorrespondanceVarNativesVarOptim->NumeroDeVariablesDePompage[pays] = -1;

            if (problemeHebdo->CaracteristiquesHydrauliques[pays]->SuiviNiveauHoraire
                == OUI_ANTARES)
            {
                CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeNiveau[pays]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                NombreDeVariables++;
                CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeDebordement[pays]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                NombreDeVariables++;
            }
            else
            {
                CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeNiveau[pays] = -1;
                CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeDebordement[pays] = -1;
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays]->AccurateWaterValue == OUI_ANTARES)
        {
            problemeHebdo->NumeroDeVariableStockFinal[pays] = NombreDeVariables;
            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_NON_BORNEE;
            NombreDeVariables++;

            for (uint nblayer = 0; nblayer < 100; nblayer++)
            {
                problemeHebdo->NumeroDeVariableDeTrancheDeStock[pays][nblayer] = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                NombreDeVariables++;
            }
        }
        else
        {
            problemeHebdo->NumeroDeVariableStockFinal[pays] = -1;
            for (uint nblayer = 0; nblayer < 100; nblayer++)
            {
                problemeHebdo->NumeroDeVariableDeTrancheDeStock[pays][nblayer] = -1;
            }
        }
    }

    ProblemeAResoudre->NombreDeVariables = NombreDeVariables;

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage == OUI_ANTARES)
    {
        char simulation = NON_ANTARES;
        OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireCoutsDeDemarrage(problemeHebdo,
                                                                                      simulation);
    }

    return;
}
