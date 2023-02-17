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

#include "spx_constantes_externes.h"
#include "../simulation/sim_structure_probleme_adequation.h"

#include <antares/logs.h>
#include <antares/study.h>
#include <antares/emergency.h>
using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;

void OPT_InitialiserLeSecondMembreDuProblemeLineaire(PROBLEME_HEBDO* problemeHebdo,
                                                     int PremierPdtDeLIntervalle,
                                                     int DernierPdtDeLIntervalle,
                                                     int NumeroDeLIntervalle,
                                                     const int optimizationNumber)
{
    int Cnt;
    int PdtJour;
    int PdtHebdo;
    int Pays;
    double* AdresseDuResultat;
    int CntCouplante;
    int Interco;
    int Jour;
    int Semaine;
    int NombreDePasDeTempsDUneJournee;
    char* DefaillanceNegativeUtiliserConsoAbattue;
    char* DefaillanceNegativeUtiliserPMinThermique;

    CONSOMMATIONS_ABATTUES* ConsommationsAbattues;
    ALL_MUST_RUN_GENERATION* AllMustRunGeneration;

    CONTRAINTES_COUPLANTES* MatriceDesContraintesCouplantes;
    CORRESPONDANCES_DES_CONTRAINTES* CorrespondanceCntNativesCntOptim;

    COUTS_DE_TRANSPORT* CoutDeTransport;

    double* SecondMembre;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    double** AdresseOuPlacerLaValeurDesCoutsMarginaux;
    int* NumeroDeJourDuPasDeTemps;
    int* NumeroDeContrainteEnergieHydraulique;
    int* NumeroDeContrainteMaxEnergieHydraulique;
    int* NumeroDeContrainteMinEnergieHydraulique;
    int* NumeroDeContrainteMaxPompage;
    CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES* CorrespondanceCntNativesCntOptimJournalieres;
    CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES* CorrespondanceCntNativesCntOptimHebdomadaires;

    ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    SecondMembre = ProblemeAResoudre->SecondMembre;

    AdresseOuPlacerLaValeurDesCoutsMarginaux
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux;

    NumeroDeJourDuPasDeTemps = problemeHebdo->NumeroDeJourDuPasDeTemps;
    NombreDePasDeTempsDUneJournee = problemeHebdo->NombreDePasDeTempsDUneJournee;
    NumeroDeContrainteEnergieHydraulique = problemeHebdo->NumeroDeContrainteEnergieHydraulique;
    NumeroDeContrainteMinEnergieHydraulique
      = problemeHebdo->NumeroDeContrainteMinEnergieHydraulique;
    NumeroDeContrainteMaxEnergieHydraulique
      = problemeHebdo->NumeroDeContrainteMaxEnergieHydraulique;
    NumeroDeContrainteMaxPompage = problemeHebdo->NumeroDeContrainteMaxPompage;
    NombreDePasDeTempsDUneJournee = problemeHebdo->NombreDePasDeTempsDUneJournee;

    DefaillanceNegativeUtiliserConsoAbattue
      = problemeHebdo->DefaillanceNegativeUtiliserConsoAbattue;
    DefaillanceNegativeUtiliserPMinThermique
      = problemeHebdo->DefaillanceNegativeUtiliserPMinThermique;

    for (Cnt = 0; Cnt < ProblemeAResoudre->NombreDeContraintes; Cnt++)
    {
        AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = NULL;

        SecondMembre[Cnt] = 0.0;
    }

    for (PdtJour = 0, PdtHebdo = PremierPdtDeLIntervalle; PdtHebdo < DernierPdtDeLIntervalle;
         PdtHebdo++, PdtJour++)
    {
        CorrespondanceCntNativesCntOptim = problemeHebdo->CorrespondanceCntNativesCntOptim[PdtJour];

        ConsommationsAbattues = problemeHebdo->ConsommationsAbattues[PdtHebdo];
        AllMustRunGeneration = problemeHebdo->AllMustRunGeneration[PdtHebdo];
        for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
        {
            Cnt = CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesBilansPays[Pays];
            SecondMembre[Cnt] = -ConsommationsAbattues->ConsommationAbattueDuPays[Pays];

            bool reserveJm1 = (problemeHebdo->YaDeLaReserveJmoins1 == OUI_ANTARES);
            bool opt1 = (optimizationNumber == PREMIERE_OPTIMISATION);
            if (reserveJm1 && opt1)
            {
                SecondMembre[Cnt]
                  -= problemeHebdo->ReserveJMoins1[Pays]->ReserveHoraireJMoins1[PdtHebdo];
            }

            AdresseDuResultat
              = &(problemeHebdo->ResultatsHoraires[Pays]->CoutsMarginauxHoraires[PdtHebdo]);
            AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = AdresseDuResultat;

            Cnt = CorrespondanceCntNativesCntOptim
                    ->NumeroDeContraintePourEviterLesChargesFictives[Pays];
            SecondMembre[Cnt] = 0.0;

            double MaxAllMustRunGeneration = 0.0;
            if (AllMustRunGeneration->AllMustRunGenerationOfArea[Pays] > 0.0)
                MaxAllMustRunGeneration = AllMustRunGeneration->AllMustRunGenerationOfArea[Pays];

            double MaxMoinsConsommationBrute = 0.0;
            if (-(ConsommationsAbattues->ConsommationAbattueDuPays[Pays]
                  + AllMustRunGeneration->AllMustRunGenerationOfArea[Pays])
                > 0.0)
                MaxMoinsConsommationBrute
                  = -(ConsommationsAbattues->ConsommationAbattueDuPays[Pays]
                      + AllMustRunGeneration->AllMustRunGenerationOfArea[Pays]);

            SecondMembre[Cnt] = DefaillanceNegativeUtiliserConsoAbattue[Pays]
                                * (MaxAllMustRunGeneration + MaxMoinsConsommationBrute);

            if (DefaillanceNegativeUtiliserPMinThermique[Pays] == 0)
            {
                SecondMembre[Cnt] -= OPT_SommeDesPminThermiques(problemeHebdo, Pays, PdtHebdo);
            }

            AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = NULL;
        }

        for (Interco = 0; Interco < problemeHebdo->NombreDInterconnexions; Interco++)
        {
            CoutDeTransport = problemeHebdo->CoutDeTransport[Interco];
            if (CoutDeTransport->IntercoGereeAvecDesCouts == OUI_ANTARES)
            {
                Cnt = CorrespondanceCntNativesCntOptim
                        ->NumeroDeContrainteDeDissociationDeFlux[Interco];
                if (CoutDeTransport->IntercoGereeAvecLoopFlow == OUI_ANTARES)
                    SecondMembre[Cnt] = problemeHebdo->ValeursDeNTC[PdtHebdo]
                                          ->ValeurDeLoopFlowOrigineVersExtremite[Interco];
                else
                    SecondMembre[Cnt] = 0.;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = NULL;
            }
        }

        for (CntCouplante = 0; CntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             CntCouplante++)
        {
            MatriceDesContraintesCouplantes
              = problemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
            if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante == CONTRAINTE_HORAIRE)
            {
                Cnt = CorrespondanceCntNativesCntOptim
                        ->NumeroDeContrainteDesContraintesCouplantes[CntCouplante];
                if (Cnt >= 0)
                {
                    SecondMembre[Cnt] = MatriceDesContraintesCouplantes
                                          ->SecondMembreDeLaContrainteCouplante[PdtHebdo];
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt]
                      = problemeHebdo->ResultatsContraintesCouplantes[CntCouplante].variablesDuales
                        + PdtHebdo;
                }
            }
        }
    }

    for (PdtHebdo = PremierPdtDeLIntervalle; PdtHebdo < DernierPdtDeLIntervalle;)
    {
        Jour = NumeroDeJourDuPasDeTemps[PdtHebdo];
        if (problemeHebdo->OptimisationAuPasHebdomadaire == NON_ANTARES)
        {
            CorrespondanceCntNativesCntOptimJournalieres
              = problemeHebdo->CorrespondanceCntNativesCntOptimJournalieres[0];
        }
        else
        {
            CorrespondanceCntNativesCntOptimJournalieres
              = problemeHebdo->CorrespondanceCntNativesCntOptimJournalieres[Jour];
        }
        for (CntCouplante = 0; CntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             CntCouplante++)
        {
            MatriceDesContraintesCouplantes
              = problemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
            if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante
                == CONTRAINTE_JOURNALIERE)
            {
                Cnt = CorrespondanceCntNativesCntOptimJournalieres
                        ->NumeroDeContrainteDesContraintesCouplantes[CntCouplante];
                if (Cnt >= 0)
                {
                    SecondMembre[Cnt]
                      = MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplante[Jour];
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt]
                      = problemeHebdo->ResultatsContraintesCouplantes[CntCouplante].variablesDuales
                        + Jour;
                }
            }
        }
        PdtHebdo += NombreDePasDeTempsDUneJournee;
    }

    if (problemeHebdo->NombreDePasDeTempsPourUneOptimisation
        > problemeHebdo->NombreDePasDeTempsDUneJournee)
    {
        Semaine = 0;
        CorrespondanceCntNativesCntOptimHebdomadaires
          = problemeHebdo->CorrespondanceCntNativesCntOptimHebdomadaires[Semaine];
        for (CntCouplante = 0; CntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             CntCouplante++)
        {
            MatriceDesContraintesCouplantes
              = problemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
            if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante
                == CONTRAINTE_HEBDOMADAIRE)
            {
                Cnt = CorrespondanceCntNativesCntOptimHebdomadaires
                        ->NumeroDeContrainteDesContraintesCouplantes[CntCouplante];
                if (Cnt >= 0)
                {
                    SecondMembre[Cnt] = MatriceDesContraintesCouplantes
                                          ->SecondMembreDeLaContrainteCouplante[Semaine];
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt]
                      = problemeHebdo->ResultatsContraintesCouplantes[CntCouplante].variablesDuales
                        + Semaine;
                }
            }
        }
    }

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        Cnt = NumeroDeContrainteEnergieHydraulique[Pays];
        if (Cnt >= 0)
        {
            SecondMembre[Cnt] = problemeHebdo->CaracteristiquesHydrauliques[Pays]
                                  ->CntEnergieH2OParIntervalleOptimise[NumeroDeLIntervalle];
            AdresseDuResultat = NULL;
            AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = AdresseDuResultat;
        }
    }

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        char presenceHydro
          = problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable;
        char TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[Pays]->TurbinageEntreBornes;
        if (presenceHydro == OUI_ANTARES
            && (TurbEntreBornes == OUI_ANTARES
                || problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDePompageModulable
                     == OUI_ANTARES))
        {
            Cnt = NumeroDeContrainteMinEnergieHydraulique[Pays];
            if (Cnt >= 0)
            {
                SecondMembre[Cnt] = problemeHebdo->CaracteristiquesHydrauliques[Pays]
                                      ->MinEnergieHydrauParIntervalleOptimise[NumeroDeLIntervalle];
                AdresseDuResultat = NULL;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = AdresseDuResultat;
            }
        }
    }

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        char presenceHydro
          = problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable;
        char TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[Pays]->TurbinageEntreBornes;
        if (presenceHydro == OUI_ANTARES
            && (TurbEntreBornes == OUI_ANTARES
                || problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDePompageModulable
                     == OUI_ANTARES))
        {
            Cnt = NumeroDeContrainteMaxEnergieHydraulique[Pays];
            if (Cnt >= 0)
            {
                SecondMembre[Cnt] = problemeHebdo->CaracteristiquesHydrauliques[Pays]
                                      ->MaxEnergieHydrauParIntervalleOptimise[NumeroDeLIntervalle];
                AdresseDuResultat = NULL;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = AdresseDuResultat;
            }
        }
    }

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDePompageModulable
            == OUI_ANTARES)
        {
            Cnt = NumeroDeContrainteMaxPompage[Pays];
            if (Cnt >= 0)
            {
                SecondMembre[Cnt] = problemeHebdo->CaracteristiquesHydrauliques[Pays]
                                      ->MaxEnergiePompageParIntervalleOptimise[NumeroDeLIntervalle];
                AdresseDuResultat = NULL;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = AdresseDuResultat;
            }
        }
    }

    for (PdtJour = 0, PdtHebdo = PremierPdtDeLIntervalle; PdtHebdo < DernierPdtDeLIntervalle;
         PdtHebdo++, PdtJour++)
    {
        CorrespondanceCntNativesCntOptim = problemeHebdo->CorrespondanceCntNativesCntOptim[PdtJour];

        for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
        {
            if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->SuiviNiveauHoraire
                == OUI_ANTARES)
            {
                Cnt = CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesNiveauxPays[Pays];
                if (Cnt >= 0)
                {
                    SecondMembre[Cnt] = problemeHebdo->CaracteristiquesHydrauliques[Pays]
                                          ->ApportNaturelHoraire[PdtHebdo];
                    if (PdtHebdo == 0)
                    {
                        SecondMembre[Cnt] += problemeHebdo->CaracteristiquesHydrauliques[Pays]
                                               ->NiveauInitialReservoir;
                    }
                    AdresseDuResultat = NULL;
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = AdresseDuResultat;
                }
            }
        }
    }

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->AccurateWaterValue == OUI_ANTARES
            && problemeHebdo->CaracteristiquesHydrauliques[Pays]->DirectLevelAccess == OUI_ANTARES)
        {
            Cnt = problemeHebdo->NumeroDeContrainteEquivalenceStockFinal[Pays];
            if (Cnt >= 0)
            {
                SecondMembre[Cnt] = 0;

                AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = NULL;
            }
        }
        if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->AccurateWaterValue == OUI_ANTARES)
        {
            Cnt = problemeHebdo->NumeroDeContrainteExpressionStockFinal[Pays];
            if (Cnt >= 0)
            {
                SecondMembre[Cnt] = 0;

                AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = NULL;
            }
        }
    }

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage == OUI_ANTARES)
    {
        OPT_InitialiserLeSecondMembreDuProblemeLineaireCoutsDeDemarrage(
          problemeHebdo, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle);
    }

    return;
}
