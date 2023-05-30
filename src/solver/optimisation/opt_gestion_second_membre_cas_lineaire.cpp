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

#include "spx_constantes_externes.h"
#include "../simulation/sim_structure_probleme_adequation.h"

#include <antares/logs.h>
#include <antares/study.h>
#include <antares/emergency.h>
using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;

static void shortTermStorageLevelsRHS(
    const std::vector<::ShortTermStorage::AREA_INPUT>& shortTermStorageInput,
    int numberOfAreas,
    double* SecondMembre,
    const CORRESPONDANCES_DES_CONTRAINTES* CorrespondanceCntNativesCntOptim,
    int hourInTheYear)
{
    for (int areaIndex = 0; areaIndex < numberOfAreas; areaIndex++)
    {
        for (auto& storage : shortTermStorageInput[areaIndex])
        {
            const int clusterGlobalIndex = storage.clusterGlobalIndex;
            const int cnt
              = CorrespondanceCntNativesCntOptim->ShortTermStorageLevelConstraint[clusterGlobalIndex];
            SecondMembre[cnt] = storage.series->inflows[hourInTheYear];
        }
    }
}

void OPT_InitialiserLeSecondMembreDuProblemeLineaire(PROBLEME_HEBDO* problemeHebdo,
                                                     int PremierPdtDeLIntervalle,
                                                     int DernierPdtDeLIntervalle,
                                                     int NumeroDeLIntervalle,
                                                     const int optimizationNumber)
{
    int weekFirstHour = problemeHebdo->weekInTheYear * 168;

    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    double* SecondMembre = ProblemeAResoudre->SecondMembre;

    double** AdresseOuPlacerLaValeurDesCoutsMarginaux
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux;

    int NombreDePasDeTempsDUneJournee = problemeHebdo->NombreDePasDeTempsDUneJournee;

    const std::vector<int>& NumeroDeJourDuPasDeTemps = problemeHebdo->NumeroDeJourDuPasDeTemps;
    const std::vector<int>& NumeroDeContrainteEnergieHydraulique
      = problemeHebdo->NumeroDeContrainteEnergieHydraulique;
    const std::vector<int>& NumeroDeContrainteMinEnergieHydraulique
      = problemeHebdo->NumeroDeContrainteMinEnergieHydraulique;
    const std::vector<int>& NumeroDeContrainteMaxEnergieHydraulique
      = problemeHebdo->NumeroDeContrainteMaxEnergieHydraulique;
    const std::vector<int>& NumeroDeContrainteMaxPompage
      = problemeHebdo->NumeroDeContrainteMaxPompage;

    const std::vector<bool>& DefaillanceNegativeUtiliserConsoAbattue
      = problemeHebdo->DefaillanceNegativeUtiliserConsoAbattue;
    const std::vector<bool>& DefaillanceNegativeUtiliserPMinThermique
      = problemeHebdo->DefaillanceNegativeUtiliserPMinThermique;

    for (int i = 0; i < ProblemeAResoudre->NombreDeContraintes; i++)
    {
        AdresseOuPlacerLaValeurDesCoutsMarginaux[i] = nullptr;

        SecondMembre[i] = 0.0;
    }

    for (int pdtJour = 0, pdtHebdo = PremierPdtDeLIntervalle; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        const CORRESPONDANCES_DES_CONTRAINTES* CorrespondanceCntNativesCntOptim
          = problemeHebdo->CorrespondanceCntNativesCntOptim[pdtJour];

        const CONSOMMATIONS_ABATTUES* ConsommationsAbattues
          = problemeHebdo->ConsommationsAbattues[pdtHebdo];
        const ALL_MUST_RUN_GENERATION* AllMustRunGeneration
          = problemeHebdo->AllMustRunGeneration[pdtHebdo];
        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            int cnt = CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesBilansPays[pays];
            SecondMembre[cnt] = -ConsommationsAbattues->ConsommationAbattueDuPays[pays];

            bool reserveJm1 = (problemeHebdo->YaDeLaReserveJmoins1);
            bool opt1 = (optimizationNumber == PREMIERE_OPTIMISATION);
            if (reserveJm1 && opt1)
            {
                SecondMembre[cnt]
                  -= problemeHebdo->ReserveJMoins1[pays]->ReserveHoraireJMoins1[pdtHebdo];
            }

            double* adresseDuResultat
              = &(problemeHebdo->ResultatsHoraires[pays].CoutsMarginauxHoraires[pdtHebdo]);
            AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = adresseDuResultat;

            cnt = CorrespondanceCntNativesCntOptim
                    ->NumeroDeContraintePourEviterLesChargesFictives[pays];
            SecondMembre[cnt] = 0.0;

            double MaxAllMustRunGeneration = 0.0;
            if (AllMustRunGeneration->AllMustRunGenerationOfArea[pays] > 0.0)
                MaxAllMustRunGeneration = AllMustRunGeneration->AllMustRunGenerationOfArea[pays];

            double MaxMoinsConsommationBrute = 0.0;
            if (-(ConsommationsAbattues->ConsommationAbattueDuPays[pays]
                  + AllMustRunGeneration->AllMustRunGenerationOfArea[pays])
                > 0.0)
                MaxMoinsConsommationBrute
                  = -(ConsommationsAbattues->ConsommationAbattueDuPays[pays]
                      + AllMustRunGeneration->AllMustRunGenerationOfArea[pays]);

            SecondMembre[cnt] = DefaillanceNegativeUtiliserConsoAbattue[pays]
                                * (MaxAllMustRunGeneration + MaxMoinsConsommationBrute);

            if (DefaillanceNegativeUtiliserPMinThermique[pays] == 0)
            {
                SecondMembre[cnt] -= OPT_SommeDesPminThermiques(problemeHebdo, pays, pdtHebdo);
            }

            AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
        }

        int hourInTheYear = weekFirstHour + pdtHebdo;
        shortTermStorageLevelsRHS(problemeHebdo->ShortTermStorage,
                                  problemeHebdo->NombreDePays,
                                  ProblemeAResoudre->SecondMembre,
                                  CorrespondanceCntNativesCntOptim,
                                  hourInTheYear);

        for (int interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
        {
            const COUTS_DE_TRANSPORT* CoutDeTransport = problemeHebdo->CoutDeTransport[interco];
            if (CoutDeTransport->IntercoGereeAvecDesCouts)
            {
                int cnt = CorrespondanceCntNativesCntOptim
                            ->NumeroDeContrainteDeDissociationDeFlux[interco];
                if (CoutDeTransport->IntercoGereeAvecLoopFlow)
                    SecondMembre[cnt] = problemeHebdo->ValeursDeNTC[pdtHebdo]
                                          ->ValeurDeLoopFlowOrigineVersExtremite[interco];
                else
                    SecondMembre[cnt] = 0.;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }

        for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            const CONTRAINTES_COUPLANTES* MatriceDesContraintesCouplantes
              = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];
            if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante != CONTRAINTE_HORAIRE)
                continue;

            int cnt = CorrespondanceCntNativesCntOptim
                        ->NumeroDeContrainteDesContraintesCouplantes[cntCouplante];
            if (cnt >= 0)
            {
                SecondMembre[cnt]
                  = MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplante[pdtHebdo];
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt]
                  = problemeHebdo->ResultatsContraintesCouplantes[cntCouplante].variablesDuales
                    + pdtHebdo;
            }
        }
    }

    for (int pdtHebdo = PremierPdtDeLIntervalle; pdtHebdo < DernierPdtDeLIntervalle;)
    {
        CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES* CorrespondanceCntNativesCntOptimJournalieres;

        int jour = NumeroDeJourDuPasDeTemps[pdtHebdo];
        if (!problemeHebdo->OptimisationAuPasHebdomadaire)
        {
            CorrespondanceCntNativesCntOptimJournalieres
              = problemeHebdo->CorrespondanceCntNativesCntOptimJournalieres[0];
        }
        else
        {
            CorrespondanceCntNativesCntOptimJournalieres
              = problemeHebdo->CorrespondanceCntNativesCntOptimJournalieres[jour];
        }
        for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            const CONTRAINTES_COUPLANTES* MatriceDesContraintesCouplantes
              = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];
            if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante
                == CONTRAINTE_JOURNALIERE)
            {
                int cnt = CorrespondanceCntNativesCntOptimJournalieres
                            ->NumeroDeContrainteDesContraintesCouplantes[cntCouplante];
                if (cnt >= 0)
                {
                    SecondMembre[cnt]
                      = MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplante[jour];
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt]
                      = problemeHebdo->ResultatsContraintesCouplantes[cntCouplante].variablesDuales
                        + jour;
                }
            }
        }
        pdtHebdo += NombreDePasDeTempsDUneJournee;
    }

    if (problemeHebdo->NombreDePasDeTempsPourUneOptimisation
        > problemeHebdo->NombreDePasDeTempsDUneJournee)
    {
        int semaine = 0;
        const CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES*
          CorrespondanceCntNativesCntOptimHebdomadaires
          = problemeHebdo->CorrespondanceCntNativesCntOptimHebdomadaires[semaine];

        for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            const CONTRAINTES_COUPLANTES* MatriceDesContraintesCouplantes
              = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];

            if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante
                != CONTRAINTE_HEBDOMADAIRE)
                continue;

            int cnt = CorrespondanceCntNativesCntOptimHebdomadaires
                        ->NumeroDeContrainteDesContraintesCouplantes[cntCouplante];
            if (cnt >= 0)
            {
                SecondMembre[cnt]
                  = MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplante[semaine];
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt]
                  = problemeHebdo->ResultatsContraintesCouplantes[cntCouplante].variablesDuales
                    + semaine;
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        int cnt = NumeroDeContrainteEnergieHydraulique[pays];
        if (cnt >= 0)
        {
            SecondMembre[cnt] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                  ->CntEnergieH2OParIntervalleOptimise[NumeroDeLIntervalle];
            AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        bool presenceHydro
          = problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDHydrauliqueModulable;
        bool TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[pays]->TurbinageEntreBornes;
        if (presenceHydro && (TurbEntreBornes
                || problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDePompageModulable))
        {
            int cnt = NumeroDeContrainteMinEnergieHydraulique[pays];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                      ->MinEnergieHydrauParIntervalleOptimise[NumeroDeLIntervalle];
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        bool presenceHydro
          = problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDHydrauliqueModulable;
        bool TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[pays]->TurbinageEntreBornes;
        if (presenceHydro && (TurbEntreBornes
                || problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDePompageModulable))
        {
            int cnt = NumeroDeContrainteMaxEnergieHydraulique[pays];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                      ->MaxEnergieHydrauParIntervalleOptimise[NumeroDeLIntervalle];
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDePompageModulable)
        {
            int cnt = NumeroDeContrainteMaxPompage[pays];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                      ->MaxEnergiePompageParIntervalleOptimise[NumeroDeLIntervalle];
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }
    }

    for (int pdtJour = 0, pdtHebdo = PremierPdtDeLIntervalle; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        const CORRESPONDANCES_DES_CONTRAINTES* CorrespondanceCntNativesCntOptim
          = problemeHebdo->CorrespondanceCntNativesCntOptim[pdtJour];

        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            if (!problemeHebdo->CaracteristiquesHydrauliques[pays]->SuiviNiveauHoraire)
                continue;

            int cnt = CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesNiveauxPays[pays];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                      ->ApportNaturelHoraire[pdtHebdo];
                if (pdtHebdo == 0)
                {
                    SecondMembre[cnt]
                      += problemeHebdo->CaracteristiquesHydrauliques[pays]->NiveauInitialReservoir;
                }
                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays]->AccurateWaterValue
            && problemeHebdo->CaracteristiquesHydrauliques[pays]->DirectLevelAccess)
        {
            int cnt = problemeHebdo->NumeroDeContrainteEquivalenceStockFinal[pays];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = 0;

                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }
        if (problemeHebdo->CaracteristiquesHydrauliques[pays]->AccurateWaterValue)
        {
            int cnt = problemeHebdo->NumeroDeContrainteExpressionStockFinal[pays];
            if (cnt >= 0)
            {
                SecondMembre[cnt] = 0;

                AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
            }
        }
    }

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    {
        OPT_InitialiserLeSecondMembreDuProblemeLineaireCoutsDeDemarrage(
          problemeHebdo, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle);
    }

    return;
}
