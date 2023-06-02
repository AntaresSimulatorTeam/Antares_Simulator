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
                   int* Colonne,
                   int timeStepInYear)
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

            const auto& zone = problemeHebdo.NomsDesPays[pays];
            const auto& namePalier = PaliersThermiquesDuPays->NomsDesPaliersThermiques[index];
            RenameThermalClusterVariable(problemeHebdo.ProblemeAResoudre,
                                         var,
                                         Enum::ExportStructDict::PalierThermique,
                                         timeStepInYear,
                                         Enum::ExportStructTimeStepDict::hour,
                                         zone,
                                         namePalier);
        }
    }
}

static void shortTermStorageBalance(
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
  const std::string& area)
{
    const auto& VarOptim_current = CorrespondanceVarNativesVarOptim[pdt];
    // Cycle over the simulation period
    const int pdt1 = (pdt + 1) % nombreDePasDeTempsPourUneOptimisation;
    const auto& VarOptim_next = CorrespondanceVarNativesVarOptim[pdt1];
    for (auto& storage : shortTermStorageInput)
    {
        int nombreDeTermes = 0;
        const int clusterGlobalIndex = storage.clusterGlobalIndex;
        // L[h+1] - L[h] - efficiency * injection[h] + withdrawal[h] = inflows[h]
        if (const int varLevel_next
            = VarOptim_next->SIM_ShortTermStorage.LevelVariable[clusterGlobalIndex];
            varLevel_next >= 0)
        {
            Pi[nombreDeTermes] = 1.0;
            Colonne[nombreDeTermes] = varLevel_next;
            nombreDeTermes++;
        }

        if (const int varLevel
            = VarOptim_current->SIM_ShortTermStorage.LevelVariable[clusterGlobalIndex];
            varLevel >= 0)
        {
            Pi[nombreDeTermes] = -1.0;
            Colonne[nombreDeTermes] = varLevel;
            nombreDeTermes++;
        }

        if (const int varInjection
            = VarOptim_current->SIM_ShortTermStorage.InjectionVariable[clusterGlobalIndex];
            varInjection >= 0)
        {
            Pi[nombreDeTermes] = -1.0 * storage.efficiency;
            Colonne[nombreDeTermes] = varInjection;
            nombreDeTermes++;
        }

        if (const int varWithdrawal
            = VarOptim_current->SIM_ShortTermStorage.WithdrawalVariable[clusterGlobalIndex];
            varWithdrawal >= 0)
        {
            Pi[nombreDeTermes] = 1.0;
            Colonne[nombreDeTermes] = varWithdrawal;
            nombreDeTermes++;
        }
        CorrespondanceCntNativesCntOptim->ShortTermStorageLevelConstraint[clusterGlobalIndex]
          = ProblemeAResoudre->NombreDeContraintes;

        const auto constraintFullName
          = BuildName(Enum::toString(Enum::ExportStructConstraintsDict::ShortTermStorageLevel),
                      LocationIdentifier(area, Enum::ExportStructLocationDict::area),
                      TimeIdentifier(pdt1, Enum::ExportStructTimeStepDict::hour));
        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
          ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=', constraintFullName);
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

    // For now only variable are exported, can't define name for constraints export
    int nvars = ProblemeAResoudre->NombreDeVariables;
    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        int timeStepInYear = problemeHebdo->weekInTheYear * 168 + pdt;

        CorrespondanceVarNativesVarOptim = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
        CorrespondanceCntNativesCntOptim = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];

        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            int nombreDeTermes = 0;

            int interco = problemeHebdo->IndexDebutIntercoOrigine[pays];
            const auto& zone = problemeHebdo->NomsDesPays[pays];

            while (interco >= 0)
            {
                var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[interco];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;

                    const auto origin
                      = problemeHebdo
                          ->NomsDesPays[problemeHebdo->PaysOrigineDeLInterconnexion[interco]];
                    const auto destination
                      = problemeHebdo
                          ->NomsDesPays[problemeHebdo->PaysExtremiteDeLInterconnexion[interco]];
                    RenameLinkVariable(ProblemeAResoudre,
                                       var,
                                       Enum::ExportStructDict::ValeurDeNTCOrigineVersExtremite,
                                       timeStepInYear,
                                       Enum::ExportStructTimeStepDict::hour,
                                       origin,
                                       destination);
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

                    const auto origin
                      = problemeHebdo
                          ->NomsDesPays[problemeHebdo->PaysOrigineDeLInterconnexion[interco]];
                    const auto destination
                      = problemeHebdo
                          ->NomsDesPays[problemeHebdo->PaysExtremiteDeLInterconnexion[interco]];
                    RenameLinkVariable(ProblemeAResoudre,
                                       var,
                                       Enum::ExportStructDict::ValeurDeNTCOrigineVersExtremite,
                                       timeStepInYear,
                                       Enum::ExportStructTimeStepDict::hour,
                                       origin,
                                       destination);
                }
                interco = problemeHebdo->IndexSuivantIntercoExtremite[interco];
            }

            exportPaliers(*problemeHebdo,
                          *CorrespondanceVarNativesVarOptim,
                          pays,
                          nombreDeTermes,
                          Pi,
                          Colonne,
                          timeStepInYear);

            var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[pays];
            if (var >= 0)
            {
                Pi[nombreDeTermes] = -1.0;
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;

                RenameZoneVariable(ProblemeAResoudre,
                                   var,
                                   Enum::ExportStructDict::ProdHyd,
                                   timeStepInYear,
                                   Enum::ExportStructTimeStepDict::hour,
                                   zone);
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

                RenameZoneVariable(ProblemeAResoudre,
                                   var,
                                   Enum::ExportStructDict::DefaillancePositive,
                                   timeStepInYear,
                                   Enum::ExportStructTimeStepDict::hour,
                                   zone);
            }
            var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[pays];
            if (var >= 0)
            {
                Pi[nombreDeTermes] = 1.0;
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;

                RenameZoneVariable(ProblemeAResoudre,
                                   var,
                                   Enum::ExportStructDict::DefaillanceNegative,
                                   timeStepInYear,
                                   Enum::ExportStructTimeStepDict::hour,
                                   zone);
            }

            shortTermStorageBalance(problemeHebdo->ShortTermStorage[pays],
                                    *CorrespondanceVarNativesVarOptim,
                                    nombreDeTermes,
                                    Pi,
                                    Colonne);

            CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesBilansPays[pays]
              = ProblemeAResoudre->NombreDeContraintes;

            std::string constraintFullName
              = BuildName(Enum::toString(Enum::ExportStructConstraintsDict::AreaBalance),
                          LocationIdentifier(zone, Enum::ExportStructLocationDict::area),
                          TimeIdentifier(timeStepInYear, Enum::ExportStructTimeStepDict::hour));
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=', constraintFullName);

            nombreDeTermes = 0;

            exportPaliers(*problemeHebdo,
                          *CorrespondanceVarNativesVarOptim,
                          pays,
                          nombreDeTermes,
                          Pi,
                          Colonne,
                          timeStepInYear);

            var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[pays];
            if (var >= 0)
            {
                Pi[nombreDeTermes] = -problemeHebdo->DefaillanceNegativeUtiliserHydro[pays];
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;

                RenameZoneVariable(ProblemeAResoudre,
                                   var,
                                   Enum::ExportStructDict::ProdHyd,
                                   timeStepInYear,
                                   Enum::ExportStructTimeStepDict::hour,
                                   problemeHebdo->NomsDesPays[pays]);
            }

            var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[pays];
            if (var >= 0)
            {
                Pi[nombreDeTermes] = 1.0;
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;

                const auto& zone = problemeHebdo->NomsDesPays[pays];
                RenameZoneVariable(ProblemeAResoudre,
                                   var,
                                   Enum::ExportStructDict::DefaillanceNegative,
                                   timeStepInYear,
                                   Enum::ExportStructTimeStepDict::hour,
                                   zone);
            }

            CorrespondanceCntNativesCntOptim->NumeroDeContraintePourEviterLesChargesFictives[pays]
              = ProblemeAResoudre->NombreDeContraintes;

            constraintFullName.clear();
            constraintFullName
              = BuildName(Enum::toString(Enum::ExportStructConstraintsDict::FictiveLoads),
                          LocationIdentifier(zone, Enum::ExportStructLocationDict::area),
                          TimeIdentifier(timeStepInYear, Enum::ExportStructTimeStepDict::hour));
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '<', constraintFullName);

            // Short term storage
            shortTermStorageLevels(problemeHebdo->ShortTermStorage[pays],
                                   ProblemeAResoudre,
                                   CorrespondanceCntNativesCntOptim,
                                   problemeHebdo->CorrespondanceVarNativesVarOptim,
                                   Pi,
                                   Colonne,
                                   nombreDePasDeTempsPourUneOptimisation,
                                   pdt,
                                   zone);
        }

        for (int interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
        {
            const std::string origin
              = problemeHebdo->NomsDesPays[problemeHebdo->PaysOrigineDeLInterconnexion[interco]];
            const std::string destination
              = problemeHebdo->NomsDesPays[problemeHebdo->PaysExtremiteDeLInterconnexion[interco]];

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

                    // if (exportStructure)
                    // {
                    //     OPT_Export_add_variable(varname,
                    //                             var,
                    //                             Enum::ExportStructDict::DefaillanceNegative,
                    //                             timeStepInYear, // TODO[FOM] remove
                    //                             pays);
                    // }
                }
                var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[interco];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = -1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;

                    RenameLinkVariable(
                      ProblemeAResoudre,
                      var,
                      Enum::ExportStructDict::CoutOrigineVersExtremiteDeLInterconnexion,
                      timeStepInYear,
                      Enum::ExportStructTimeStepDict::hour,
                      origin,
                      destination);
                }
                var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[interco];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;

                    RenameLinkVariable(
                      ProblemeAResoudre,
                      var,
                      Enum::ExportStructDict::CoutExtremiteVersOrigineDeLInterconnexion,
                      timeStepInYear,
                      Enum::ExportStructTimeStepDict::hour,
                      origin,
                      destination);
                }

                CorrespondanceCntNativesCntOptim->NumeroDeContrainteDeDissociationDeFlux[interco]
                  = ProblemeAResoudre->NombreDeContraintes;

                std::string constraint_time
                  = TimeIdentifier(timeStepInYear, Enum::ExportStructTimeStepDict::hour);
                std::string constraint_location = LocationIdentifier(
                  origin + ZONE_SEPARATOR + destination, Enum::ExportStructLocationDict::link);
                auto constraintFullName
                  = BuildName(Antares::Data::Enum::toString(
                                Enum::ExportStructConstraintsDict::FlowDissociation),
                              constraint_location,
                              constraint_time);
                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=', constraintFullName);
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

                    const auto& zone = problemeHebdo->NomsDesPays[pays];
                    const auto& namePalier
                      = PaliersThermiquesDuPays->NomsDesPaliersThermiques
                          [MatriceDesContraintesCouplantes->NumeroDuPalierDispatch[index]];
                    RenameThermalClusterVariable(
                      ProblemeAResoudre,
                      var,
                      Enum::ExportStructDict::CorrespondanceVarNativesVarOptim,
                      pdt1,
                      Enum::ExportStructTimeStepDict::hour,
                      zone,
                      namePalier);
                }
            }

            CorrespondanceCntNativesCntOptim
              ->NumeroDeContrainteDesContraintesCouplantes[cntCouplante]
              = ProblemeAResoudre->NombreDeContraintes;

            std::string constraintFullName
              = BuildName(MatriceDesContraintesCouplantes->NomDeLaContrainteCouplante,
                          Enum::toString(Enum::ExportStructBindingConstraintType::hourly),
                          TimeIdentifier(timeStepInYear, Enum::ExportStructTimeStepDict::hour));

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre,
              Pi,
              Colonne,
              nombreDeTermes,
              MatriceDesContraintesCouplantes->SensDeLaContrainteCouplante,
              constraintFullName);
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

            std::string constraintFullName
              = BuildName(MatriceDesContraintesCouplantes->NomDeLaContrainteCouplante,
                          Enum::toString(Enum::ExportStructBindingConstraintType::daily),
                          TimeIdentifier(jour, Enum::ExportStructTimeStepDict::day));

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre,
              Pi,
              Colonne,
              nombreDeTermes,
              MatriceDesContraintesCouplantes->SensDeLaContrainteCouplante,
              constraintFullName);
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
            std::string constraintFullName
              = BuildName(MatriceDesContraintesCouplantes->NomDeLaContrainteCouplante,
                          Enum::toString(Enum::ExportStructBindingConstraintType::weekly),
                          TimeIdentifier(semaine, Enum::ExportStructTimeStepDict::week));

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre,
              Pi,
              Colonne,
              nombreDeTermes,
              MatriceDesContraintesCouplantes->SensDeLaContrainteCouplante,
              constraintFullName);
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const auto& zone = problemeHebdo->NomsDesPays[pays];
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
            std::string constraintFullName = BuildName(
              Enum::toString(Enum::ExportStructConstraintsDict::HydroPower),
              LocationIdentifier(zone, Enum::ExportStructLocationDict::area),
              TimeIdentifier(problemeHebdo->weekInTheYear, Enum::ExportStructTimeStepDict::week));

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=', constraintFullName);
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
            const auto& zone = problemeHebdo->NomsDesPays[pays];
            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
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
                int timeStepInYear = problemeHebdo->weekInTheYear * 168 + pdt;

                std::string constraintFullName = BuildName(
                  Enum::toString(
                    Enum::ExportStructConstraintsDict::HydroPowerSmoothingUsingVariationSum),
                  LocationIdentifier(zone, Enum::ExportStructLocationDict::area),
                  TimeIdentifier(timeStepInYear, Enum::ExportStructTimeStepDict::hour));
                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=', constraintFullName);
            }
        }
    }
    else if (problemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
    {
        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            if (!problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDHydrauliqueModulable)
                continue;

            const auto& zone = problemeHebdo->NomsDesPays[pays];
            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
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
                int timeStepInYear = problemeHebdo->weekInTheYear * 168 + pdt;
                std::string constraintFullName = BuildName(
                  Enum::toString(
                    Enum::ExportStructConstraintsDict::HydroPowerSmoothingUsingVariationMaxDown),
                  LocationIdentifier(zone, Enum::ExportStructLocationDict::area),
                  TimeIdentifier(timeStepInYear, Enum::ExportStructTimeStepDict::hour));
                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '<', constraintFullName);

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
                constraintFullName.clear();
                constraintFullName = BuildName(
                  Enum::toString(
                    Enum::ExportStructConstraintsDict::HydroPowerSmoothingUsingVariationMaxUp),
                  LocationIdentifier(zone, Enum::ExportStructLocationDict::area),
                  TimeIdentifier(timeStepInYear, Enum::ExportStructTimeStepDict::hour));

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '>', constraintFullName);
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
        const auto& zone = problemeHebdo->NomsDesPays[pays];

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
            std::string constraintFullName = BuildName(
              Enum::toString(Enum::ExportStructConstraintsDict::MinHydroPower),
              LocationIdentifier(zone, Enum::ExportStructLocationDict::area),
              TimeIdentifier(problemeHebdo->weekInTheYear, Enum::ExportStructTimeStepDict::week));

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '>', constraintFullName);
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
            std::string constraintFullName = BuildName(
              Enum::toString(Enum::ExportStructConstraintsDict::MaxHydroPower),
              LocationIdentifier(zone, Enum::ExportStructLocationDict::area),
              TimeIdentifier(problemeHebdo->weekInTheYear, Enum::ExportStructTimeStepDict::week));

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '<', constraintFullName);
        }
        else
            problemeHebdo->NumeroDeContrainteMaxEnergieHydraulique[pays] = -1;
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const auto& zone = problemeHebdo->NomsDesPays[pays];
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

            std::string constraintFullName = BuildName(
              Enum::toString(Enum::ExportStructConstraintsDict::MaxPumping),
              LocationIdentifier(zone, Enum::ExportStructLocationDict::area),
              TimeIdentifier(problemeHebdo->weekInTheYear, Enum::ExportStructTimeStepDict::week));

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '<', constraintFullName);
        }
        else
            problemeHebdo->NumeroDeContrainteMaxPompage[pays] = -1;
    }

    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        CorrespondanceVarNativesVarOptim = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
        CorrespondanceCntNativesCntOptim = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];

        int timeStepInYear = problemeHebdo->weekInTheYear * 168 + pdt;

        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
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
                std::string constraintFullName
                  = BuildName(Enum::toString(Enum::ExportStructConstraintsDict::AreaHydroLevel),
                              Enum::toString(Enum::ExportStructBindingConstraintType::hourly),
                              TimeIdentifier(timeStepInYear, Enum::ExportStructTimeStepDict::hour));

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=', constraintFullName);
            }
            else
                CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesNiveauxPays[pays] = -1;
        }
    }

    /* For each area with ad hoc properties, two possible sets of two additional constraints */
    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const auto& zone = problemeHebdo->NomsDesPays[pays];
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

            std::string constraintFullName = BuildName(
              Enum::toString(Enum::ExportStructConstraintsDict::FinalStockEquivalent),
              LocationIdentifier(zone, Enum::ExportStructLocationDict::area),
              TimeIdentifier(problemeHebdo->weekInTheYear, Enum::ExportStructTimeStepDict::week));

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=', constraintFullName);
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

            std::string constraintFullName = BuildName(
              Enum::toString(Enum::ExportStructConstraintsDict::FinalStockExpression),
              LocationIdentifier(zone, Enum::ExportStructLocationDict::area),
              TimeIdentifier(problemeHebdo->weekInTheYear, Enum::ExportStructTimeStepDict::week));

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=', constraintFullName);
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
