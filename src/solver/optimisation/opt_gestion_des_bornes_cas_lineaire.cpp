// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <cmath>
#include <spx_constantes_externes.h>

#include "antares/solver/optimisation/opt_gestion_des_bornes_reserves.h"
#include "antares/solver/optimisation/variables/VariableManagerUtils.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

void OPT_InitialiserLesBornesDesVariablesDuProblemeLineaireCoutsDeDemarrage(PROBLEME_HEBDO*,
                                                                            const int,
                                                                            const int);

void OPT_MaxDesPmaxHydrauliques(PROBLEME_HEBDO* problemeHebdo)
{
    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        problemeHebdo->CaracteristiquesHydrauliques[pays].MaxDesPmaxHydrauliques = 0.0;
        if (!problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
        {
            continue;
        }
        const std::vector<double>& ContrainteDePmaxHydrauliqueHoraire
          = problemeHebdo->CaracteristiquesHydrauliques[pays].ContrainteDePmaxHydrauliqueHoraire;
        double pmaxHyd = -1;
        for (uint pdtHebdo = 0; pdtHebdo < problemeHebdo->NombreDePasDeTemps; pdtHebdo++)
        {
            if (ContrainteDePmaxHydrauliqueHoraire[pdtHebdo] > pmaxHyd)
            {
                pmaxHyd = ContrainteDePmaxHydrauliqueHoraire[pdtHebdo];
            }
        }

        problemeHebdo->CaracteristiquesHydrauliques[pays].MaxDesPmaxHydrauliques = pmaxHyd;
    }
}

double OPT_SommeDesPminThermiques(const PROBLEME_HEBDO* problemeHebdo, int Pays, uint pdtHebdo)
{
    double sommeDesPminThermiques = 0.0;
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo
                                                          ->PaliersThermiquesDuPays[Pays];
    const std::vector<PDISP_ET_COUTS_HORAIRES_PAR_PALIER>& PuissanceDisponibleEtCout
      = PaliersThermiquesDuPays.PuissanceDisponibleEtCout;

    for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
    {
        sommeDesPminThermiques += PuissanceDisponibleEtCout[index]
                                    .PuissanceMinDuPalierThermique[pdtHebdo];
    }

    return sommeDesPminThermiques;
}

