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

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "variables/VariableManagerUtils.h"

void OPT_InitialiserLesCoutsLineaireCoutsDeDemarrage(PROBLEME_HEBDO*, const int, const int);

static void shortTermStorageCost(
  int PremierPdtDeLIntervalle,
  int DernierPdtDeLIntervalle,
  int NombreDePays,
  const std::vector<::ShortTermStorage::AREA_INPUT>& shortTermStorageInput,
  VariableManagement::VariableManager& variableManager,
  std::vector<double>& linearCost)
{
    for (int pays = 0; pays < NombreDePays; ++pays)
    {
        for (const auto& storage: shortTermStorageInput[pays])
        {
            for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0;
                 pdtHebdo < DernierPdtDeLIntervalle;
                 pdtHebdo++, pdtJour++)
            {
                const int clusterGlobalIndex = storage.clusterGlobalIndex;
                if (const int varLevel = variableManager.ShortTermStorageLevel(clusterGlobalIndex,
                                                                               pdtJour);
                    varLevel >= 0)
                {
                    linearCost[varLevel] = storage.series->costLevel[pdtHebdo];
                }

                if (const int varInjection = variableManager.ShortTermStorageInjection(
                      clusterGlobalIndex,
                      pdtJour);
                    varInjection >= 0)
                {
                    linearCost[varInjection] = storage.series->costInjection[pdtHebdo];
                }

                if (const int varWithdrawal = variableManager.ShortTermStorageWithdrawal(
                      clusterGlobalIndex,
                      pdtJour);
                    varWithdrawal >= 0)
                {
                    linearCost[varWithdrawal] = storage.series->costWithdrawal[pdtHebdo];
                }
                if (const int varCostVariationInjection = variableManager
                                                            .ShortTermStorageCostVariationInjection(
                                                              clusterGlobalIndex,
                                                              pdtJour);
                    storage.penalizeVariationInjection && varCostVariationInjection >= 0)
                {
                    linearCost[varCostVariationInjection] = storage.series
                                                              ->costVariationInjection[pdtHebdo];
                }
                if (const int varCostVariationWithdrawal
                    = variableManager.ShortTermStorageCostVariationWithdrawal(clusterGlobalIndex,
                                                                              pdtJour);
                    storage.penalizeVariationWithdrawal && varCostVariationWithdrawal >= 0)
                {
                    linearCost[varCostVariationWithdrawal] = storage.series
                                                               ->costVariationWithdrawal[pdtHebdo];
                }
            }
        }
    }
}

