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
#include "opt_export_structure.h"

#include "../simulation/simulation.h"
#include "../utils/filename.h"
#include "opt_fonctions.h"
#include "opt_rename_problem.h"

#include <antares/study.h>

using namespace Antares::Data;

void exportPaliers(const PROBLEME_HEBDO& problemeHebdo,
                   const CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim,
                   int pays,
                   int& nombreDeTermes,
                   double* Pi,
                   int* Colonne)
{
    const PALIERS_THERMIQUES* PaliersThermiquesDuPays = problemeHebdo.PaliersThermiquesDuPays[pays];

    for (int index = 0; index < PaliersThermiquesDuPays->NombreDePaliersThermiques; index++)
    {
        const int palier
          = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        int var = CorrespondanceVarNativesVarOptim.NumeroDeVariableDuPalierThermique[palier];
        if (var >= 0)
        {
            Pi[nombreDeTermes] = -1.0;
            Colonne[nombreDeTermes] = var;
            nombreDeTermes++;
        }
    }
}

static void shortTermStorageBalance(
  PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
  const ::ShortTermStorage::AREA_INPUT& shortTermStorageInput,
  const CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim,
  int& nombreDeTermes,
  double* Pi,
  int* Colonne)
{
    for (const auto& storage : shortTermStorageInput)
    {
        const int clusterGlobalIndex = storage.clusterGlobalIndex;
        if (const int varInjection = CorrespondanceVarNativesVarOptim.SIM_ShortTermStorage
                                       .InjectionVariable[clusterGlobalIndex];
            varInjection >= 0)
        {
            Pi[nombreDeTermes] = 1.0;
            Colonne[nombreDeTermes] = varInjection;
            nombreDeTermes++;
        }

        if (const int varWithdrawal = CorrespondanceVarNativesVarOptim.SIM_ShortTermStorage
                                        .WithdrawalVariable[clusterGlobalIndex];
            varWithdrawal >= 0)
        {
            Pi[nombreDeTermes] = -1.0;
            Colonne[nombreDeTermes] = varWithdrawal;
            nombreDeTermes++;
        }
    }
}

static void shortTermStorageLevels(
  const ::ShortTermStorage::AREA_INPUT& shortTermStorageInput,
  PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
  CORRESPONDANCES_DES_CONTRAINTES* CorrespondanceCntNativesCntOptim,
  CORRESPONDANCES_DES_VARIABLES** CorrespondanceVarNativesVarOptim,
  double* Pi,
  int* Colonne,
  int nombreDePasDeTempsPourUneOptimisation,
  int pdt,
  ConstraintNamer& constraintNamer)
{
    const auto& VarOptimCurrent = CorrespondanceVarNativesVarOptim[pdt];
    // Cycle over the simulation period
    const int timestepPrevious
      = (pdt - 1 + nombreDePasDeTempsPourUneOptimisation) % nombreDePasDeTempsPourUneOptimisation;
    const auto& VarOptimPrevious = CorrespondanceVarNativesVarOptim[timestepPrevious];
    for (auto& storage : shortTermStorageInput)
    {
        int nombreDeTermes = 0;
        const int clusterGlobalIndex = storage.clusterGlobalIndex;
        // L[h] - L[h-1] - efficiency * injection[h] + withdrawal[h] = inflows[h]
        if (const int varLevel
            = VarOptimCurrent->SIM_ShortTermStorage.LevelVariable[clusterGlobalIndex];
            varLevel >= 0)
        {
            Pi[nombreDeTermes] = 1.0;
            Colonne[nombreDeTermes] = varLevel;
            nombreDeTermes++;
        }

        if (const int varLevel_previous
            = VarOptimPrevious->SIM_ShortTermStorage.LevelVariable[clusterGlobalIndex];
            varLevel_previous >= 0)
        {
            Pi[nombreDeTermes] = -1.0;
            Colonne[nombreDeTermes] = varLevel_previous;
            nombreDeTermes++;
        }

        if (const int varInjection
            = VarOptimCurrent->SIM_ShortTermStorage.InjectionVariable[clusterGlobalIndex];
            varInjection >= 0)
        {
            Pi[nombreDeTermes] = -1.0 * storage.efficiency;
            Colonne[nombreDeTermes] = varInjection;
            nombreDeTermes++;
        }

        if (const int varWithdrawal
            = VarOptimCurrent->SIM_ShortTermStorage.WithdrawalVariable[clusterGlobalIndex];
            varWithdrawal >= 0)
        {
            Pi[nombreDeTermes] = 1.0;
            Colonne[nombreDeTermes] = varWithdrawal;
            nombreDeTermes++;
        }
        CorrespondanceCntNativesCntOptim->ShortTermStorageLevelConstraint[clusterGlobalIndex]
          = ProblemeAResoudre->NombreDeContraintes;

        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
          ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=');
        constraintNamer.ShortTermStorageLevel();
    }
}

void OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaire(PROBLEME_HEBDO* problemeHebdo)
{
    int var;

    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    CORRESPONDANCES_DES_CONTRAINTES* CorrespondanceCntNativesCntOptim;

    Study::Ptr study = Study::Current::Get();
    const bool exportStructure = problemeHebdo->ExportStructure;
    const bool firstWeekOfSimulation = problemeHebdo->firstWeekOfSimulation;

    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    int nombreDePasDeTempsDUneJournee = problemeHebdo->NombreDePasDeTempsDUneJournee;
    int nombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    double* Pi = ProblemeAResoudre->Pi;
    int* Colonne = ProblemeAResoudre->Colonne;

    ProblemeAResoudre->NombreDeContraintes = 0;
    ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;
    ConstraintNamer constraintNamer(ProblemeAResoudre);

    // For now only variable are exported, can't define name for constraints export
    int nvars = ProblemeAResoudre->NombreDeVariables;
    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        int timeStepInYear = problemeHebdo->weekInTheYear * 168 + pdt;
        constraintNamer.UpdateTimeStep(timeStepInYear);
        CorrespondanceVarNativesVarOptim = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
        CorrespondanceCntNativesCntOptim = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];

        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            int nombreDeTermes = 0;

            int interco = problemeHebdo->IndexDebutIntercoOrigine[pays];
            const auto& area = problemeHebdo->NomsDesPays[pays];
            constraintNamer.UpdateArea(area);

            while (interco >= 0)
            {
                var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[interco];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
                interco = problemeHebdo->IndexSuivantIntercoOrigine[interco];
            }
            interco = problemeHebdo->IndexDebutIntercoExtremite[pays];
            while (interco >= 0)
            {
                var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[interco];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = -1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
                interco = problemeHebdo->IndexSuivantIntercoExtremite[interco];
            }

            exportPaliers(
              *problemeHebdo, *CorrespondanceVarNativesVarOptim, pays, nombreDeTermes, Pi, Colonne);

            constraintNamer.UpdateArea(area);
            var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[pays];
            if (var >= 0)
            {
                Pi[nombreDeTermes] = -1.0;
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;
            }

            var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDePompage[pays];
            if (var >= 0)
            {
                Pi[nombreDeTermes] = 1.0;
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;
            }

            var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[pays];
            if (var >= 0)
            {
                Pi[nombreDeTermes] = -1.0;
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;
            }
            var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[pays];
            if (var >= 0)
            {
                Pi[nombreDeTermes] = 1.0;
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;
            }

            shortTermStorageBalance(ProblemeAResoudre,
                                    problemeHebdo->ShortTermStorage[pays],
                                    *CorrespondanceVarNativesVarOptim,
                                    nombreDeTermes,
                                    Pi,
                                    Colonne);

            CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesBilansPays[pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=');
            constraintNamer.AreaBalance();
            nombreDeTermes = 0;

            exportPaliers(
              *problemeHebdo, *CorrespondanceVarNativesVarOptim, pays, nombreDeTermes, Pi, Colonne);
            constraintNamer.UpdateArea(area);
            var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[pays];
            if (var >= 0)
            {
                Pi[nombreDeTermes] = -problemeHebdo->DefaillanceNegativeUtiliserHydro[pays];
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;
            }

            var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[pays];
            if (var >= 0)
            {
                Pi[nombreDeTermes] = 1.0;
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;

                const auto& area = problemeHebdo->NomsDesPays[pays];
            }

            CorrespondanceCntNativesCntOptim->NumeroDeContraintePourEviterLesChargesFictives[pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '<');
            constraintNamer.FictiveLoads();
            // Short term storage
            shortTermStorageLevels(problemeHebdo->ShortTermStorage[pays],
                                   ProblemeAResoudre,
                                   CorrespondanceCntNativesCntOptim,
                                   problemeHebdo->CorrespondanceVarNativesVarOptim,
                                   Pi,
                                   Colonne,
                                   nombreDePasDeTempsPourUneOptimisation,
                                   pdt,
                                   constraintNamer);
        }

        for (int interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
        {
            const COUTS_DE_TRANSPORT* CoutDeTransport = problemeHebdo->CoutDeTransport[interco];
            if (CoutDeTransport->IntercoGereeAvecDesCouts)
            {
                int nombreDeTermes = 0;
                var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[interco];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;

                    // TODO VP: var "pays" is used in function call below but is previously used to
                    // loop
                    // until NombreDePays, check if it's expected behavior
                    // I added the next line to keep the same behavior as before
                    int pays = problemeHebdo->NombreDePays;
                }
                var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[interco];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = -1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
                var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[interco];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }

                CorrespondanceCntNativesCntOptim->NumeroDeContrainteDeDissociationDeFlux[interco]
                  = ProblemeAResoudre->NombreDeContraintes;

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=');
                constraintNamer.FlowDissociation();
            }
        }

        for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            const CONTRAINTES_COUPLANTES* MatriceDesContraintesCouplantes
              = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];

            if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante != CONTRAINTE_HORAIRE)
                continue;

            int nbInterco
              = MatriceDesContraintesCouplantes->NombreDInterconnexionsDansLaContrainteCouplante;
            int nombreDeTermes = 0;
            for (int index = 0; index < nbInterco; index++)
            {
                int interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[index];
                double poids = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[index];
                int Offset = MatriceDesContraintesCouplantes->OffsetTemporelSurLInterco[index];
                int pdt1;

                if (Offset >= 0)
                {
                    pdt1 = (pdt + Offset) % nombreDePasDeTempsPourUneOptimisation;
                }
                else
                {
                    pdt1 = (pdt + Offset + problemeHebdo->NombreDePasDeTemps)
                           % nombreDePasDeTempsPourUneOptimisation;
                }

                var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt1]
                        ->NumeroDeVariableDeLInterconnexion[interco];

                if (var >= 0)
                {
                    Pi[nombreDeTermes] = poids;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
            }

            int nbClusters
              = MatriceDesContraintesCouplantes->NombreDePaliersDispatchDansLaContrainteCouplante;
            for (int index = 0; index < nbClusters; index++)
            {
                int pays = MatriceDesContraintesCouplantes->PaysDuPalierDispatch[index];
                const PALIERS_THERMIQUES* PaliersThermiquesDuPays
                  = problemeHebdo->PaliersThermiquesDuPays[pays];
                const int palier
                  = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques
                      [MatriceDesContraintesCouplantes->NumeroDuPalierDispatch[index]];
                double poids = MatriceDesContraintesCouplantes->PoidsDuPalierDispatch[index];
                int Offset
                  = MatriceDesContraintesCouplantes->OffsetTemporelSurLePalierDispatch[index];
                int pdt1;
                if (Offset >= 0)
                {
                    pdt1 = (pdt + Offset) % nombreDePasDeTempsPourUneOptimisation;
                }
                else
                {
                    pdt1 = (pdt + Offset + problemeHebdo->NombreDePasDeTemps)
                           % nombreDePasDeTempsPourUneOptimisation;
                }

                var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt1]
                        ->NumeroDeVariableDuPalierThermique[palier];

                if (var >= 0)
                {
                    Pi[nombreDeTermes] = poids;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
            }
            CorrespondanceCntNativesCntOptim
              ->NumeroDeContrainteDesContraintesCouplantes[cntCouplante]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre,
              Pi,
              Colonne,
              nombreDeTermes,
              MatriceDesContraintesCouplantes->SensDeLaContrainteCouplante);
            constraintNamer.BindingConstraintHour(
              MatriceDesContraintesCouplantes->NomDeLaContrainteCouplante);
        }
    }

    for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         cntCouplante++)
    {
        const CONTRAINTES_COUPLANTES* MatriceDesContraintesCouplantes
          = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];
        if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante != CONTRAINTE_JOURNALIERE)
            continue;

        int nbInterco
          = MatriceDesContraintesCouplantes->NombreDInterconnexionsDansLaContrainteCouplante;
        int nbClusters
          = MatriceDesContraintesCouplantes->NombreDePaliersDispatchDansLaContrainteCouplante;
        int pdtDebut = 0;
        while (pdtDebut < nombreDePasDeTempsPourUneOptimisation)
        {
            int jour = problemeHebdo->NumeroDeJourDuPasDeTemps[pdtDebut];
            CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES*
              CorrespondanceCntNativesCntOptimJournalieres
              = problemeHebdo->CorrespondanceCntNativesCntOptimJournalieres[jour];
            int nombreDeTermes = 0;

            for (int index = 0; index < nbInterco; index++)
            {
                int interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[index];
                double poids = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[index];
                int Offset = MatriceDesContraintesCouplantes->OffsetTemporelSurLInterco[index];

                for (int pdt = pdtDebut; pdt < pdtDebut + nombreDePasDeTempsDUneJournee; pdt++)
                {
                    int pdt1;
                    if (Offset >= 0)
                    {
                        pdt1 = (pdt + Offset) % nombreDePasDeTempsPourUneOptimisation;
                    }
                    else
                    {
                        pdt1 = (pdt + Offset + problemeHebdo->NombreDePasDeTemps)
                               % nombreDePasDeTempsPourUneOptimisation;
                    }

                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt1]
                            ->NumeroDeVariableDeLInterconnexion[interco];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = poids;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
            }

            for (int index = 0; index < nbClusters; index++)
            {
                int pays = MatriceDesContraintesCouplantes->PaysDuPalierDispatch[index];
                const PALIERS_THERMIQUES* PaliersThermiquesDuPays
                  = problemeHebdo->PaliersThermiquesDuPays[pays];
                const int palier
                  = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques
                      [MatriceDesContraintesCouplantes->NumeroDuPalierDispatch[index]];
                double poids = MatriceDesContraintesCouplantes->PoidsDuPalierDispatch[index];
                int Offset
                  = MatriceDesContraintesCouplantes->OffsetTemporelSurLePalierDispatch[index];

                for (int pdt = pdtDebut; pdt < pdtDebut + nombreDePasDeTempsDUneJournee; pdt++)
                {
                    int pdt1;
                    if (Offset >= 0)
                    {
                        pdt1 = (pdt + Offset) % nombreDePasDeTempsPourUneOptimisation;
                    }
                    else
                    {
                        pdt1 = (pdt + Offset + problemeHebdo->NombreDePasDeTemps)
                               % nombreDePasDeTempsPourUneOptimisation;
                    }

                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt1]
                            ->NumeroDeVariableDuPalierThermique[palier];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = poids;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
            }

            assert(cntCouplante >= 0);
            CorrespondanceCntNativesCntOptimJournalieres
              ->NumeroDeContrainteDesContraintesCouplantes[cntCouplante]
              = ProblemeAResoudre->NombreDeContraintes;

            constraintNamer.UpdateTimeStep(jour);
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre,
              Pi,
              Colonne,
              nombreDeTermes,
              MatriceDesContraintesCouplantes->SensDeLaContrainteCouplante);
            constraintNamer.BindingConstraintDay(
              MatriceDesContraintesCouplantes->NomDeLaContrainteCouplante);
            pdtDebut += nombreDePasDeTempsDUneJournee;
        }
    }

    if (nombreDePasDeTempsPourUneOptimisation > nombreDePasDeTempsDUneJournee)
    {
        int semaine = problemeHebdo->weekInTheYear;
        CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES* CorrespondanceCntNativesCntOptimHebdomadaires
          = problemeHebdo->CorrespondanceCntNativesCntOptimHebdomadaires[0];
        for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            const CONTRAINTES_COUPLANTES* MatriceDesContraintesCouplantes
              = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];
            if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante
                != CONTRAINTE_HEBDOMADAIRE)
                continue;

            int nbInterco
              = MatriceDesContraintesCouplantes->NombreDInterconnexionsDansLaContrainteCouplante;
            int nombreDeTermes = 0;
            for (int index = 0; index < nbInterco; index++)
            {
                int interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[index];
                double poids = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[index];
                int Offset = MatriceDesContraintesCouplantes->OffsetTemporelSurLInterco[index];
                for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
                {
                    int pdt1;
                    if (Offset >= 0)
                    {
                        pdt1 = (pdt + Offset) % nombreDePasDeTempsPourUneOptimisation;
                    }
                    else
                    {
                        pdt1 = (pdt + Offset + problemeHebdo->NombreDePasDeTemps)
                               % nombreDePasDeTempsPourUneOptimisation;
                    }

                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt1]
                            ->NumeroDeVariableDeLInterconnexion[interco];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = poids;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
            }

            int nbClusters
              = MatriceDesContraintesCouplantes->NombreDePaliersDispatchDansLaContrainteCouplante;
            for (int index = 0; index < nbClusters; index++)
            {
                int pays = MatriceDesContraintesCouplantes->PaysDuPalierDispatch[index];
                const PALIERS_THERMIQUES* PaliersThermiquesDuPays
                  = problemeHebdo->PaliersThermiquesDuPays[pays];
                const int palier
                  = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques
                      [MatriceDesContraintesCouplantes->NumeroDuPalierDispatch[index]];
                double poids = MatriceDesContraintesCouplantes->PoidsDuPalierDispatch[index];
                int Offset
                  = MatriceDesContraintesCouplantes->OffsetTemporelSurLePalierDispatch[index];
                for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
                {
                    int pdt1;
                    if (Offset >= 0)
                    {
                        pdt1 = (pdt + Offset) % nombreDePasDeTempsPourUneOptimisation;
                    }
                    else
                    {
                        pdt1 = (pdt + Offset + problemeHebdo->NombreDePasDeTemps)
                               % nombreDePasDeTempsPourUneOptimisation;
                    }

                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt1]
                            ->NumeroDeVariableDuPalierThermique[palier];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = poids;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
            }

            CorrespondanceCntNativesCntOptimHebdomadaires
              ->NumeroDeContrainteDesContraintesCouplantes[cntCouplante]
              = ProblemeAResoudre->NombreDeContraintes;

            constraintNamer.UpdateTimeStep(semaine);
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre,
              Pi,
              Colonne,
              nombreDeTermes,
              MatriceDesContraintesCouplantes->SensDeLaContrainteCouplante);
            constraintNamer.BindingConstraintWeek(
              MatriceDesContraintesCouplantes->NomDeLaContrainteCouplante);
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const auto& area = problemeHebdo->NomsDesPays[pays];
        constraintNamer.UpdateArea(area);
        bool presenceHydro
          = problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDHydrauliqueModulable;
        bool TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[pays]->TurbinageEntreBornes;
        bool presencePompage
          = problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDePompageModulable;

        int nombreDeTermes = 0;
        if (presenceHydro && !TurbEntreBornes)
        {
            if (presencePompage)
            {
                for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
                {
                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt]
                            ->NumeroDeVariablesDeLaProdHyd[pays];

                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = 1.0;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt]
                            ->NumeroDeVariablesDePompage[pays];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes]
                          = problemeHebdo->CaracteristiquesHydrauliques[pays]->PumpingRatio;
                        Pi[nombreDeTermes] *= -1.0;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
            }
            else
            {
                for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
                {
                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt]
                            ->NumeroDeVariablesDeLaProdHyd[pays];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = 1.0;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
            }

            problemeHebdo->NumeroDeContrainteEnergieHydraulique[pays]
              = ProblemeAResoudre->NombreDeContraintes;
            constraintNamer.UpdateTimeStep(problemeHebdo->weekInTheYear);
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=');
            constraintNamer.HydroPower();
        }
        else
            problemeHebdo->NumeroDeContrainteEnergieHydraulique[pays] = -1;
    }

    if (problemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
    {
        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            if (!problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDHydrauliqueModulable)
                continue;
            const auto& area = problemeHebdo->NomsDesPays[pays];
            constraintNamer.UpdateArea(area);
            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                int timeStepInYear = problemeHebdo->weekInTheYear * 168 + pdt;
                constraintNamer.UpdateTimeStep(timeStepInYear);
                CorrespondanceVarNativesVarOptim
                  = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
                int nombreDeTermes = 0;
                var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[pays];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
                int pdt1 = pdt + 1;
                if (pdt1 >= nombreDePasDeTempsPourUneOptimisation)
                    pdt1 = 0;

                if (int var1 = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt1]
                                 ->NumeroDeVariablesDeLaProdHyd[pays];
                    var1 >= 0)
                {
                    Pi[nombreDeTermes] = -1.0;
                    Colonne[nombreDeTermes] = var1;
                    nombreDeTermes++;
                }

                if (int var2 = CorrespondanceVarNativesVarOptim
                                 ->NumeroDeVariablesVariationHydALaBaisse[pays];
                    var2 >= 0)
                {
                    Pi[nombreDeTermes] = -1.0;
                    Colonne[nombreDeTermes] = var2;
                    nombreDeTermes++;
                }

                if (int var3 = CorrespondanceVarNativesVarOptim
                                 ->NumeroDeVariablesVariationHydALaHausse[pays];
                    var3 >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var3;
                    nombreDeTermes++;
                }

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=');
                constraintNamer.HydroPowerSmoothingUsingVariationSum();
            }
        }
    }
    else if (problemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
    {
        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            if (!problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDHydrauliqueModulable)
                continue;

            const auto& area = problemeHebdo->NomsDesPays[pays];
            constraintNamer.UpdateArea(area);
            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                int timeStepInYear = problemeHebdo->weekInTheYear * 168 + pdt;
                constraintNamer.UpdateTimeStep(timeStepInYear);
                CorrespondanceVarNativesVarOptim
                  = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
                int nombreDeTermes = 0;
                var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[pays];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
                int var1 = problemeHebdo->CorrespondanceVarNativesVarOptim[0]
                             ->NumeroDeVariablesVariationHydALaBaisse[pays];
                if (var1 >= 0)
                {
                    Pi[nombreDeTermes] = -1.0;
                    Colonne[nombreDeTermes] = var1;
                    nombreDeTermes++;
                }

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '<');
                constraintNamer.HydroPowerSmoothingUsingVariationMaxDown();

                nombreDeTermes = 0;
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
                var1 = problemeHebdo->CorrespondanceVarNativesVarOptim[0]
                         ->NumeroDeVariablesVariationHydALaHausse[pays];
                if (var1 >= 0)
                {
                    Pi[nombreDeTermes] = -1.0;
                    Colonne[nombreDeTermes] = var1;
                    nombreDeTermes++;
                }

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '>');
                constraintNamer.HydroPowerSmoothingUsingVariationMaxUp();
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const bool presenceHydro
          = problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDHydrauliqueModulable;
        const bool presencePompage
          = problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDePompageModulable;
        const bool TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[pays]->TurbinageEntreBornes;
        const auto& area = problemeHebdo->NomsDesPays[pays];
        constraintNamer.UpdateArea(area);
        if (presenceHydro && (TurbEntreBornes || presencePompage))
        {
            int nombreDeTermes = 0;
            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt]
                        ->NumeroDeVariablesDeLaProdHyd[pays];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
            }

            problemeHebdo->NumeroDeContrainteMinEnergieHydraulique[pays]
              = ProblemeAResoudre->NombreDeContraintes;
            constraintNamer.UpdateTimeStep(problemeHebdo->weekInTheYear);
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '>');
            constraintNamer.MinHydroPower();
        }
        else
            problemeHebdo->NumeroDeContrainteMinEnergieHydraulique[pays] = -1;

        if (presenceHydro
            && (TurbEntreBornes
                || problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDePompageModulable))
        {
            int nombreDeTermes = 0;
            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                constraintNamer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
                var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt]
                        ->NumeroDeVariablesDeLaProdHyd[pays];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
            }

            problemeHebdo->NumeroDeContrainteMaxEnergieHydraulique[pays]
              = ProblemeAResoudre->NombreDeContraintes;
            constraintNamer.UpdateTimeStep(problemeHebdo->weekInTheYear);
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '<');
            constraintNamer.MaxHydroPower();
        }
        else
            problemeHebdo->NumeroDeContrainteMaxEnergieHydraulique[pays] = -1;
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const auto& area = problemeHebdo->NomsDesPays[pays];
        if (problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDePompageModulable)
        {
            int nombreDeTermes = 0;
            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt]
                        ->NumeroDeVariablesDePompage[pays];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
            }

            problemeHebdo->NumeroDeContrainteMaxPompage[pays]
              = ProblemeAResoudre->NombreDeContraintes;

            constraintNamer.UpdateTimeStep(problemeHebdo->weekInTheYear);
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '<');
            constraintNamer.MaxPumping();
        }
        else
            problemeHebdo->NumeroDeContrainteMaxPompage[pays] = -1;
    }

    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        CorrespondanceVarNativesVarOptim = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
        CorrespondanceCntNativesCntOptim = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];

        int timeStepInYear = problemeHebdo->weekInTheYear * 168 + pdt;
        constraintNamer.UpdateTimeStep(timeStepInYear);
        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            const auto& area = problemeHebdo->NomsDesPays[pays];
            constraintNamer.UpdateArea(area);
            if (problemeHebdo->CaracteristiquesHydrauliques[pays]->SuiviNiveauHoraire)
            {
                int nombreDeTermes = 0;

                var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeNiveau[pays];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }

                if (pdt > 0)
                {
                    int var1 = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt - 1]
                                 ->NumeroDeVariablesDeNiveau[pays];
                    if (var1 >= 0)
                    {
                        Pi[nombreDeTermes] = -1.0;
                        Colonne[nombreDeTermes] = var1;
                        nombreDeTermes++;
                    }
                }

                var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[pays];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }

                var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDePompage[pays];
                if (var >= 0)
                {
                    Pi[nombreDeTermes]
                      = problemeHebdo->CaracteristiquesHydrauliques[pays]->PumpingRatio;
                    Pi[nombreDeTermes] *= -1;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }

                var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeDebordement[pays];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }

                CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesNiveauxPays[pays]
                  = ProblemeAResoudre->NombreDeContraintes;

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=');
                constraintNamer.AreaHydroLevel();
            }
            else
                CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesNiveauxPays[pays] = -1;
        }
    }

    /* For each area with ad hoc properties, two possible sets of two additional constraints */
    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const auto& week = problemeHebdo->weekInTheYear;
        const auto& area = problemeHebdo->NomsDesPays[pays];
        constraintNamer.UpdateArea(area);
        constraintNamer.UpdateTimeStep(week * 168 + nombreDePasDeTempsPourUneOptimisation - 1);

        if (problemeHebdo->CaracteristiquesHydrauliques[pays]->AccurateWaterValue
            && problemeHebdo->CaracteristiquesHydrauliques[pays]->DirectLevelAccess)
        /*  equivalence constraint : StockFinal- Niveau[T]= 0*/
        {
            int nombreDeTermes = 0;
            var = problemeHebdo->NumeroDeVariableStockFinal[pays];
            if (var >= 0)
            {
                Pi[nombreDeTermes] = 1.0;
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;
            }
            var = problemeHebdo
                    ->CorrespondanceVarNativesVarOptim[nombreDePasDeTempsPourUneOptimisation - 1]
                    ->NumeroDeVariablesDeNiveau[pays];
            if (var >= 0)
            {
                Pi[nombreDeTermes] = -1.0;
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;
            }
            problemeHebdo->NumeroDeContrainteEquivalenceStockFinal[pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=');
            constraintNamer.FinalStockEquivalent();
        }
        if (problemeHebdo->CaracteristiquesHydrauliques[pays]->AccurateWaterValue)
        /*  expression constraint : - StockFinal +sum (stocklayers) = 0*/
        {
            int nombreDeTermes = 0;
            var = problemeHebdo->NumeroDeVariableStockFinal[pays];
            if (var >= 0)
            {
                Pi[nombreDeTermes] = -1.0;
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;
            }
            for (int layerindex = 0; layerindex < 100; layerindex++)
            {
                var = problemeHebdo->NumeroDeVariableDeTrancheDeStock[pays][layerindex];

                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
            }

            problemeHebdo->NumeroDeContrainteExpressionStockFinal[pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=');
            constraintNamer.FinalStockExpression();
        }
    }

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    {
        OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaireCoutsDeDemarrage(problemeHebdo,
                                                                                false);
    }

    // Export structure
    if (exportStructure)
    {
        if (firstWeekOfSimulation)
        {
            OPT_ExportInterco(study->resultWriter, problemeHebdo);
            OPT_ExportAreaName(study->resultWriter, study->areas);
        }
    }

    return;
}
