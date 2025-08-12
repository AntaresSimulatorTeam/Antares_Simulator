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
#include <spx_constantes_externes.h>

#include "antares/solver/optimisation/opt_rename_problem.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "variables/VariableManagerUtils.h"

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireCoutsDeDemarrage(PROBLEME_HEBDO*,
                                                                                   bool);

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaire(PROBLEME_HEBDO* problemeHebdo)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    int NombreDePasDeTempsPourUneOptimisation = problemeHebdo
                                                  ->NombreDePasDeTempsPourUneOptimisation;
    int NombreDeVariables = 0;
    VariableNamer variableNamer(ProblemeAResoudre->NomDesVariables);
    auto variableManager = VariableManagerFromProblemHebdo(problemeHebdo);

    for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        variableNamer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);

        for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
        {
            variableManager.NTCDirect(interco, pdt) = NombreDeVariables;
            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;

            const auto origin = problemeHebdo->NomsDesPays
                                  [problemeHebdo->PaysOrigineDeLInterconnexion[interco]];
            const auto destination = problemeHebdo->NomsDesPays
                                       [problemeHebdo->PaysExtremiteDeLInterconnexion[interco]];
            variableNamer.NTCDirect(NombreDeVariables, origin, destination);
            NombreDeVariables++;

            if (problemeHebdo->CoutDeTransport[interco].IntercoGereeAvecDesCouts)
            {
                variableManager.IntercoDirectCost(interco, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.IntercoDirectCost(NombreDeVariables, origin, destination);
                NombreDeVariables++;
                variableManager.IntercoIndirectCost(interco, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.IntercoIndirectCost(NombreDeVariables, origin, destination);
                NombreDeVariables++;
            }
        }

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo
                                                                  ->PaliersThermiquesDuPays[pays];
            variableNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
            {
                const int palier = PaliersThermiquesDuPays
                                     .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
                const auto& clusterName = PaliersThermiquesDuPays.NomsDesPaliersThermiques[index];
                variableManager.DispatchableProduction(palier, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.DispatchableProduction(NombreDeVariables, clusterName);
                NombreDeVariables++;
            }

            for (const auto& storage: problemeHebdo->ShortTermStorage[pays])
            {
                const int clusterGlobalIndex = storage.clusterGlobalIndex;
                // 1. Injection
                variableManager.ShortTermStorageInjection(clusterGlobalIndex, pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ShortTermStorageInjection(NombreDeVariables, storage.name);
                NombreDeVariables++;
                // 2. Withdrawal
                variableManager.ShortTermStorageWithdrawal(clusterGlobalIndex, pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ShortTermStorageWithdrawal(NombreDeVariables, storage.name);
                NombreDeVariables++;
                // 3. Level
                variableManager.ShortTermStorageLevel(clusterGlobalIndex, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ShortTermStorageLevel(NombreDeVariables, storage.name);
                NombreDeVariables++;

                // 4. Cost Variation Injection
                if (storage.penalizeVariationInjection)
                {
                    variableManager.ShortTermStorageCostVariationInjection(clusterGlobalIndex, pdt)
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_INFERIEUREMENT;
                    variableNamer.ShortTermStorageCostVariationInjection(NombreDeVariables,
                                                                         storage.name);
                    ++NombreDeVariables;
                }
                // 5. Cost Variation Withdrawal
                if (storage.penalizeVariationWithdrawal)
                {
                    variableManager.ShortTermStorageCostVariationWithdrawal(clusterGlobalIndex, pdt)
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_INFERIEUREMENT;
                    variableNamer.ShortTermStorageCostVariationWithdrawal(NombreDeVariables,
                                                                          storage.name);
                    ++NombreDeVariables;
                }
            }

            variableManager.PositiveUnsuppliedEnergy(pays, pdt) = NombreDeVariables;

            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            variableNamer.PositiveUnsuppliedEnergy(NombreDeVariables);
            NombreDeVariables++;

            variableManager.NegativeUnsuppliedEnergy(pays, pdt) = NombreDeVariables;

            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_INFERIEUREMENT;
            variableNamer.NegativeUnsuppliedEnergy(NombreDeVariables);
            NombreDeVariables++;
        }

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            variableNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
            {
                variableManager.HydProd(pays, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.HydProd(NombreDeVariables);
                NombreDeVariables++;
            }
            else
            {
                variableManager.HydProd(pays, pdt) = -1;
            }
            variableManager.HydProdDown(pays, pdt) = -1;
            variableManager.HydProdUp(pays, pdt) = -1;
            if (problemeHebdo->TypeDeLissageHydraulique
                == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
            {
                if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
                {
                    variableManager.HydProdDown(pays, pdt) = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_INFERIEUREMENT;
                    variableNamer.HydProdDown(NombreDeVariables);
                    NombreDeVariables++;
                    variableManager.HydProdUp(pays, pdt) = NombreDeVariables;
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
                variableManager.HydProdDown(pays, pdt) = NombreDeVariables;

                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.HydProdDown(NombreDeVariables);
                NombreDeVariables++;
                variableManager.HydProdUp(pays, pdt) = NombreDeVariables;

                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.HydProdUp(NombreDeVariables);
                NombreDeVariables++;
            }

            if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable)
            {
                variableManager.Pumping(pays, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.Pumping(NombreDeVariables);
                NombreDeVariables++;
            }
            else
            {
                variableManager.Pumping(pays, pdt) = -1;
            }

            if (problemeHebdo->CaracteristiquesHydrauliques[pays].SuiviNiveauHoraire)
            {
                variableManager.HydroLevel(pays, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.HydroLevel(NombreDeVariables);
                NombreDeVariables++;
                variableManager.Overflow(pays, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.Overflow(NombreDeVariables);
                NombreDeVariables++;
            }
            else
            {
                variableManager.HydroLevel(pays, pdt) = -1;
                variableManager.Overflow(pays, pdt) = -1;
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
            variableManager.FinalStorage(pays) = NombreDeVariables;
            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_NON_BORNEE;
            variableNamer.FinalStorage(NombreDeVariables);
            NombreDeVariables++;

            for (uint nblayer = 0; nblayer < 100; nblayer++)
            {
                variableManager.LayerStorage(pays, nblayer) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.LayerStorage(NombreDeVariables, nblayer);
                NombreDeVariables++;
            }
        }
        else
        {
            variableManager.FinalStorage(pays) = -1;
            for (uint nblayer = 0; nblayer < 100; nblayer++)
            {
                variableManager.LayerStorage(pays, nblayer) = -1;
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
