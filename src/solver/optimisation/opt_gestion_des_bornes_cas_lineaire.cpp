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

#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_structure_probleme_economique.h"
#include "../simulation/adequacy_patch_runtime_data.h"

#include "opt_fonctions.h"
#include "adequacy_patch_local_matching/adq_patch_local_matching.h"
#include <yuni/core/math.h>

#include "spx_constantes_externes.h"
#include "variables/VariableManagement.h"
#include "variables/VariableManagerUtils.h"

using namespace Antares;
using namespace Antares::Data;

using namespace Yuni;

void OPT_MaxDesPmaxHydrauliques(PROBLEME_HEBDO* problemeHebdo)
{
    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        problemeHebdo->CaracteristiquesHydrauliques[pays].MaxDesPmaxHydrauliques = 0.0;
        if (!problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
            continue;
        const std::vector<double>& ContrainteDePmaxHydrauliqueHoraire
          = problemeHebdo->CaracteristiquesHydrauliques[pays].ContrainteDePmaxHydrauliqueHoraire;
        double pmaxHyd = -1;
        for (uint pdtHebdo = 0; pdtHebdo < problemeHebdo->NombreDePasDeTemps; pdtHebdo++)
        {
            if (ContrainteDePmaxHydrauliqueHoraire[pdtHebdo] > pmaxHyd)
                pmaxHyd = ContrainteDePmaxHydrauliqueHoraire[pdtHebdo];
        }

        problemeHebdo->CaracteristiquesHydrauliques[pays].MaxDesPmaxHydrauliques = pmaxHyd;
    }

    return;
}

double OPT_SommeDesPminThermiques(const PROBLEME_HEBDO* problemeHebdo, int Pays, uint pdtHebdo)
{
    double sommeDesPminThermiques = 0.0;
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays
      = problemeHebdo->PaliersThermiquesDuPays[Pays];
    const std::vector<PDISP_ET_COUTS_HORAIRES_PAR_PALIER>& PuissanceDisponibleEtCout
      = PaliersThermiquesDuPays.PuissanceDisponibleEtCout;

    for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
    {
        sommeDesPminThermiques
          += PuissanceDisponibleEtCout[index].PuissanceMinDuPalierThermique[pdtHebdo];
    }

    return sommeDesPminThermiques;
}

void setBoundsForUnsuppliedEnergy(PROBLEME_HEBDO* problemeHebdo,
                                  const AdqPatchParams& adqPatchParams,
                                  const int PremierPdtDeLIntervalle,
                                  const int DernierPdtDeLIntervalle,
                                  const int optimizationNumber)
{
    // OUTPUT
    std::vector<double>& Xmin = problemeHebdo->ProblemeAResoudre->Xmin;
    std::vector<double>& Xmax = problemeHebdo->ProblemeAResoudre->Xmax;
    std::vector<double*>& AdresseOuPlacerLaValeurDesVariablesOptimisees
      = problemeHebdo->ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees;

    const bool reserveJm1 = (problemeHebdo->YaDeLaReserveJmoins1);
    const bool opt1 = (optimizationNumber == PREMIERE_OPTIMISATION);
    auto variable_manager = VariableManagerFromProblemHebdo(problemeHebdo);

    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        const ALL_MUST_RUN_GENERATION& AllMustRunGeneration
          = problemeHebdo->AllMustRunGeneration[pdtHebdo];
        const CONSOMMATIONS_ABATTUES& ConsommationsAbattues
          = problemeHebdo->ConsommationsAbattues[pdtHebdo];

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            double ResidualLoadInArea = ConsommationsAbattues.ConsommationAbattueDuPays[pays];

            if (reserveJm1 && opt1)
            {
                ResidualLoadInArea
                  += problemeHebdo->ReserveJMoins1[pays].ReserveHoraireJMoins1[pdtHebdo];
            }

            int var = variable_manager.PositiveUnsuppliedEnergy(pays, pdtJour);
            Xmin[var] = 0.0;

            double MaxAllMustRunGenerationOfArea = 0.;
            if (AllMustRunGeneration.AllMustRunGenerationOfArea[pays] > 0.)
                MaxAllMustRunGenerationOfArea
                  = AllMustRunGeneration.AllMustRunGenerationOfArea[pays];

            ResidualLoadInArea += MaxAllMustRunGenerationOfArea;
            if (ResidualLoadInArea >= 0.)
                Xmax[var] = ResidualLoadInArea + 1e-5;
            else
                Xmax[var] = 0.;

            // adq patch: update ENS <= DENS in 2nd run
            if (adqPatchParams.enabled && adqPatchParams.localMatching.enabled
                && !problemeHebdo->adequacyPatchRuntimeData->AdequacyFirstStep
                && problemeHebdo->adequacyPatchRuntimeData->areaMode[pays]
                     == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
                Xmax[var] = std::min(
                  Xmax[var], problemeHebdo->ResultatsHoraires[pays].ValeursHorairesDENS[pdtHebdo]);

            problemeHebdo->ResultatsHoraires[pays].ValeursHorairesDeDefaillancePositive[pdtHebdo]
              = 0.0;

            AdresseOuPlacerLaValeurDesVariablesOptimisees[var]
              = &(problemeHebdo->ResultatsHoraires[pays]
                    .ValeursHorairesDeDefaillancePositive[pdtHebdo]);
        }
    }
}

