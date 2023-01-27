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
#include "../simulation/sim_structure_probleme_economique.h"
#include "../simulation/sim_structure_probleme_adequation.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"
#include "adq_patch_local_matching.h"
#include <math.h>
#include <yuni/core/math.h>
#include <limits.h>

#include "spx_constantes_externes.h"

#define EPSILON_DEFAILLANCE 1e-3
#include <antares/logs.h>
#include <antares/study.h>
#include <antares/emergency.h>
using namespace Antares;
using namespace Antares::Data;

using namespace Yuni;

void OPT_MaxDesPmaxHydrauliques(PROBLEME_HEBDO* problemeHebdo)
{
    int Pays;
    int PdtHebdo;
    double PmaxHyd;
    double* ContrainteDePmaxHydrauliqueHoraire;

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        problemeHebdo->CaracteristiquesHydrauliques[Pays]->MaxDesPmaxHydrauliques = 0.0;
        problemeHebdo->CaracteristiquesHydrauliques[Pays]->MaxDesPmaxHydrauliquesRef = 0.0;
        if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable
            != OUI_ANTARES)
            continue;
        ContrainteDePmaxHydrauliqueHoraire
          = problemeHebdo->CaracteristiquesHydrauliques[Pays]->ContrainteDePmaxHydrauliqueHoraire;
        PmaxHyd = -1;
        for (PdtHebdo = 0; PdtHebdo < problemeHebdo->NombreDePasDeTemps; PdtHebdo++)
        {
            if (ContrainteDePmaxHydrauliqueHoraire[PdtHebdo] > PmaxHyd)
                PmaxHyd = ContrainteDePmaxHydrauliqueHoraire[PdtHebdo];
        }

        problemeHebdo->CaracteristiquesHydrauliques[Pays]->MaxDesPmaxHydrauliques = PmaxHyd;
        problemeHebdo->CaracteristiquesHydrauliques[Pays]->MaxDesPmaxHydrauliquesRef = PmaxHyd;
    }

    return;
}

double OPT_SommeDesPminThermiques(PROBLEME_HEBDO* problemeHebdo, int Pays, int PdtHebdo)
{
    int Index;
    double SommeDesPminThermiques;
    PDISP_ET_COUTS_HORAIRES_PAR_PALIER** PuissanceDisponibleEtCout;
    PALIERS_THERMIQUES* PaliersThermiquesDuPays;

    SommeDesPminThermiques = 0.0;
    PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[Pays];
    PuissanceDisponibleEtCout = PaliersThermiquesDuPays->PuissanceDisponibleEtCout;

    for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++)
    {
        SommeDesPminThermiques
          += PuissanceDisponibleEtCout[Index]->PuissanceMinDuPalierThermique[PdtHebdo];
    }

    return (SommeDesPminThermiques);
}

