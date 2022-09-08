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

void OPT_InitialiserLeSecondMembreDuProblemeLineaire(PROBLEME_HEBDO* ProblemeHebdo,
                                                     int PremierPdtDeLIntervalle,
                                                     int DernierPdtDeLIntervalle,
                                                     int NumeroDeLIntervalle)
{
    int Cnt;
    int PdtJour;
    int PdtHebdo;
    int Pays;
    double* AdresseDuResultat;
    int CntCouplante;
    int Interco;
    int Jour;
    int Zone;
    int NombreDeZonesDeReserveJMoins1;
    int* NumeroDeZoneDeReserveJMoins1;
    char ContrainteActivable;
    double TotalReserve;
    int il;
    int ilMax;
    double X;
    int Var;
    int Semaine;
    char YaDeLaReserveJmoins1;
    double a;
    char ContrainteDeReserveJMoins1ParZone;
    char NumeroDOptimisation;
    int NombreDePasDeTempsDUneJournee;
    char* DefaillanceNegativeUtiliserConsoAbattue;
    char* DefaillanceNegativeUtiliserPMinThermique;

    CONSOMMATIONS_ABATTUES* ConsommationsAbattues;
    ALL_MUST_RUN_GENERATION* AllMustRunGeneration;

    CONTRAINTES_COUPLANTES* MatriceDesContraintesCouplantes;
    CORRESPONDANCES_DES_CONTRAINTES* CorrespondanceCntNativesCntOptim;

    int* IndicesDebutDeLigne;
    int* NombreDeTermesDesLignes;
    double* CoefficientsDeLaMatriceDesContraintes;
    int* IndicesColonnes;
    int* TypeDeVariable;
    double* Xmax;
    double* Xmin;
    int type;

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

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    ContrainteDeReserveJMoins1ParZone = ProblemeHebdo->ContrainteDeReserveJMoins1ParZone;

    SecondMembre = ProblemeAResoudre->SecondMembre;
    IndicesDebutDeLigne = ProblemeAResoudre->IndicesDebutDeLigne;
    NombreDeTermesDesLignes = ProblemeAResoudre->NombreDeTermesDesLignes;
    IndicesColonnes = ProblemeAResoudre->IndicesColonnes;
    CoefficientsDeLaMatriceDesContraintes
      = ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes;

    TypeDeVariable = ProblemeAResoudre->TypeDeVariable;
    Xmin = ProblemeAResoudre->Xmin;
    Xmax = ProblemeAResoudre->Xmax;

    NumeroDOptimisation = ProblemeAResoudre->NumeroDOptimisation;
    AdresseOuPlacerLaValeurDesCoutsMarginaux
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux;

    YaDeLaReserveJmoins1 = ProblemeHebdo->YaDeLaReserveJmoins1;
    NumeroDeJourDuPasDeTemps = ProblemeHebdo->NumeroDeJourDuPasDeTemps;
    NombreDePasDeTempsDUneJournee = ProblemeHebdo->NombreDePasDeTempsDUneJournee;
    NumeroDeContrainteEnergieHydraulique = ProblemeHebdo->NumeroDeContrainteEnergieHydraulique;
    NumeroDeContrainteMinEnergieHydraulique
      = ProblemeHebdo->NumeroDeContrainteMinEnergieHydraulique;
    NumeroDeContrainteMaxEnergieHydraulique
      = ProblemeHebdo->NumeroDeContrainteMaxEnergieHydraulique;
    NumeroDeContrainteMaxPompage = ProblemeHebdo->NumeroDeContrainteMaxPompage;
    NombreDePasDeTempsDUneJournee = ProblemeHebdo->NombreDePasDeTempsDUneJournee;
    NombreDeZonesDeReserveJMoins1 = ProblemeHebdo->NombreDeZonesDeReserveJMoins1;
    NumeroDeZoneDeReserveJMoins1 = ProblemeHebdo->NumeroDeZoneDeReserveJMoins1;

    DefaillanceNegativeUtiliserConsoAbattue
      = ProblemeHebdo->DefaillanceNegativeUtiliserConsoAbattue;
    DefaillanceNegativeUtiliserPMinThermique
      = ProblemeHebdo->DefaillanceNegativeUtiliserPMinThermique;

    for (Cnt = 0; Cnt < ProblemeAResoudre->NombreDeContraintes; Cnt++)
    {
        AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = NULL;

        SecondMembre[Cnt] = 0.0;
    }

    for (PdtJour = 0, PdtHebdo = PremierPdtDeLIntervalle; PdtHebdo < DernierPdtDeLIntervalle;
         PdtHebdo++, PdtJour++)
    {
        CorrespondanceCntNativesCntOptim = ProblemeHebdo->CorrespondanceCntNativesCntOptim[PdtJour];

        ConsommationsAbattues = ProblemeHebdo->ConsommationsAbattues[PdtHebdo];
        AllMustRunGeneration = ProblemeHebdo->AllMustRunGeneration[PdtHebdo];
        for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
        {
            Cnt = CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesBilansPays[Pays];
            SecondMembre[Cnt] = -ConsommationsAbattues->ConsommationAbattueDuPays[Pays];

            bool reserveJm1 = (ProblemeHebdo->YaDeLaReserveJmoins1 == OUI_ANTARES);
            bool opt1 = (ProblemeAResoudre->NumeroDOptimisation == PREMIERE_OPTIMISATION);
            bool exp = (ProblemeHebdo->Expansion == OUI_ANTARES);
            bool opt2 = (ProblemeAResoudre->NumeroDOptimisation == DEUXIEME_OPTIMISATION);
            if ((reserveJm1 && opt1) || (reserveJm1 && exp && opt2))
            {
                SecondMembre[Cnt]
                  -= ProblemeHebdo->ReserveJMoins1[Pays]->ReserveHoraireJMoins1[PdtHebdo];
            }

            AdresseDuResultat
              = &(ProblemeHebdo->ResultatsHoraires[Pays]->CoutsMarginauxHoraires[PdtHebdo]);
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
                SecondMembre[Cnt] -= OPT_SommeDesPminThermiques(ProblemeHebdo, Pays, PdtHebdo);
            }

            AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = NULL;
        }

        for (Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
        {
            CoutDeTransport = ProblemeHebdo->CoutDeTransport[Interco];
            if (CoutDeTransport->IntercoGereeAvecDesCouts == OUI_ANTARES)
            {
                Cnt = CorrespondanceCntNativesCntOptim
                        ->NumeroDeContrainteDeDissociationDeFlux[Interco];
                if (CoutDeTransport->IntercoGereeAvecLoopFlow == OUI_ANTARES)
                    SecondMembre[Cnt] = ProblemeHebdo->ValeursDeNTC[PdtHebdo]
                                          ->ValeurDeLoopFlowOrigineVersExtremite[Interco];
                else
                    SecondMembre[Cnt] = 0.;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = NULL;
            }
        }

        for (CntCouplante = 0; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes;
             CntCouplante++)
        {
            MatriceDesContraintesCouplantes
              = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
            if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante == CONTRAINTE_HORAIRE)
            {
                Cnt = CorrespondanceCntNativesCntOptim
                        ->NumeroDeContrainteDesContraintesCouplantes[CntCouplante];
                if (Cnt >= 0)
                {
                    SecondMembre[Cnt] = MatriceDesContraintesCouplantes
                                          ->SecondMembreDeLaContrainteCouplante[PdtHebdo];
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = ProblemeHebdo->ResultatsContraintesCouplantes[CntCouplante].variablesDuales + PdtHebdo;
                }
            }
        }

        if (ContrainteDeReserveJMoins1ParZone == OUI_ANTARES)
        {
            for (Zone = 0; Zone < NombreDeZonesDeReserveJMoins1; Zone++)
            {
                Cnt = CorrespondanceCntNativesCntOptim
                        ->NumeroPremiereContrainteDeReserveParZone[Zone];
                if (Cnt >= 0)
                {
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = NULL;
                    SecondMembre[Cnt] = -100.0;

                    if (NumeroDOptimisation == PREMIERE_OPTIMISATION)
                    {
                        ContrainteActivable = NON_ANTARES;
                        if (YaDeLaReserveJmoins1 == OUI_ANTARES)
                            ContrainteActivable = OUI_ANTARES;
                        if (ContrainteActivable == OUI_ANTARES)
                        {
                            TotalReserve = 0.0;
                            for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
                            {
                                if (NumeroDeZoneDeReserveJMoins1[Pays] == Zone)
                                {
                                    TotalReserve += ProblemeHebdo->ReserveJMoins1[Pays]
                                                      ->ReserveHoraireJMoins1[PdtHebdo];
                                }
                            }
                            SecondMembre[Cnt] = TotalReserve;
                        }
                    }
                }

                Cnt = CorrespondanceCntNativesCntOptim
                        ->NumeroDeuxiemeContrainteDeReserveParZone[Zone];
                if (Cnt >= 0)
                {
                    AdresseDuResultat
                      = &(ProblemeHebdo->CoutsMarginauxDesContraintesDeReserveParZone[Zone]
                            ->CoutsMarginauxHorairesDeLaReserveParZone[PdtHebdo]);
                    *AdresseDuResultat = 0.0;
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = AdresseDuResultat;

                    ContrainteActivable = NON_ANTARES;
                    if (NumeroDOptimisation != PREMIERE_OPTIMISATION)
                    {
                        if (YaDeLaReserveJmoins1 == OUI_ANTARES)
                            ContrainteActivable = OUI_ANTARES;
                    }
                    if (ContrainteActivable == NON_ANTARES)
                    {
                        il = IndicesDebutDeLigne[Cnt];
                        ilMax = il + NombreDeTermesDesLignes[Cnt];
                        X = 0.0;
                        while (il < ilMax)
                        {
                            Var = IndicesColonnes[il];
                            a = CoefficientsDeLaMatriceDesContraintes[il];
                            type = TypeDeVariable[Var];
                            if (a < 0)
                            {
                                if (type == VARIABLE_BORNEE_DES_DEUX_COTES
                                    || type == VARIABLE_BORNEE_SUPERIEUREMENT)
                                    X += a * Xmax[Var];
                                else
                                {
                                    logs.info();
                                    logs.error()
                                      << "Fatal error when computing second member of the reserve "
                                         "constraints :\n"
                                      << "  somme variables are unbounded on one or both sides\n"
                                      << "  results will be useless, please call for maintenance";
                                    logs.info();
                                }
                            }
                            else
                            {
                                if (type == VARIABLE_BORNEE_DES_DEUX_COTES
                                    || type == VARIABLE_BORNEE_INFERIEUREMENT)
                                    X += a * Xmin[Var];
                                else
                                {
                                    logs.info();
                                    logs.error()
                                      << "Fatal error when computing second member of the reserve "
                                         "constraints :\n"
                                      << "  somme variables are unbounded on one or both sides\n"
                                      << "  results will be useless, please call for maintenance";
                                    logs.info();
                                }
                            }
                            il++;
                        }
                        SecondMembre[Cnt] = X - 100.0;
                    }
                    else
                    {
                        TotalReserve = 0.0;
                        for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
                        {
                            if (NumeroDeZoneDeReserveJMoins1[Pays] == Zone)
                            {
                                TotalReserve += ProblemeHebdo->ReserveJMoins1[Pays]
                                                  ->ReserveHoraireJMoins1[PdtHebdo];
                            }
                        }
                        SecondMembre[Cnt]
                          = ProblemeHebdo->ReserveJMoins1[Pays]->ReserveHoraireJMoins1[PdtHebdo];

                        il = IndicesDebutDeLigne[Cnt];
                        ilMax = il + NombreDeTermesDesLignes[Cnt];
                        X = 0.0;
                        while (il < ilMax)
                        {
                            Var = IndicesColonnes[il];

                            if (TypeDeVariable[Var] == VARIABLE_BORNEE_DES_DEUX_COTES)
                                X += Xmax[Var];
                            il++;
                        }
                        SecondMembre[Cnt] -= X;
                    }
                }
            }
        }
    }

    for (PdtHebdo = PremierPdtDeLIntervalle; PdtHebdo < DernierPdtDeLIntervalle;)
    {
        Jour = NumeroDeJourDuPasDeTemps[PdtHebdo];
        if (ProblemeHebdo->OptimisationAuPasHebdomadaire == NON_ANTARES)
        {
            CorrespondanceCntNativesCntOptimJournalieres
              = ProblemeHebdo->CorrespondanceCntNativesCntOptimJournalieres[0];
        }
        else
        {
            CorrespondanceCntNativesCntOptimJournalieres
              = ProblemeHebdo->CorrespondanceCntNativesCntOptimJournalieres[Jour];
        }
        for (CntCouplante = 0; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes;
             CntCouplante++)
        {
            MatriceDesContraintesCouplantes
              = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
            if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante
                == CONTRAINTE_JOURNALIERE)
            {
                Cnt = CorrespondanceCntNativesCntOptimJournalieres
                        ->NumeroDeContrainteDesContraintesCouplantes[CntCouplante];
                if (Cnt >= 0)
                {
                    SecondMembre[Cnt]
                      = MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplante[Jour];
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = ProblemeHebdo->ResultatsContraintesCouplantes[CntCouplante].variablesDuales + Jour;
                }
            }
        }
        PdtHebdo += NombreDePasDeTempsDUneJournee;
    }

    if (ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation
        > ProblemeHebdo->NombreDePasDeTempsDUneJournee)
    {
        Semaine = 0;
        CorrespondanceCntNativesCntOptimHebdomadaires
          = ProblemeHebdo->CorrespondanceCntNativesCntOptimHebdomadaires[Semaine];
        for (CntCouplante = 0; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes;
             CntCouplante++)
        {
            MatriceDesContraintesCouplantes
              = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
            if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante
                == CONTRAINTE_HEBDOMADAIRE)
            {
                Cnt = CorrespondanceCntNativesCntOptimHebdomadaires
                        ->NumeroDeContrainteDesContraintesCouplantes[CntCouplante];
                if (Cnt >= 0)
                {
                    SecondMembre[Cnt] = MatriceDesContraintesCouplantes
                                          ->SecondMembreDeLaContrainteCouplante[Semaine];
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = ProblemeHebdo->ResultatsContraintesCouplantes[CntCouplante].variablesDuales + Semaine;
                }
            }
        }
    }

    for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
    {
        Cnt = NumeroDeContrainteEnergieHydraulique[Pays];
        if (Cnt >= 0)
        {
            SecondMembre[Cnt] = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]
                                  ->CntEnergieH2OParIntervalleOptimise[NumeroDeLIntervalle];
            AdresseDuResultat = NULL;
            AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = AdresseDuResultat;
        }
    }

    for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
    {
        char presenceHydro
          = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable;
        char TurbEntreBornes
          = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->TurbinageEntreBornes;
        if (presenceHydro == OUI_ANTARES
            && (TurbEntreBornes == OUI_ANTARES
                || ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDePompageModulable
                     == OUI_ANTARES))
        {
            Cnt = NumeroDeContrainteMinEnergieHydraulique[Pays];
            if (Cnt >= 0)
            {
                SecondMembre[Cnt] = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]
                                      ->MinEnergieHydrauParIntervalleOptimise[NumeroDeLIntervalle];
                AdresseDuResultat = NULL;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = AdresseDuResultat;
            }
        }
    }

    for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
    {
        char presenceHydro
          = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable;
        char TurbEntreBornes
          = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->TurbinageEntreBornes;
        if (presenceHydro == OUI_ANTARES
            && (TurbEntreBornes == OUI_ANTARES
                || ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDePompageModulable
                     == OUI_ANTARES))
        {
            Cnt = NumeroDeContrainteMaxEnergieHydraulique[Pays];
            if (Cnt >= 0)
            {
                SecondMembre[Cnt] = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]
                                      ->MaxEnergieHydrauParIntervalleOptimise[NumeroDeLIntervalle];
                AdresseDuResultat = NULL;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = AdresseDuResultat;
            }
        }
    }

    for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
    {
        if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDePompageModulable
            == OUI_ANTARES)
        {
            Cnt = NumeroDeContrainteMaxPompage[Pays];
            if (Cnt >= 0)
            {
                SecondMembre[Cnt] = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]
                                      ->MaxEnergiePompageParIntervalleOptimise[NumeroDeLIntervalle];
                AdresseDuResultat = NULL;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = AdresseDuResultat;
            }
        }
    }

    for (PdtJour = 0, PdtHebdo = PremierPdtDeLIntervalle; PdtHebdo < DernierPdtDeLIntervalle;
         PdtHebdo++, PdtJour++)
    {
        CorrespondanceCntNativesCntOptim = ProblemeHebdo->CorrespondanceCntNativesCntOptim[PdtJour];

        for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
        {
            if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->SuiviNiveauHoraire
                == OUI_ANTARES)
            {
                Cnt = CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesNiveauxPays[Pays];
                if (Cnt >= 0)
                {
                    SecondMembre[Cnt] = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]
                                          ->ApportNaturelHoraire[PdtHebdo];
                    if (PdtHebdo == 0)
                    {
                        SecondMembre[Cnt] += ProblemeHebdo->CaracteristiquesHydrauliques[Pays]
                                               ->NiveauInitialReservoir;
                    }
                    AdresseDuResultat = NULL;
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = AdresseDuResultat;
                }
            }
        }
    }

    for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
    {
        if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->AccurateWaterValue == OUI_ANTARES
            && ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->DirectLevelAccess == OUI_ANTARES)
        {
            Cnt = ProblemeHebdo->NumeroDeContrainteEquivalenceStockFinal[Pays];
            if (Cnt >= 0)
            {
                SecondMembre[Cnt] = 0;

                AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = NULL;
            }
        }
        if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->AccurateWaterValue == OUI_ANTARES)
        {
            Cnt = ProblemeHebdo->NumeroDeContrainteExpressionStockFinal[Pays];
            if (Cnt >= 0)
            {
                SecondMembre[Cnt] = 0;

                AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = NULL;
            }
        }
    }

    if (ProblemeHebdo->OptimisationAvecCoutsDeDemarrage == OUI_ANTARES)
    {
        OPT_InitialiserLeSecondMembreDuProblemeLineaireCoutsDeDemarrage(
          ProblemeHebdo, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle);
    }

    return;
}