static void setBoundsForShortTermStorage(PROBLEME_HEBDO* problemeHebdo,
                                         const int PremierPdtDeLIntervalle,
                                         const int DernierPdtDeLIntervalle)
{
    std::vector<double>& Xmin = problemeHebdo->ProblemeAResoudre->Xmin;
    std::vector<double>& Xmax = problemeHebdo->ProblemeAResoudre->Xmax;
    std::vector<double*>& AddressForVars
      = problemeHebdo->ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees;
    int weekFirstHour = problemeHebdo->weekInTheYear * 168;

    auto variable_manager = VariableManagerFromProblemHebdo(problemeHebdo);

    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        int hourInTheYear = weekFirstHour + pdtHebdo;
        for (uint32_t areaIndex = 0; areaIndex < problemeHebdo->NombreDePays; areaIndex++)
        {
            int storageIndex = 0;
            for (const auto& storage : problemeHebdo->ShortTermStorage[areaIndex])
            {
                const int clusterGlobalIndex = storage.clusterGlobalIndex;
                auto& STSResult
                  = problemeHebdo->ResultatsHoraires[areaIndex].ShortTermStorage[pdtHebdo];
                // 1. Injection
                int varInjection
                  = variable_manager.ShortTermStorageInjection(clusterGlobalIndex, pdtJour);
                Xmin[varInjection] = 0.;
                Xmax[varInjection] = storage.injectionNominalCapacity
                                     * storage.series->maxInjectionModulation[hourInTheYear];
                AddressForVars[varInjection] = &STSResult.injection[storageIndex];

                // 2. Withdrwal
                int varWithdrawal
                  = variable_manager.ShortTermStorageWithdrawal(clusterGlobalIndex, pdtJour);
                Xmin[varWithdrawal] = 0.;
                Xmax[varWithdrawal] = storage.withdrawalNominalCapacity
                                      * storage.series->maxWithdrawalModulation[hourInTheYear];
                AddressForVars[varWithdrawal] = &STSResult.withdrawal[storageIndex];

                // 3. Levels
                int varLevel = variable_manager.ShortTermStorageLevel(clusterGlobalIndex, pdtJour);
                if (pdtHebdo == DernierPdtDeLIntervalle - 1 && !storage.initialLevelOptim)
                {
                    Xmin[varLevel] = Xmax[varLevel]
                      = storage.reservoirCapacity * storage.initialLevel;
                }
                else
                {
                    Xmin[varLevel]
                      = storage.reservoirCapacity * storage.series->lowerRuleCurve[hourInTheYear];
                    Xmax[varLevel]
                      = storage.reservoirCapacity * storage.series->upperRuleCurve[hourInTheYear];
                }
                AddressForVars[varLevel] = &STSResult.level[storageIndex];

                storageIndex++;
            }
        }
    }
}