void setBoundsForUnsuppliedEnergy(PROBLEME_HEBDO* problemeHebdo,
                                  const int PremierPdtDeLIntervalle,
                                  const int DernierPdtDeLIntervalle,
                                  const int optimizationNumber)
{
    // OUTPUT
    double* Xmin = problemeHebdo->ProblemeAResoudre->Xmin;
    double* Xmax = problemeHebdo->ProblemeAResoudre->Xmax;
    double** AdresseOuPlacerLaValeurDesVariablesOptimisees
      = problemeHebdo->ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees;

    const bool reserveJm1 = (problemeHebdo->YaDeLaReserveJmoins1 == OUI_ANTARES);
    const bool opt1 = (optimizationNumber == PREMIERE_OPTIMISATION);

    for (int PdtHebdo = PremierPdtDeLIntervalle, PdtJour = 0; PdtHebdo < DernierPdtDeLIntervalle;
         PdtHebdo++, PdtJour++)
    {
        const CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim
          = problemeHebdo->CorrespondanceVarNativesVarOptim[PdtJour];
        const ALL_MUST_RUN_GENERATION* AllMustRunGeneration
          = problemeHebdo->AllMustRunGeneration[PdtHebdo];
        const CONSOMMATIONS_ABATTUES* ConsommationsAbattues
          = problemeHebdo->ConsommationsAbattues[PdtHebdo];

        for (int Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
        {
            double ResidualLoadInArea = ConsommationsAbattues->ConsommationAbattueDuPays[Pays];

            if (reserveJm1 && opt1)
            {
                ResidualLoadInArea
                  += problemeHebdo->ReserveJMoins1[Pays]->ReserveHoraireJMoins1[PdtHebdo];
            }

            int Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[Pays];
            Xmin[Var] = 0.0;

            double MaxAllMustRunGenerationOfArea = 0.;
            if (AllMustRunGeneration->AllMustRunGenerationOfArea[Pays] > 0.)
                MaxAllMustRunGenerationOfArea
                  = AllMustRunGeneration->AllMustRunGenerationOfArea[Pays];

            ResidualLoadInArea += MaxAllMustRunGenerationOfArea;
            if (ResidualLoadInArea >= 0.)
                Xmax[Var] = ResidualLoadInArea + 1e-5;
            else
                Xmax[Var] = 0.;

            // adq patch: update ENS <= DENS in 2nd run
            if (problemeHebdo->adqPatchParams
                && problemeHebdo->adqPatchParams->AdequacyFirstStep == false
                && problemeHebdo->adequacyPatchRuntimeData.areaMode[Pays]
                     == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
                Xmax[Var] = std::min(
                  Xmax[Var], problemeHebdo->ResultatsHoraires[Pays]->ValeursHorairesDENS[PdtHebdo]);

            problemeHebdo->ResultatsHoraires[Pays]->ValeursHorairesDeDefaillancePositive[PdtHebdo]
              = 0.0;

            AdresseOuPlacerLaValeurDesVariablesOptimisees[Var]
              = &(problemeHebdo->ResultatsHoraires[Pays]
                    ->ValeursHorairesDeDefaillancePositive[PdtHebdo]);
        }
    }
}

void OPT_InitialiserLesBornesDesVariablesDuProblemeLineaire(PROBLEME_HEBDO* problemeHebdo,
                                                            const int PremierPdtDeLIntervalle,
                                                            const int DernierPdtDeLIntervalle,
                                                            const int optimizationNumber)
{
    int PdtHebdo;
    int PdtJour;
    int Interco;
    int Pays;
    int Palier;
    int Var;
    int Index;
    double* AdresseDuResultat;
    int maxThermalPlant;
    double** AdresseOuPlacerLaValeurDesVariablesOptimisees;
    double** AdresseOuPlacerLaValeurDesCoutsReduits;
    double* Xmin;
    double* Xmax;
    int* TypeDeVariable;
    
    VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    PALIERS_THERMIQUES* PaliersThermiquesDuPays;
    PDISP_ET_COUTS_HORAIRES_PAR_PALIER* PuissanceDisponibleEtCout;
    COUTS_DE_TRANSPORT* CoutDeTransport;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;

    ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    AdresseOuPlacerLaValeurDesVariablesOptimisees
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees;
    AdresseOuPlacerLaValeurDesCoutsReduits
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits;
    Xmin = ProblemeAResoudre->Xmin;
    Xmax = ProblemeAResoudre->Xmax;
    TypeDeVariable = ProblemeAResoudre->TypeDeVariable;

    for (Var = 0; Var < ProblemeAResoudre->NombreDeVariables; Var++)
    {
        AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = NULL;
        AdresseOuPlacerLaValeurDesCoutsReduits[Var] = NULL;
    }

    for (PdtHebdo = PremierPdtDeLIntervalle, PdtJour = 0; PdtHebdo < DernierPdtDeLIntervalle;
         PdtHebdo++, PdtJour++)
    {
        CorrespondanceVarNativesVarOptim = problemeHebdo->CorrespondanceVarNativesVarOptim[PdtJour];
        ValeursDeNTC = problemeHebdo->ValeursDeNTC[PdtHebdo];

        for (Interco = 0; Interco < problemeHebdo->NombreDInterconnexions; Interco++)
        {
            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
            CoutDeTransport = problemeHebdo->CoutDeTransport[Interco];

            AdequacyPatch::setNTCbounds(Xmax[Var], Xmin[Var], ValeursDeNTC, Interco, problemeHebdo);

            if (Math::Infinite(Xmax[Var]) == 1)
            {
                if (Math::Infinite(Xmin[Var]) == -1)
                    TypeDeVariable[Var] = VARIABLE_NON_BORNEE;
                else
                    TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
            }
            else
            {
                if (Math::Infinite(Xmin[Var]) == -1)
                    TypeDeVariable[Var] = VARIABLE_BORNEE_SUPERIEUREMENT;
                else
                    TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
            }

            AdresseDuResultat = &(problemeHebdo->VariablesDualesDesContraintesDeNTC[PdtHebdo]
                                    ->VariableDualeParInterconnexion[Interco]);
            AdresseOuPlacerLaValeurDesCoutsReduits[Var] = AdresseDuResultat;

            AdresseDuResultat = &(ValeursDeNTC->ValeurDuFlux[Interco]);
            AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = AdresseDuResultat;

            if (CoutDeTransport->IntercoGereeAvecDesCouts == OUI_ANTARES)
            {
                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco];

                if (CoutDeTransport->IntercoGereeAvecLoopFlow == OUI_ANTARES)
                    Xmax[Var] = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco]
                                - ValeursDeNTC->ValeurDeLoopFlowOrigineVersExtremite[Interco];
                else
                    Xmax[Var] = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];

                Xmax[Var] += 0.01;
                TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
                if (Math::Infinite(Xmax[Var]) == 1)
                {
                    TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
                }
                Xmin[Var] = 0.0;
                AdresseOuPlacerLaValeurDesCoutsReduits[Var] = NULL;
                AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = NULL;

                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco];
                if (CoutDeTransport->IntercoGereeAvecLoopFlow == OUI_ANTARES)
                    Xmax[Var] = ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]
                                + ValeursDeNTC->ValeurDeLoopFlowOrigineVersExtremite[Interco];
                else
                    Xmax[Var] = ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco];

                Xmax[Var] += 0.01;
                TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
                if (Math::Infinite(Xmax[Var]) == 1)
                {
                    TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
                }
                Xmin[Var] = 0.0;
                AdresseOuPlacerLaValeurDesCoutsReduits[Var] = NULL;
                AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = NULL;
            }
        }

        for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
        {
            PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[Pays];
            maxThermalPlant = PaliersThermiquesDuPays->NombreDePaliersThermiques;

            for (Index = 0; Index < maxThermalPlant; Index++)
            {
                Palier
                  = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
                Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[Palier];
                PuissanceDisponibleEtCout
                  = PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Index];

                Xmin[Var] = PuissanceDisponibleEtCout->PuissanceMinDuPalierThermique[PdtHebdo];

                Xmax[Var]
                  = PuissanceDisponibleEtCout->PuissanceDisponibleDuPalierThermique[PdtHebdo];

                AdresseDuResultat = &(problemeHebdo->ResultatsHoraires[Pays]
                                        ->ProductionThermique[PdtHebdo]
                                        ->ProductionThermiqueDuPalier[Index]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = AdresseDuResultat;
            }

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
            problemeHebdo->ResultatsHoraires[Pays]->TurbinageHoraire[PdtHebdo] = 0.0;
            if (Var >= 0)
            {
                Xmin[Var] = 0.0;
                Xmax[Var] = problemeHebdo->CaracteristiquesHydrauliques[Pays]
                              ->ContrainteDePmaxHydrauliqueHoraire[PdtHebdo];
                AdresseDuResultat
                  = &(problemeHebdo->ResultatsHoraires[Pays]->TurbinageHoraire[PdtHebdo]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = AdresseDuResultat;
            }

            if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable
                == OUI_ANTARES)
            {
                if (problemeHebdo->TypeDeLissageHydraulique
                    == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
                {
                    Var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariablesVariationHydALaBaisse[Pays];
                    if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                    {
                        Xmin[Var] = 0.0;
                        Xmax[Var] = LINFINI_ANTARES;
                        AdresseOuPlacerLaValeurDesCoutsReduits[Var] = NULL;
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = NULL;
                    }
                    Var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariablesVariationHydALaHausse[Pays];
                    if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                    {
                        Xmin[Var] = 0.0;
                        Xmax[Var] = LINFINI_ANTARES;
                        AdresseOuPlacerLaValeurDesCoutsReduits[Var] = NULL;
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = NULL;
                    }
                }
                else if (problemeHebdo->TypeDeLissageHydraulique
                         == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
                {
                    if (PdtJour == 0)
                    {
                        Var = CorrespondanceVarNativesVarOptim
                                ->NumeroDeVariablesVariationHydALaBaisse[Pays];
                        if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                        {
                            Xmin[Var] = 0.0;
                            Xmax[Var] = problemeHebdo->CaracteristiquesHydrauliques[Pays]
                                          ->MaxDesPmaxHydrauliques;
                            AdresseOuPlacerLaValeurDesCoutsReduits[Var] = NULL;
                            AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = NULL;
                        }

                        Var = CorrespondanceVarNativesVarOptim
                                ->NumeroDeVariablesVariationHydALaHausse[Pays];
                        if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                        {
                            Xmin[Var] = 0.0;
                            Xmax[Var] = problemeHebdo->CaracteristiquesHydrauliques[Pays]
                                          ->MaxDesPmaxHydrauliques;
                            AdresseOuPlacerLaValeurDesCoutsReduits[Var] = NULL;
                            AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = NULL;
                        }
                    }
                }
            }

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDePompage[Pays];
            problemeHebdo->ResultatsHoraires[Pays]->PompageHoraire[PdtHebdo] = 0.0;
            if (Var >= 0)
            {
                Xmin[Var] = 0.0;
                Xmax[Var] = problemeHebdo->CaracteristiquesHydrauliques[Pays]
                              ->ContrainteDePmaxPompageHoraire[PdtHebdo];
                AdresseDuResultat
                  = &(problemeHebdo->ResultatsHoraires[Pays]->PompageHoraire[PdtHebdo]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = AdresseDuResultat;
            }

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeDebordement[Pays];

            problemeHebdo->ResultatsHoraires[Pays]->debordementsHoraires[PdtHebdo] = 0.;
            if (Var >= 0)
            {
                Xmin[Var] = 0.0;
                Xmax[Var] = problemeHebdo->CaracteristiquesHydrauliques[Pays]
                              ->ApportNaturelHoraire[PdtHebdo];
                AdresseOuPlacerLaValeurDesCoutsReduits[Var] = NULL;
                AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = NULL;
            }

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeNiveau[Pays];
            if (Var >= 0)
            {
                Xmin[Var]
                  = problemeHebdo->CaracteristiquesHydrauliques[Pays]->NiveauHoraireInf[PdtHebdo];
                Xmax[Var]
                  = problemeHebdo->CaracteristiquesHydrauliques[Pays]->NiveauHoraireSup[PdtHebdo];
                AdresseDuResultat
                  = &(problemeHebdo->ResultatsHoraires[Pays]->niveauxHoraires[PdtHebdo]);
                AdresseOuPlacerLaValeurDesCoutsReduits[Var] = NULL;
                AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = AdresseDuResultat;
            }

            {
                Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[Pays];

                Xmin[Var] = 0.0;

                Xmax[Var] = LINFINI_ANTARES;

                problemeHebdo->ResultatsHoraires[Pays]
                  ->ValeursHorairesDeDefaillanceNegative[PdtHebdo]
                  = 0.0;
                AdresseDuResultat = &(problemeHebdo->ResultatsHoraires[Pays]
                                        ->ValeursHorairesDeDefaillanceNegative[PdtHebdo]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = AdresseDuResultat;
            }

            problemeHebdo->ResultatsHoraires[Pays]->ValeursHorairesDeDefaillanceEnReserve[PdtHebdo]
              = 0.0;
        }
    }

    setBoundsForUnsuppliedEnergy(
      problemeHebdo, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle, optimizationNumber);

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->AccurateWaterValue == OUI_ANTARES)
        {
            Var = problemeHebdo->NumeroDeVariableStockFinal[Pays];
            if (Var >= 0)
            {
                Xmin[Var] = -(LINFINI_ANTARES);
                Xmax[Var] = LINFINI_ANTARES;

                AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = NULL;

                //	Note: if there were a single optimization run instead of two; the following
                // could be used: 	AdresseDuResultat =
                //&(problemeHebdo->CaracteristiquesHydrauliques[Pays]->LevelForTimeInterval);
                //	AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = AdresseDuResultat;

                AdresseOuPlacerLaValeurDesCoutsReduits[Var] = NULL;
            }
            for (uint nblayer = 0; nblayer < 100; nblayer++)
            {
                Var = problemeHebdo->NumeroDeVariableDeTrancheDeStock[Pays][nblayer];
                if (Var >= 0)
                {
                    Xmin[Var] = 0;
                    Xmax[Var] = problemeHebdo->CaracteristiquesHydrauliques[Pays]->TailleReservoir
                                / double(100);

                    AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = NULL;
                    AdresseOuPlacerLaValeurDesCoutsReduits[Var] = NULL;
                }
            }
        }
    }

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage == OUI_ANTARES)
    {
        OPT_InitialiserLesBornesDesVariablesDuProblemeLineaireCoutsDeDemarrage(
          problemeHebdo, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle);
    }

    return;
}
