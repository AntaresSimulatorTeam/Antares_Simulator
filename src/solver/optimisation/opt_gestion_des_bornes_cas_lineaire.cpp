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
#include "../simulation/adequacy_patch_runtime_data.h"

#include "opt_fonctions.h"
#include "adequacy_patch_local_matching/adq_patch_local_matching.h"
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

            int var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[Pays];
            Xmin[var] = 0.0;

            double MaxAllMustRunGenerationOfArea = 0.;
            if (AllMustRunGeneration->AllMustRunGenerationOfArea[Pays] > 0.)
                MaxAllMustRunGenerationOfArea
                  = AllMustRunGeneration->AllMustRunGenerationOfArea[Pays];

            ResidualLoadInArea += MaxAllMustRunGenerationOfArea;
            if (ResidualLoadInArea >= 0.)
                Xmax[var] = ResidualLoadInArea + 1e-5;
            else
                Xmax[var] = 0.;

            // adq patch: update ENS <= DENS in 2nd run
            // if (problemeHebdo->adqPatchParams
            //     && problemeHebdo->adqPatchParams->AdequacyFirstStep == false
            //     && problemeHebdo->adequacyPatchRuntimeData->areaMode[Pays]
            //          == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            //     Xmax[var] = std::min(
            //       Xmax[var], problemeHebdo->ResultatsHoraires[Pays]->ValeursHorairesDENS[PdtHebdo]);

            // no need to adjust Xmax -> simulate ENS <= DENS in 2nd run when AdequacyFirstStep == false

            problemeHebdo->ResultatsHoraires[Pays]->ValeursHorairesDeDefaillancePositive[PdtHebdo]
              = 0.0;

            AdresseOuPlacerLaValeurDesVariablesOptimisees[var]
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
    int var;
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

    for (var = 0; var < ProblemeAResoudre->NombreDeVariables; var++)
    {
        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;
        AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
    }

    for (PdtHebdo = PremierPdtDeLIntervalle, PdtJour = 0; PdtHebdo < DernierPdtDeLIntervalle;
         PdtHebdo++, PdtJour++)
    {
        CorrespondanceVarNativesVarOptim = problemeHebdo->CorrespondanceVarNativesVarOptim[PdtJour];
        ValeursDeNTC = problemeHebdo->ValeursDeNTC[PdtHebdo];

        for (Interco = 0; Interco < problemeHebdo->NombreDInterconnexions; Interco++)
        {
            var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
            CoutDeTransport = problemeHebdo->CoutDeTransport[Interco];

            AdequacyPatch::setNTCbounds(Xmax[var], Xmin[var], ValeursDeNTC, Interco, problemeHebdo); // since AdequacyFirstStep is always false bounds are set properly !!

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

            AdresseDuResultat = &(problemeHebdo->VariablesDualesDesContraintesDeNTC[PdtHebdo]
                                    ->VariableDualeParInterconnexion[Interco]);
            AdresseOuPlacerLaValeurDesCoutsReduits[var] = AdresseDuResultat;

            AdresseDuResultat = &(ValeursDeNTC->ValeurDuFlux[Interco]);
            AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = AdresseDuResultat;

            if (CoutDeTransport->IntercoGereeAvecDesCouts == OUI_ANTARES)
            {
                var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco];

                if (CoutDeTransport->IntercoGereeAvecLoopFlow == OUI_ANTARES)
                    Xmax[var] = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco]
                                - ValeursDeNTC->ValeurDeLoopFlowOrigineVersExtremite[Interco];
                else
                    Xmax[var] = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];

                Xmax[var] += 0.01;
                TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
                if (Math::Infinite(Xmax[var]) == 1)
                {
                    TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
                }
                Xmin[var] = 0.0;
                AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;

                var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco];
                if (CoutDeTransport->IntercoGereeAvecLoopFlow == OUI_ANTARES)
                    Xmax[var] = ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]
                                + ValeursDeNTC->ValeurDeLoopFlowOrigineVersExtremite[Interco];
                else
                    Xmax[var] = ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco];

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

        for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
        {
            PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[Pays];
            maxThermalPlant = PaliersThermiquesDuPays->NombreDePaliersThermiques;

            for (Index = 0; Index < maxThermalPlant; Index++)
            {
                Palier
                  = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
                var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[Palier];
                PuissanceDisponibleEtCout
                  = PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Index];

                Xmin[var] = PuissanceDisponibleEtCout->PuissanceMinDuPalierThermique[PdtHebdo];

                Xmax[var]
                  = PuissanceDisponibleEtCout->PuissanceDisponibleDuPalierThermique[PdtHebdo];

                AdresseDuResultat = &(problemeHebdo->ResultatsHoraires[Pays]
                                        ->ProductionThermique[PdtHebdo]
                                        ->ProductionThermiqueDuPalier[Index]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = AdresseDuResultat;
            }

            var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
            problemeHebdo->ResultatsHoraires[Pays]->TurbinageHoraire[PdtHebdo] = 0.0;
            if (var >= 0)
            {
                Xmin[var] = 0.0;
                Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[Pays]
                              ->ContrainteDePmaxHydrauliqueHoraire[PdtHebdo];
                AdresseDuResultat
                  = &(problemeHebdo->ResultatsHoraires[Pays]->TurbinageHoraire[PdtHebdo]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = AdresseDuResultat;
            }

            if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable
                == OUI_ANTARES)
            {
                if (problemeHebdo->TypeDeLissageHydraulique
                    == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
                {
                    var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariablesVariationHydALaBaisse[Pays];
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                    {
                        Xmin[var] = 0.0;
                        Xmax[var] = LINFINI_ANTARES;
                        AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;
                    }
                    var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariablesVariationHydALaHausse[Pays];
                    if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                    {
                        Xmin[var] = 0.0;
                        Xmax[var] = LINFINI_ANTARES;
                        AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
                        AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;
                    }
                }
                else if (problemeHebdo->TypeDeLissageHydraulique
                         == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
                {
                    if (PdtJour == 0)
                    {
                        var = CorrespondanceVarNativesVarOptim
                                ->NumeroDeVariablesVariationHydALaBaisse[Pays];
                        if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                        {
                            Xmin[var] = 0.0;
                            Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[Pays]
                                          ->MaxDesPmaxHydrauliques;
                            AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
                            AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;
                        }

                        var = CorrespondanceVarNativesVarOptim
                                ->NumeroDeVariablesVariationHydALaHausse[Pays];
                        if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                        {
                            Xmin[var] = 0.0;
                            Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[Pays]
                                          ->MaxDesPmaxHydrauliques;
                            AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
                            AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;
                        }
                    }
                }
            }

            var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDePompage[Pays];
            problemeHebdo->ResultatsHoraires[Pays]->PompageHoraire[PdtHebdo] = 0.0;
            if (var >= 0)
            {
                Xmin[var] = 0.0;
                Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[Pays]
                              ->ContrainteDePmaxPompageHoraire[PdtHebdo];
                AdresseDuResultat
                  = &(problemeHebdo->ResultatsHoraires[Pays]->PompageHoraire[PdtHebdo]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = AdresseDuResultat;
            }

            var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeDebordement[Pays];

            problemeHebdo->ResultatsHoraires[Pays]->debordementsHoraires[PdtHebdo] = 0.;
            if (var >= 0)
            {
                Xmin[var] = 0.0;
                Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[Pays]
                              ->ApportNaturelHoraire[PdtHebdo];
                AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;
            }

            var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeNiveau[Pays];
            if (var >= 0)
            {
                Xmin[var]
                  = problemeHebdo->CaracteristiquesHydrauliques[Pays]->NiveauHoraireInf[PdtHebdo];
                Xmax[var]
                  = problemeHebdo->CaracteristiquesHydrauliques[Pays]->NiveauHoraireSup[PdtHebdo];
                AdresseDuResultat
                  = &(problemeHebdo->ResultatsHoraires[Pays]->niveauxHoraires[PdtHebdo]);
                AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = AdresseDuResultat;
            }

            {
                var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[Pays];

                Xmin[var] = 0.0;

                Xmax[var] = LINFINI_ANTARES;

                problemeHebdo->ResultatsHoraires[Pays]
                  ->ValeursHorairesDeDefaillanceNegative[PdtHebdo]
                  = 0.0;
                AdresseDuResultat = &(problemeHebdo->ResultatsHoraires[Pays]
                                        ->ValeursHorairesDeDefaillanceNegative[PdtHebdo]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = AdresseDuResultat;
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
            var = problemeHebdo->NumeroDeVariableStockFinal[Pays];
            if (var >= 0)
            {
                Xmin[var] = -(LINFINI_ANTARES);
                Xmax[var] = LINFINI_ANTARES;

                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;

                //	Note: if there were a single optimization run instead of two; the following
                // could be used: 	AdresseDuResultat =
                //&(problemeHebdo->CaracteristiquesHydrauliques[Pays]->LevelForTimeInterval);
                //	AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = AdresseDuResultat;

                AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
            }
            for (uint nblayer = 0; nblayer < 100; nblayer++)
            {
                var = problemeHebdo->NumeroDeVariableDeTrancheDeStock[Pays][nblayer];
                if (var >= 0)
                {
                    Xmin[var] = 0;
                    Xmax[var] = problemeHebdo->CaracteristiquesHydrauliques[Pays]->TailleReservoir
                                / double(100);

                    AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = NULL;
                    AdresseOuPlacerLaValeurDesCoutsReduits[var] = NULL;
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