void OPT_InitialiserLesBornesDesVariablesDuProblemeLineaire(PROBLEME_HEBDO* problemeHebdo,
                                                            const AdqPatchParams& adqPatchParams,
                                                            const int PremierPdtDeLIntervalle,
                                                            const int DernierPdtDeLIntervalle,
                                                            const int optimizationNumber)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    std::vector<double*>& AdresseOuPlacerLaValeurDesVariablesOptimisees
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees;
    std::vector<double*>& AdresseOuPlacerLaValeurDesCoutsReduits
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits;
    std::vector<double>& Xmin = ProblemeAResoudre->Xmin;
    std::vector<double>& Xmax = ProblemeAResoudre->Xmax;
    std::vector<int>& TypeDeVariable = ProblemeAResoudre->TypeDeVariable;

    for (int i = 0; i < ProblemeAResoudre->NombreDeVariables; i++)
    {
        AdresseOuPlacerLaValeurDesVariablesOptimisees[i] = nullptr;
        AdresseOuPlacerLaValeurDesCoutsReduits[i] = nullptr;
    }

    auto variable_manager = VariableManagerFromProblemHebdo(problemeHebdo);
    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        VALEURS_DE_NTC_ET_RESISTANCES& ValeursDeNTC = problemeHebdo->ValeursDeNTC[pdtHebdo];

        for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
        {
            int var = variable_manager.NTCDirect(interco, pdtJour);
            const COUTS_DE_TRANSPORT& CoutDeTransport = problemeHebdo->CoutDeTransport[interco];

            AdequacyPatch::setNTCbounds(
              Xmax[var], Xmin[var], ValeursDeNTC, interco, problemeHebdo, adqPatchParams);

            if (Math::Infinite(Xmax[var]) == 1)
            {
                if (Math::Infinite(Xmin[var]) == -1)
                    TypeDeVariable[var] = VARIABLE_NON_BORNEE;
                else
                    TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
            }
            else
            {
                if (Math::Infinite(Xmin[var]) == -1)
                    TypeDeVariable[var] = VARIABLE_BORNEE_SUPERIEUREMENT;
                else
                    TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
            }

            double* adresseDuResultat
              = &(problemeHebdo->VariablesDualesDesContraintesDeNTC[pdtHebdo]
                    .VariableDualeParInterconnexion[interco]);
            AdresseOuPlacerLaValeurDesCoutsReduits[var] = adresseDuResultat;

            adresseDuResultat = &(ValeursDeNTC.ValeurDuFlux[interco]);
            AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;

            if (CoutDeTransport.IntercoGereeAvecDesCouts)
            {
                var = variable_manager.IntercoDirectCost(interco, pdtJour);

                if (CoutDeTransport.IntercoGereeAvecLoopFlow)
                    Xmax[var] = ValeursDeNTC.ValeurDeNTCOrigineVersExtremite[interco]
                                - ValeursDeNTC.ValeurDeLoopFlowOrigineVersExtremite[interco];
                else
                    Xmax[var] = ValeursDeNTC.ValeurDeNTCOrigineVersExtremite[interco];

                Xmax[var] += 0.01;
                TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
                if (Math::Infinite(Xmax[var]) == 1)
                {
                    TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
                }
                Xmin[var] = 0.0;
                AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;

                var = variable_manager.IntercoIndirectCost(interco, pdtJour);
                if (CoutDeTransport.IntercoGereeAvecLoopFlow)
                    Xmax[var] = ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine[interco]
                                + ValeursDeNTC.ValeurDeLoopFlowOrigineVersExtremite[interco];
                else
                    Xmax[var] = ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine[interco];

                Xmax[var] += 0.01;
                TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
                if (Math::Infinite(Xmax[var]) == 1)
                {
                    TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
                }
                Xmin[var] = 0.0;
                AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;
            }
        }

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays
              = problemeHebdo->PaliersThermiquesDuPays[pays];
            int maxThermalPlant = PaliersThermiquesDuPays.NombreDePaliersThermiques;

            for (int index = 0; index < maxThermalPlant; index++)
            {
                const int palier
                  = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
                int var = variable_manager.DispatchableProduction(palier, pdtJour);
                const PDISP_ET_COUTS_HORAIRES_PAR_PALIER& PuissanceDisponibleEtCout
                  = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[index];

                Xmin[var] = PuissanceDisponibleEtCout.PuissanceMinDuPalierThermique[pdtHebdo];

                Xmax[var]
                  = PuissanceDisponibleEtCout.PuissanceDisponibleDuPalierThermique[pdtHebdo];

                double* adresseDuResultat = &(problemeHebdo->ResultatsHoraires[pays]
                                                .ProductionThermique[pdtHebdo]
                                                .ProductionThermiqueDuPalier[index]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
            }

            int var = variable_manager.HydProd(pays, pdtJour);
            problemeHebdo->ResultatsHoraires[pays].TurbinageHoraire[pdtHebdo] = 0.0;
            if (var >= 0)
            {
                Xmin[var]
                  = problemeHebdo->CaracteristiquesHydrauliques[pays].MingenHoraire[pdtHebdo];
                Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                              .ContrainteDePmaxHydrauliqueHoraire[pdtHebdo];
                double* adresseDuResultat
                  = &(problemeHebdo->ResultatsHoraires[pays].TurbinageHoraire[pdtHebdo]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
            }

            if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
            {
                if (problemeHebdo->TypeDeLissageHydraulique
                    == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
                {
                    var = variable_manager.HydProdDown(pays, pdtJour);
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                    {
                        Xmin[var] = 0.0;
                        Xmax[var] = LINFINI_ANTARES;
                        AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;
                    }
                    var = variable_manager.HydProdUp(pays, pdtJour);
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                    {
                        Xmin[var] = 0.0;
                        Xmax[var] = LINFINI_ANTARES;
                        AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;
                    }
                }
                else if (problemeHebdo->TypeDeLissageHydraulique
                           == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX
                         && pdtJour == 0)
                {
                    var = variable_manager.HydProdDown(pays, pdtJour);
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                    {
                        Xmin[var] = 0.0;
                        Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                      .MaxDesPmaxHydrauliques;
                        AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;
                    }

                    var = variable_manager.HydProdUp(pays, pdtJour);
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                    {
                        Xmin[var] = 0.0;
                        Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                      .MaxDesPmaxHydrauliques;
                        AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;
                    }
                }
            }

            var = variable_manager.Pumping(pays, pdtJour);
            problemeHebdo->ResultatsHoraires[pays].PompageHoraire[pdtHebdo] = 0.0;
            if (var >= 0)
            {
                Xmin[var] = 0.0;
                Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                              .ContrainteDePmaxPompageHoraire[pdtHebdo];
                double* adresseDuResultat
                  = &(problemeHebdo->ResultatsHoraires[pays].PompageHoraire[pdtHebdo]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
            }

            var = variable_manager.Overflow(pays, pdtJour);

            problemeHebdo->ResultatsHoraires[pays].debordementsHoraires[pdtHebdo] = 0.;
            if (var >= 0)
            {
                Xmin[var] = 0.0;
                Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                              .ApportNaturelHoraire[pdtHebdo];
                AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;
            }

            var = variable_manager.HydroLevel(pays, pdtJour);
            if (var >= 0)
            {
                Xmin[var]
                  = problemeHebdo->CaracteristiquesHydrauliques[pays].NiveauHoraireInf[pdtHebdo];
                Xmax[var]
                  = problemeHebdo->CaracteristiquesHydrauliques[pays].NiveauHoraireSup[pdtHebdo];
                double* adresseDuResultat
                  = &(problemeHebdo->ResultatsHoraires[pays].niveauxHoraires[pdtHebdo]);
                AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
            }

            {
                var = variable_manager.NegativeUnsuppliedEnergy(pays, pdtJour);

                Xmin[var] = 0.0;

                Xmax[var] = LINFINI_ANTARES;

                problemeHebdo->ResultatsHoraires[pays]
                  .ValeursHorairesDeDefaillanceNegative[pdtHebdo]
                  = 0.0;
                double* adresseDuResultat = &(problemeHebdo->ResultatsHoraires[pays]
                                                .ValeursHorairesDeDefaillanceNegative[pdtHebdo]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
            }

            problemeHebdo->ResultatsHoraires[pays].ValeursHorairesDeDefaillanceEnReserve[pdtHebdo]
              = 0.0;
        }
    }

    setBoundsForUnsuppliedEnergy(problemeHebdo,
                                 adqPatchParams,
                                 PremierPdtDeLIntervalle,
                                 DernierPdtDeLIntervalle,
                                 optimizationNumber);

    setBoundsForShortTermStorage(problemeHebdo, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle);

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
        {
            int var = variable_manager.FinalStorage(
              pays, problemeHebdo->NombreDePasDeTempsPourUneOptimisation - 1);
            if (var >= 0)
            {
                Xmin[var] = -(LINFINI_ANTARES);
                Xmax[var] = LINFINI_ANTARES;

                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;

                //	Note: if there were a single optimization run instead of two; the following
                // could be used: 	adresseDuResultat =
                //&(problemeHebdo->CaracteristiquesHydrauliques[pays].LevelForTimeInterval);
                //	AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;

                AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
            }
            for (uint nblayer = 0; nblayer < 100; nblayer++)
            {
                var = variable_manager.LayerStorage(
                  pays, nblayer, problemeHebdo->NombreDePasDeTempsPourUneOptimisation - 1);
                if (var >= 0)
                {
                    Xmin[var] = 0;
                    Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[pays].TailleReservoir
                                / double(100);

                    AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;
                    AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
                }
            }
        }
    }

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    {
        OPT_InitialiserLesBornesDesVariablesDuProblemeLineaireCoutsDeDemarrage(
          problemeHebdo, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle);
    }

    return;
}