void OPT_InitialiserLesCoutsLineaire(PROBLEME_HEBDO* problemeHebdo,
                                     const int PremierPdtDeLIntervalle,
                                     const int DernierPdtDeLIntervalle)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    int pdtJour = 0;

    ProblemeAResoudre->CoutQuadratique.assign(ProblemeAResoudre->NombreDeVariables, 0.);
    auto variableManager = VariableManagerFromProblemHebdo(problemeHebdo);

    shortTermStorageCost(PremierPdtDeLIntervalle,
                         DernierPdtDeLIntervalle,
                         problemeHebdo->NombreDePays,
                         problemeHebdo->ShortTermStorage,
                         variableManager,
                         ProblemeAResoudre->CoutLineaire);

    for (int pdtHebdo = PremierPdtDeLIntervalle; pdtHebdo < DernierPdtDeLIntervalle; pdtHebdo++)
    {
        for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
        {
            const COUTS_DE_TRANSPORT& CoutDeTransport = problemeHebdo->CoutDeTransport[interco];

            int var = variableManager.NTCDirect(interco, pdtJour);
            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[var] = 0.0;
            }

            if (CoutDeTransport.IntercoGereeAvecDesCouts)
            {
                var = variableManager.IntercoDirectCost(interco, pdtJour);
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    ProblemeAResoudre->CoutLineaire[var] = CoutDeTransport
                                                             .CoutDeTransportOrigineVersExtremite
                                                               [pdtHebdo];
                }
                var = variableManager.IntercoIndirectCost(interco, pdtJour);
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    ProblemeAResoudre->CoutLineaire[var] = CoutDeTransport
                                                             .CoutDeTransportExtremiteVersOrigine
                                                               [pdtHebdo];
                }
            }
        }

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; ++pays)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo
                                                                  ->PaliersThermiquesDuPays[pays];
            int var;

            for (int Index = 0; Index < PaliersThermiquesDuPays.NombreDePaliersThermiques; Index++)
            {
                int palier = PaliersThermiquesDuPays
                               .NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
                var = variableManager.DispatchableProduction(palier, pdtJour);
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    ProblemeAResoudre->CoutLineaire[var]
                      = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[Index]
                          .CoutHoraireDeProductionDuPalierThermique[pdtHebdo];
                }
            }

            var = variableManager.HydProd(pays, pdtJour);
            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[var] = 0.0;
            }

            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[var] = problemeHebdo->BruitSurCoutHydraulique
                                                         [pays][problemeHebdo->HeureDansLAnnee
                                                                + pdtHebdo];

                if (!problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
                {
                    ProblemeAResoudre->CoutLineaire[var] += problemeHebdo
                                                              ->CaracteristiquesHydrauliques[pays]
                                                              .WeeklyWaterValueStateRegular;
                }
            }

            if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
            {
                double P;
                if (problemeHebdo->TypeDeLissageHydraulique
                    == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
                {
                    P = problemeHebdo->CaracteristiquesHydrauliques[pays]
                          .PenalisationDeLaVariationDeProductionHydrauliqueSurSommeDesVariations;

                    var = variableManager.HydProdDown(pays, pdtJour);
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                    {
                        ProblemeAResoudre->CoutLineaire[var] = P;
                    }
                    var = variableManager.HydProdUp(pays, pdtJour);
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                    {
                        ProblemeAResoudre->CoutLineaire[var] = P;
                    }
                }
                else if (problemeHebdo->TypeDeLissageHydraulique
                           == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX
                         && pdtJour == 0)
                {
                    P = problemeHebdo->CaracteristiquesHydrauliques[pays]
                          .PenalisationDeLaVariationDeProductionHydrauliqueSurVariationMax;
                    var = variableManager.HydProdDown(pays, pdtJour);

                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                    {
                        ProblemeAResoudre->CoutLineaire[var] = P;
                    }
                    var = variableManager.HydProdUp(pays, pdtJour);
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                    {
                        ProblemeAResoudre->CoutLineaire[var] = -P;
                    }
                }
            }

            var = variableManager.Pumping(pays, pdtJour);
            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            {
                /* Sets the cost of the pumping variable when such a variable is actually defined
                (i.e. var>=0)


                1-   When the "AccurateWaterValue" optimization mode is not used, the pumping
                variable must be given an explicit meaningful value, because no reservoir level
                variables are defined in this case. The pumping cost is based on the water value
                found for the initial reservoir level (with a sign change and weighted by the
                pumping ratio) In the eventuality of a zero-value water configuration, the pumping
                cost is then uplifted by an "espilon" component whose definition is given in the
                next section.

                2-   When the "AccurateWaterValue" optimization mode is used, the economic
                    contribution of pumping is mainly described through the cost set for the
                reservoir level variable ; as a consequence, the pumping variable is given only a
                residual "epsilon" cost based on the hydro generating "economic noise" The residual
                pumping cost is set to 2*fabs(generating noise), so as to make sure that, if the
                pumping ratio is equal to 1.0, no meaningless situations involving pumping and
                generating equal power amounts in the same location shall ever occur.

                */

                if (!problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
                {
                    ProblemeAResoudre->CoutLineaire[var] = problemeHebdo
                                                             ->CaracteristiquesHydrauliques[pays]
                                                             .WeeklyWaterValueStateRegular;

                    ProblemeAResoudre->CoutLineaire[var] *= problemeHebdo
                                                              ->CaracteristiquesHydrauliques[pays]
                                                              .PumpingRatio;
                    ProblemeAResoudre->CoutLineaire[var] *= -1.;

                    ProblemeAResoudre->CoutLineaire[var] += 2.
                                                            * fabs(
                                                              problemeHebdo->BruitSurCoutHydraulique
                                                                [pays]
                                                                [problemeHebdo->HeureDansLAnnee
                                                                 + pdtHebdo]);
                }
                else
                {
                    ProblemeAResoudre->CoutLineaire[var] = 2.
                                                           * fabs(
                                                             problemeHebdo->BruitSurCoutHydraulique
                                                               [pays][problemeHebdo->HeureDansLAnnee
                                                                      + pdtHebdo]);
                }
            }

            var = variableManager.Overflow(pays, pdtJour);
            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            {
                /* Sets the cost of the overflow variable when such a variable is actually defined
               (i.e. var>=0)


               1-   When the "AccurateWaterValue" optimization mode is not used, the overflow
               variable must be given a cost translating the fact that overflowing is at the same
               time:
                - A loss of a storage oppportunity, whose value is in this case the water value
               found for the initial level.
                - A spillage of power, whose value is the reference spillage cost.

               2-   When the "AccurateWaterValue" optimization mode is used, the economic loss of
               storage oppportunity incurred when overflowing is modelled through the costs of the
               reservoir level variables. The cost of the overflow variable involves therefore only
               a spillage component.

              */

                if (!problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
                {
                    ProblemeAResoudre->CoutLineaire[var] = problemeHebdo
                                                             ->CoutDeDefaillanceNegative[pays];

                    ProblemeAResoudre->CoutLineaire[var] += problemeHebdo
                                                              ->CaracteristiquesHydrauliques[pays]
                                                              .WeeklyWaterValueStateRegular;
                }
                else
                {
                    ProblemeAResoudre->CoutLineaire[var] = problemeHebdo
                                                             ->CoutDeDefaillanceNegative[pays];
                }
            }

            var = variableManager.HydroLevel(pays, pdtJour);
            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[var] = 0;
            }

            var = variableManager.PositiveUnsuppliedEnergy(pays, pdtJour);
            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[var] = problemeHebdo
                                                         ->CoutDeDefaillancePositive[pays];
            }

            var = variableManager.NegativeUnsuppliedEnergy(pays, pdtJour);
            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[var] = problemeHebdo
                                                         ->CoutDeDefaillanceNegative[pays];
            }
        }

        pdtJour++;
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; ++pays)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
        {
            int var = variableManager.FinalStorage(pays);
            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[var] = 0;
            }

            for (int layerindex = 0; layerindex < 100; layerindex++)
            {
                var = variableManager.LayerStorage(pays, layerindex);
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    ProblemeAResoudre->CoutLineaire[var] = -problemeHebdo
                                                              ->CaracteristiquesHydrauliques[pays]
                                                              .WaterLayerValues[layerindex];
                }
            }
        }
    }

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    {
        OPT_InitialiserLesCoutsLineaireCoutsDeDemarrage(problemeHebdo,
                                                        PremierPdtDeLIntervalle,
                                                        DernierPdtDeLIntervalle);
    }
}