void setBoundsForUnsuppliedEnergy(PROBLEME_HEBDO* problemeHebdo,
                                  const int PremierPdtDeLIntervalle,
                                  const int DernierPdtDeLIntervalle)
{
    // OUTPUT
    std::vector<double>& Xmin = problemeHebdo->ProblemeAResoudre->Xmin;
    std::vector<double>& Xmax = problemeHebdo->ProblemeAResoudre->Xmax;
    std::vector<double*>& AdresseOuPlacerLaValeurDesVariablesOptimisees
      = problemeHebdo->ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees;

    auto variableManager = VariableManagerFromProblemHebdo(problemeHebdo);

    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            int var = variableManager.UnsuppliedEnergy(pays, pdtJour);
            Xmin[var] = 0.0;
            Xmax[var] = LINFINI_ANTARES;

            problemeHebdo->ResultatsHoraires[pays].ValeursHorairesDeDefaillancePositive[pdtHebdo]
              = 0.0;

            AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = &(
              problemeHebdo->ResultatsHoraires[pays]
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
    std::vector<double*>& AddressForVars = problemeHebdo->ProblemeAResoudre
                                             ->AdresseOuPlacerLaValeurDesVariablesOptimisees;
    int weekFirstHour = problemeHebdo->weekInTheYear * 168;

    auto variableManager = VariableManagerFromProblemHebdo(problemeHebdo);

    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        int hourInTheYear = weekFirstHour + pdtHebdo;
        for (uint32_t areaIndex = 0; areaIndex < problemeHebdo->NombreDePays; areaIndex++)
        {
            int storageIndex = 0;
            for (const auto& storage: problemeHebdo->ShortTermStorage[areaIndex])
            {
                const int clusterGlobalIndex = storage.clusterGlobalIndex;
                auto& STSResult = problemeHebdo->ResultatsHoraires[areaIndex]
                                    .ShortTermStorage[storageIndex];
                // 1. Injection
                int varInjection = variableManager.ShortTermStorageInjection(clusterGlobalIndex,
                                                                             pdtJour);
                Xmin[varInjection] = 0.;
                Xmax[varInjection] = storage.injectionNominalCapacity
                                     * storage.series->maxInjectionModulation[hourInTheYear];
                AddressForVars[varInjection] = &STSResult.injection[pdtHebdo];

                // 2. Withdrwal
                int varWithdrawal = variableManager.ShortTermStorageWithdrawal(clusterGlobalIndex,
                                                                               pdtJour);
                Xmin[varWithdrawal] = 0.;
                Xmax[varWithdrawal] = storage.withdrawalNominalCapacity
                                      * storage.series->maxWithdrawalModulation[hourInTheYear];
                AddressForVars[varWithdrawal] = &STSResult.withdrawal[pdtHebdo];

                // 3. Levels
                int varLevel = variableManager.ShortTermStorageLevel(clusterGlobalIndex, pdtJour);
                if (pdtHebdo == DernierPdtDeLIntervalle - 1 && !storage.initialLevelOptim)
                {
                    Xmin[varLevel] = Xmax[varLevel] = storage.reservoirCapacity
                                                      * storage.initialLevel;
                }
                else
                {
                    Xmin[varLevel] = storage.reservoirCapacity
                                     * storage.series->lowerRuleCurve[hourInTheYear];
                    Xmax[varLevel] = storage.reservoirCapacity
                                     * storage.series->upperRuleCurve[hourInTheYear];
                }
                AddressForVars[varLevel] = &STSResult.level[pdtHebdo];
                // 4. Cost Variation Injection

                // is this necessary?
                if (storage.penalizeVariationInjection)
                {
                    int varCostVariationInjection = variableManager
                                                      .ShortTermStorageCostVariationInjection(
                                                        clusterGlobalIndex,
                                                        pdtJour);

                    Xmin[varCostVariationInjection] = 0.;
                }
                // 5. Cost Variation Withdrawal
                // is this necessary?
                if (storage.penalizeVariationWithdrawal)
                {
                    int varCostVariationWithdrawal = variableManager
                                                       .ShortTermStorageCostVariationWithdrawal(
                                                         clusterGlobalIndex,
                                                         pdtJour);
                    Xmin[varCostVariationWithdrawal] = 0.;
                }
                storageIndex++;
                if (storage.allowOverflow)
                {
                    int var = variableManager.ShortTermStorageOverflow(clusterGlobalIndex, pdtJour);
                    Xmin[var] = 0;
                    Xmax[var] = LINFINI_ANTARES;
                    AddressForVars[var] = &STSResult.overflow[pdtHebdo];
                }
            }
        }
    }
}

