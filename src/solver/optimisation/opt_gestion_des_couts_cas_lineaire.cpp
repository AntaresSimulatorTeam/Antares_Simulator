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

#include <yuni/yuni.h>
#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/sim_spread_generator.h"
#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"
#include "variables/VariableManagerUtils.h"

#include "opt_fonctions.h"

static void shortTermStorageCost(
  int PremierPdtDeLIntervalle,
  int DernierPdtDeLIntervalle,
  int NombreDePays,
  const std::vector<::ShortTermStorage::AREA_INPUT>& shortTermStorageInput,
  VariableManagement::VariableManager& variable_manager,
  std::vector<double>& linearCost)
{
    SIM::SpreadGenerator spreadGenerator;
    for (int pays = 0; pays < NombreDePays; ++pays)
    {
        for (const auto& storage : shortTermStorageInput[pays])
        {
            for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0;
                 pdtHebdo < DernierPdtDeLIntervalle;
                 pdtHebdo++, pdtJour++)
            {
                const int clusterGlobalIndex = storage.clusterGlobalIndex;
                if (const int varLevel
                    = variable_manager.ShortTermStorageLevel(clusterGlobalIndex, pdtJour);
                    varLevel >= 0)
                {
                    linearCost[varLevel] = 0;
                }

                const double cost = spreadGenerator.generate();
                if (const int varInjection
                    = variable_manager.ShortTermStorageInjection(clusterGlobalIndex, pdtJour);
                    varInjection >= 0)
                {
                    linearCost[varInjection] = cost;
                }

                if (const int varWithdrawal
                    = variable_manager.ShortTermStorageWithdrawal(clusterGlobalIndex, pdtJour);
                    varWithdrawal >= 0)
                {
                    linearCost[varWithdrawal] = storage.efficiency * cost;
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
    auto variable_manager = VariableManagerFromProblemHebdo(problemeHebdo);

    shortTermStorageCost(PremierPdtDeLIntervalle,
                         DernierPdtDeLIntervalle,
                         problemeHebdo->NombreDePays,
                         problemeHebdo->ShortTermStorage,
                         variable_manager,
                         ProblemeAResoudre->CoutLineaire);

    for (int pdtHebdo = PremierPdtDeLIntervalle; pdtHebdo < DernierPdtDeLIntervalle; pdtHebdo++)
    {
        for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
        {
            const COUTS_DE_TRANSPORT& CoutDeTransport = problemeHebdo->CoutDeTransport[interco];

            int var = variable_manager.NTCDirect(interco, pdtJour);
            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[var] = 0.0;
            }

            if (CoutDeTransport.IntercoGereeAvecDesCouts)
            {
                var = variable_manager.IntercoDirectCost(interco, pdtJour);
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    ProblemeAResoudre->CoutLineaire[var]
                      = CoutDeTransport.CoutDeTransportOrigineVersExtremite[pdtHebdo];
                }
                var = variable_manager.IntercoIndirectCost(interco, pdtJour);
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    ProblemeAResoudre->CoutLineaire[var]
                      = CoutDeTransport.CoutDeTransportExtremiteVersOrigine[pdtHebdo];
                }
            }
        }

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; ++pays)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays
              = problemeHebdo->PaliersThermiquesDuPays[pays];
            int var;

            for (int Index = 0; Index < PaliersThermiquesDuPays.NombreDePaliersThermiques; Index++)
            {
                int palier
                  = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
                var = variable_manager.DispatchableProduction(palier, pdtJour);
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    ProblemeAResoudre->CoutLineaire[var]
                      = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[Index]
                          .CoutHoraireDeProductionDuPalierThermique[pdtHebdo];
                }
            }

            var = variable_manager.HydProd(pays, pdtJour);
            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                ProblemeAResoudre->CoutLineaire[var] = 0.0;

            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[var]
                  = problemeHebdo
                      ->BruitSurCoutHydraulique[pays][problemeHebdo->HeureDansLAnnee + pdtHebdo];

                if (!problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
                    ProblemeAResoudre->CoutLineaire[var]
                      += problemeHebdo->CaracteristiquesHydrauliques[pays]
                           .WeeklyWaterValueStateRegular;
            }

            if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
            {
                double P;
                if (problemeHebdo->TypeDeLissageHydraulique
                    == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
                {
                    P = problemeHebdo->CaracteristiquesHydrauliques[pays]
                          .PenalisationDeLaVariationDeProductionHydrauliqueSurSommeDesVariations;

                    var = variable_manager.HydProdDown(pays, pdtJour);
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                        ProblemeAResoudre->CoutLineaire[var] = P;
                    var = variable_manager.HydProdUp(pays, pdtJour);
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                        ProblemeAResoudre->CoutLineaire[var] = P;
                }
                else if (problemeHebdo->TypeDeLissageHydraulique
                           == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX
                         && pdtJour == 0)
                {
                    P = problemeHebdo->CaracteristiquesHydrauliques[pays]
                          .PenalisationDeLaVariationDeProductionHydrauliqueSurVariationMax;
                    var = variable_manager.HydProdDown(pays, pdtJour);

                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                        ProblemeAResoudre->CoutLineaire[var] = P;
                    var = variable_manager.HydProdUp(pays, pdtJour);
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                        ProblemeAResoudre->CoutLineaire[var] = -P;
                }
            }

            var = variable_manager.Pumping(pays, pdtJour);
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
                    ProblemeAResoudre->CoutLineaire[var]
                      = problemeHebdo->CaracteristiquesHydrauliques[pays]
                          .WeeklyWaterValueStateRegular;

                    ProblemeAResoudre->CoutLineaire[var]
                      *= problemeHebdo->CaracteristiquesHydrauliques[pays].PumpingRatio;
                    ProblemeAResoudre->CoutLineaire[var] *= -1.;

                    ProblemeAResoudre->CoutLineaire[var]
                      += 2.
                         * fabs(problemeHebdo
                                  ->BruitSurCoutHydraulique[pays][problemeHebdo->HeureDansLAnnee
                                                                  + pdtHebdo]);
                }
                else
                {
                    ProblemeAResoudre->CoutLineaire[var]
                      = 2.
                        * fabs(problemeHebdo
                                 ->BruitSurCoutHydraulique[pays][problemeHebdo->HeureDansLAnnee
                                                                 + pdtHebdo]);
                }
            }

            var = variable_manager.Overflow(pays, pdtJour);
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
                    ProblemeAResoudre->CoutLineaire[var]
                      = problemeHebdo->CoutDeDefaillanceNegative[pays];

                    ProblemeAResoudre->CoutLineaire[var]
                      += problemeHebdo->CaracteristiquesHydrauliques[pays]
                           .WeeklyWaterValueStateRegular;
                }
                else
                {
                    ProblemeAResoudre->CoutLineaire[var]
                      = problemeHebdo->CoutDeDefaillanceNegative[pays];
                }
            }

            var = variable_manager.HydroLevel(pays, pdtJour);
            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[var] = 0;
            }

            var = variable_manager.PositiveUnsuppliedEnergy(pays, pdtJour);
            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[var]
                  = problemeHebdo->CoutDeDefaillancePositive[pays];
            }

            var = variable_manager.NegativeUnsuppliedEnergy(pays, pdtJour);
            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[var]
                  = problemeHebdo->CoutDeDefaillanceNegative[pays];
            }
        }

        pdtJour++;
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; ++pays)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
        {
            int var = variable_manager.FinalStorage(
              pays, problemeHebdo->NombreDePasDeTempsPourUneOptimisation - 1);
            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[var] = 0;
            }

            for (int layerindex = 0; layerindex < 100; layerindex++)
            {
                var = variable_manager.LayerStorage(
                  pays, layerindex, problemeHebdo->NombreDePasDeTempsPourUneOptimisation - 1);
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    ProblemeAResoudre->CoutLineaire[var]
                      = -problemeHebdo->CaracteristiquesHydrauliques[pays]
                           .WaterLayerValues[layerindex];
                }
            }
        }
    }

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    {
        OPT_InitialiserLesCoutsLineaireCoutsDeDemarrage(
          problemeHebdo, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle);
    }

    return;
}
