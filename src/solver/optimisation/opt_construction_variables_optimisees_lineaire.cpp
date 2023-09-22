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

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaire(PROBLEME_HEBDO* problemeHebdo)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre.get();

    int NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
    int NombreDeVariables = 0;
    VariableNamer variableNamer(ProblemeAResoudre->NomDesVariables);

    for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        variableNamer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        auto& CorrespondanceVarNativesVarOptim
          = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];

        for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
        {
            CorrespondanceVarNativesVarOptim.NumeroDeVariableDeLInterconnexion[interco]
              = NombreDeVariables;
            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;

            const auto origin
              = problemeHebdo->NomsDesPays[problemeHebdo->PaysOrigineDeLInterconnexion[interco]];
            const auto destination
              = problemeHebdo->NomsDesPays[problemeHebdo->PaysExtremiteDeLInterconnexion[interco]];
            variableNamer.NTCDirect(NombreDeVariables, origin, destination);
            NombreDeVariables++;

            if (problemeHebdo->CoutDeTransport[interco].IntercoGereeAvecDesCouts)
            {
                CorrespondanceVarNativesVarOptim
                  .NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[interco]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.IntercoDirectCost(NombreDeVariables, origin, destination);
                NombreDeVariables++;
                CorrespondanceVarNativesVarOptim
                  .NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[interco]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.IntercoIndirectCost(NombreDeVariables, origin, destination);
                NombreDeVariables++;
            }
        }

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays
              = problemeHebdo->PaliersThermiquesDuPays[pays];
            variableNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
            {
                const int palier
                  = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
                const auto& clusterName = PaliersThermiquesDuPays.NomsDesPaliersThermiques[index];
                CorrespondanceVarNativesVarOptim.NumeroDeVariableDuPalierThermique[palier]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.DispatchableProduction(NombreDeVariables, clusterName);
                NombreDeVariables++;
            }

            for (const auto& storage : problemeHebdo->ShortTermStorage[pays])
            {
                const int clusterGlobalIndex = storage.clusterGlobalIndex;
                // 1. Injection
                CorrespondanceVarNativesVarOptim.SIM_ShortTermStorage
                  .InjectionVariable[clusterGlobalIndex]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ShortTermStorageInjection(NombreDeVariables, storage.name);
                NombreDeVariables++;
                // 2. Withdrawal
                CorrespondanceVarNativesVarOptim.SIM_ShortTermStorage
                  .WithdrawalVariable[clusterGlobalIndex]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ShortTermStorageWithdrawal(NombreDeVariables, storage.name);
                NombreDeVariables++;
                // 3. Level
                CorrespondanceVarNativesVarOptim.SIM_ShortTermStorage
                  .LevelVariable[clusterGlobalIndex]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ShortTermStorageLevel(NombreDeVariables, storage.name);
                NombreDeVariables++;
            }

            CorrespondanceVarNativesVarOptim.NumeroDeVariableDefaillancePositive[pays]
              = NombreDeVariables;

            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            variableNamer.PositiveUnsuppliedEnergy(NombreDeVariables);
            NombreDeVariables++;

            CorrespondanceVarNativesVarOptim.NumeroDeVariableDefaillanceNegative[pays]
              = NombreDeVariables;

            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_INFERIEUREMENT;
            variableNamer.NegativeUnsuppliedEnergy(NombreDeVariables);
            NombreDeVariables++;
        }

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            variableNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
            {
                CorrespondanceVarNativesVarOptim.NumeroDeVariablesDeLaProdHyd[pays]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.HydProd(NombreDeVariables);
                NombreDeVariables++;
            }
            else
                CorrespondanceVarNativesVarOptim.NumeroDeVariablesDeLaProdHyd[pays] = -1;

            CorrespondanceVarNativesVarOptim.NumeroDeVariablesVariationHydALaBaisse[pays] = -1;
            CorrespondanceVarNativesVarOptim.NumeroDeVariablesVariationHydALaHausse[pays] = -1;
            if (problemeHebdo->TypeDeLissageHydraulique
                == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
            {
                if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
                {
                    CorrespondanceVarNativesVarOptim.NumeroDeVariablesVariationHydALaBaisse[pays]
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_INFERIEUREMENT;
                    variableNamer.HydProdDown(NombreDeVariables);
                    NombreDeVariables++;
                    CorrespondanceVarNativesVarOptim.NumeroDeVariablesVariationHydALaHausse[pays]
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_INFERIEUREMENT;
                    variableNamer.HydProdUp(NombreDeVariables);
                    NombreDeVariables++;
                }
            }
            else if (problemeHebdo->TypeDeLissageHydraulique
                       == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX
                     && problemeHebdo->CaracteristiquesHydrauliques[pays]
                          .PresenceDHydrauliqueModulable
                     && pdt == 0)
            {
                CorrespondanceVarNativesVarOptim.NumeroDeVariablesVariationHydALaBaisse[pays]
                  = NombreDeVariables;

                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.HydProdDown(NombreDeVariables);
                NombreDeVariables++;
                CorrespondanceVarNativesVarOptim.NumeroDeVariablesVariationHydALaHausse[pays]
                  = NombreDeVariables;

                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.HydProdUp(NombreDeVariables);
                NombreDeVariables++;
            }

            if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable)
            {
                CorrespondanceVarNativesVarOptim.NumeroDeVariablesDePompage[pays]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.Pumping(NombreDeVariables);
                NombreDeVariables++;
            }
            else
                CorrespondanceVarNativesVarOptim.NumeroDeVariablesDePompage[pays] = -1;

            if (problemeHebdo->CaracteristiquesHydrauliques[pays].SuiviNiveauHoraire)
            {
                CorrespondanceVarNativesVarOptim.NumeroDeVariablesDeNiveau[pays]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.HydroLevel(NombreDeVariables);
                NombreDeVariables++;
                CorrespondanceVarNativesVarOptim.NumeroDeVariablesDeDebordement[pays]
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.Overflow(NombreDeVariables);
                NombreDeVariables++;
            }
            else
            {
                CorrespondanceVarNativesVarOptim.NumeroDeVariablesDeNiveau[pays] = -1;
                CorrespondanceVarNativesVarOptim.NumeroDeVariablesDeDebordement[pays] = -1;
            }
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        variableNamer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168
                                     + NombreDePasDeTempsPourUneOptimisation - 1);
        variableNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
        {
            problemeHebdo->NumeroDeVariableStockFinal[pays] = NombreDeVariables;
            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_NON_BORNEE;
            variableNamer.FinalStorage(NombreDeVariables);
            NombreDeVariables++;

            for (uint nblayer = 0; nblayer < 100; nblayer++)
            {
                problemeHebdo->NumeroDeVariableDeTrancheDeStock[pays][nblayer] = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.LayerStorage(NombreDeVariables, nblayer);
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

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    {
        OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireCoutsDeDemarrage(problemeHebdo,
                                                                                      false);
    }

    return;
}