void OPT_InitialiserLesBornesDesVariablesDuProblemeLineaire(PROBLEME_HEBDO* problemeHebdo,
                                                            const int PremierPdtDeLIntervalle,
                                                            const int DernierPdtDeLIntervalle)
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

    auto variableManager = VariableManagerFromProblemHebdo(problemeHebdo);
    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        VALEURS_DE_NTC_ET_RESISTANCES& ValeursDeNTC = problemeHebdo->ValeursDeNTC[pdtHebdo];

        for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
        {
            int var = variableManager.DirectFlow(interco, pdtJour);
            const COUTS_DE_TRANSPORT& CoutDeTransport = problemeHebdo->CoutDeTransport[interco];

            Xmax[var] = ValeursDeNTC.ValeurDeNTCOrigineVersExtremite[interco];
            Xmin[var] = -(ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine[interco]);

            if (std::isinf(Xmax[var]) && Xmax[var] > 0)
            {
                if (std::isinf(Xmin[var]) && Xmin[var] < 0)
                {
                    TypeDeVariable[var] = VARIABLE_NON_BORNEE;
                }
                else
                {
                    TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
                }
            }
            else
            {
                if (std::isinf(Xmin[var]) && Xmin[var] < 0)
                {
                    TypeDeVariable[var] = VARIABLE_BORNEE_SUPERIEUREMENT;
                }
                else
                {
                    TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
                }
            }

            double* adresseDuResultat = &(
              problemeHebdo->VariablesDualesDesContraintesDeNTC[pdtHebdo]
                .VariableDualeParInterconnexion[interco]);
            AdresseOuPlacerLaValeurDesCoutsReduits[var] = adresseDuResultat;

            adresseDuResultat = &(ValeursDeNTC.ValeurDuFlux[interco]);
            AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;

            if (CoutDeTransport.IntercoGereeAvecDesCouts)
            {
                var = variableManager.PositiveDirectFlow(interco, pdtJour);

                if (CoutDeTransport.IntercoGereeAvecLoopFlow)
                {
                    Xmax[var] = ValeursDeNTC.ValeurDeNTCOrigineVersExtremite[interco]
                                - ValeursDeNTC.ValeurDeLoopFlowOrigineVersExtremite[interco];
                }
                else
                {
                    Xmax[var] = ValeursDeNTC.ValeurDeNTCOrigineVersExtremite[interco];
                }

                Xmax[var] += 0.01;
                TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
                if (std::isinf(Xmax[var]) && Xmax[var] > 0)
                {
                    TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
                }
                Xmin[var] = 0.0;
                AdresseOuPlacerLaValeurDesCoutsReduits[var] = nullptr;
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = nullptr;

                var = variableManager.PositiveIndirectFlow(interco, pdtJour);
                if (CoutDeTransport.IntercoGereeAvecLoopFlow)
                {
                    Xmax[var] = ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine[interco]
                                + ValeursDeNTC.ValeurDeLoopFlowOrigineVersExtremite[interco];
                }
                else
                {
                    Xmax[var] = ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine[interco];
                }

                Xmax[var] += 0.01;
                TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
                if (std::isinf(Xmax[var]) && Xmax[var] > 0)
                {
                    TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
                }
                Xmin[var] = 0.0;
                AdresseOuPlacerLaValeurDesCoutsReduits[var] = nullptr;
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = nullptr;
            }
        }

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo
                                                                  ->PaliersThermiquesDuPays[pays];
            int maxThermalPlant = PaliersThermiquesDuPays.NombreDePaliersThermiques;

            for (int index = 0; index < maxThermalPlant; index++)
            {
                const int palier = PaliersThermiquesDuPays
                                     .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
                int var = variableManager.DispatchableProduction(palier, pdtJour);
                const PDISP_ET_COUTS_HORAIRES_PAR_PALIER& PuissanceDisponibleEtCout
                  = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[index];

                Xmin[var] = PuissanceDisponibleEtCout.PuissanceMinDuPalierThermique[pdtHebdo];

                Xmax[var] = PuissanceDisponibleEtCout
                              .PuissanceDisponibleDuPalierThermique[pdtHebdo];

                double* adresseDuResultat = &(problemeHebdo->ResultatsHoraires[pays]
                                                .ProductionThermique[pdtHebdo]
                                                .ProductionThermiqueDuPalier[index]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
            }

            int var = variableManager.HydProd(pays, pdtJour);
            problemeHebdo->ResultatsHoraires[pays].TurbinageHoraire[pdtHebdo] = 0.0;
            if (var >= 0)
            {
                Xmin[var] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                              .MingenHoraire[pdtHebdo];
                Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                              .ContrainteDePmaxHydrauliqueHoraire[pdtHebdo];
                double* adresseDuResultat = &(
                  problemeHebdo->ResultatsHoraires[pays].TurbinageHoraire[pdtHebdo]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
            }

            if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
            {
                if (problemeHebdo->TypeDeLissageHydraulique
                    == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
                {
                    var = variableManager.HydProdDown(pays, pdtJour);
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                    {
                        Xmin[var] = 0.0;
                        Xmax[var] = LINFINI_ANTARES;
                        AdresseOuPlacerLaValeurDesCoutsReduits[var] = nullptr;
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = nullptr;
                    }
                    var = variableManager.HydProdUp(pays, pdtJour);
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                    {
                        Xmin[var] = 0.0;
                        Xmax[var] = LINFINI_ANTARES;
                        AdresseOuPlacerLaValeurDesCoutsReduits[var] = nullptr;
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = nullptr;
                    }
                }
                else if (problemeHebdo->TypeDeLissageHydraulique
                           == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX
                         && pdtJour == 0)
                {
                    var = variableManager.HydProdDown(pays, pdtJour);
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                    {
                        Xmin[var] = 0.0;
                        Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                      .MaxDesPmaxHydrauliques;
                        AdresseOuPlacerLaValeurDesCoutsReduits[var] = nullptr;
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = nullptr;
                    }

                    var = variableManager.HydProdUp(pays, pdtJour);
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                    {
                        Xmin[var] = 0.0;
                        Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                      .MaxDesPmaxHydrauliques;
                        AdresseOuPlacerLaValeurDesCoutsReduits[var] = nullptr;
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = nullptr;
                    }
                }
            }

            var = variableManager.Pumping(pays, pdtJour);
            problemeHebdo->ResultatsHoraires[pays].PompageHoraire[pdtHebdo] = 0.0;
            if (var >= 0)
            {
                Xmin[var] = 0.0;
                Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                              .ContrainteDePmaxPompageHoraire[pdtHebdo];
                double* adresseDuResultat = &(
                  problemeHebdo->ResultatsHoraires[pays].PompageHoraire[pdtHebdo]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
            }

            var = variableManager.Overflow(pays, pdtJour);
            if (var >= 0)
            {
                Xmin[var] = 0.0;
                Xmax[var] = std::max(
                  0.,
                  problemeHebdo->CaracteristiquesHydrauliques[pays].ApportNaturelHoraire[pdtHebdo]);
                AdresseOuPlacerLaValeurDesCoutsReduits[var] = nullptr;
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = &problemeHebdo
                                                                        ->ResultatsHoraires[pays]
                                                                        .debordementsHoraires
                                                                          [pdtHebdo];
            }

            var = variableManager.HydroLevel(pays, pdtJour);
            if (var >= 0)
            {
                Xmin[var] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                              .NiveauHoraireInf[pdtHebdo];
                Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                              .NiveauHoraireSup[pdtHebdo];
                double* adresseDuResultat = &(
                  problemeHebdo->ResultatsHoraires[pays].niveauxHoraires[pdtHebdo]);
                AdresseOuPlacerLaValeurDesCoutsReduits[var] = nullptr;
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
            }

            {
                var = variableManager.Spillage(pays, pdtJour);

                Xmin[var] = 0.0;
                Xmax[var] = LINFINI_ANTARES;

                problemeHebdo->ResultatsHoraires[pays]
                  .ValeursHorairesDeDefaillanceNegative[pdtHebdo]
                  = 0.0;
                double* adresseDuResultat = &(problemeHebdo->ResultatsHoraires[pays]
                                                .ValeursHorairesDeDefaillanceNegative[pdtHebdo]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
            }
        }
    }

    setBoundsForUnsuppliedEnergy(problemeHebdo, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle);

    setBoundsForShortTermStorage(problemeHebdo, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle);

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
        {
            int var = variableManager.FinalStorage(pays);
            if (var >= 0)
            {
                Xmin[var] = -(LINFINI_ANTARES);
                Xmax[var] = LINFINI_ANTARES;

                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = nullptr;
                AdresseOuPlacerLaValeurDesCoutsReduits[var] = nullptr;
            }
            for (uint nblayer = 0; nblayer < 100; nblayer++)
            {
                var = variableManager.LayerStorage(pays, nblayer);
                if (var >= 0)
                {
                    Xmin[var] = 0;
                    Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[pays].TailleReservoir
                                / double(100);

                    AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = nullptr;
                    AdresseOuPlacerLaValeurDesCoutsReduits[var] = nullptr;
                }
            }
        }
    }

    if (problemeHebdo->OptimisationNotFastMode)
    {
        OPT_InitialiserLesBornesDesVariablesDuProblemeLineaireCoutsDeDemarrage(
          problemeHebdo,
          PremierPdtDeLIntervalle,
          DernierPdtDeLIntervalle);
        if (problemeHebdo->allReserves)
        {
            OPT_InitialiserLesBornesDesVariablesDuProblemeLineaireReserves(problemeHebdo,
                                                                           PremierPdtDeLIntervalle,
                                                                           DernierPdtDeLIntervalle);
        }
    }
}
