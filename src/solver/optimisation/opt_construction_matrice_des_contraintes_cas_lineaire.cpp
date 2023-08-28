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
#include "../utils/filename.h"
#include "opt_fonctions.h"
#include "constraint_builder.h"
#include "opt_rename_problem.h"
#include "sim_structure_probleme_economique.h"

#include <antares/study.h>

using namespace Antares::Data;

// TODO DELETE THIS FILE

void OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaire(PROBLEME_HEBDO* problemeHebdo,
                                                             Solver::IResultWriter& writer)
{
    int var;

    const bool exportStructure = problemeHebdo->ExportStructure;
    const bool firstWeekOfSimulation = problemeHebdo->firstWeekOfSimulation;

    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre.get();

    int nombreDePasDeTempsDUneJournee = problemeHebdo->NombreDePasDeTempsDUneJournee;
    int nombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    std::vector<double>& Pi = ProblemeAResoudre->Pi;
    std::vector<int>& Colonne = ProblemeAResoudre->Colonne;

    ProblemeAResoudre->NombreDeContraintes = 0;
    ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;
    ConstraintNamer constraintNamer(ProblemeAResoudre->NomDesContraintes,
                                    problemeHebdo->NamedProblems);
    
    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
      auto& CorrespondanceVarNativesVarOptim = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
        int timeStepInYear = problemeHebdo->weekInTheYear * 168 + pdt;
        constraintNamer.UpdateTimeStep(timeStepInYear);

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {

            // Some other constraint
            int nombreDeTermes = 0;

            // exportPaliers(
            //   *problemeHebdo, CorrespondanceVarNativesVarOptim, pays, nombreDeTermes, Pi, Colonne);
            // var = CorrespondanceVarNativesVarOptim.NumeroDeVariablesDeLaProdHyd[pays];
            if (var >= 0)
            {
                Pi[nombreDeTermes] = -problemeHebdo->DefaillanceNegativeUtiliserHydro[pays];
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;
            }

            var = CorrespondanceVarNativesVarOptim.NumeroDeVariableDefaillanceNegative[pays];
            if (var >= 0)
            {
                Pi[nombreDeTermes] = 1.0;
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;
            }

            constraintNamer.FictiveLoads(ProblemeAResoudre->NombreDeContraintes);
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '<');
            // Short term storage
            // shortTermStorageLevels(problemeHebdo->ShortTermStorage[pays],
            //                        ProblemeAResoudre,
            //                        problemeHebdo->CorrespondanceVarNativesVarOptim,
            //                        Pi,
            //                        Colonne,
            //                        nombreDePasDeTempsPourUneOptimisation,
            //                        pdt,
            //                        constraintNamer);
        }

        for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
        {
            if (problemeHebdo->CoutDeTransport[interco].IntercoGereeAvecDesCouts)
            {
                int nombreDeTermes = 0;
                var = CorrespondanceVarNativesVarOptim.NumeroDeVariableDeLInterconnexion[interco];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
                var = CorrespondanceVarNativesVarOptim
                        .NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[interco];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = -1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
                var = CorrespondanceVarNativesVarOptim
                        .NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[interco];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }

                const auto origin
                  = problemeHebdo
                      ->NomsDesPays[problemeHebdo->PaysOrigineDeLInterconnexion[interco]];
                const auto destination
                  = problemeHebdo
                      ->NomsDesPays[problemeHebdo->PaysExtremiteDeLInterconnexion[interco]];
                constraintNamer.FlowDissociation(
                  ProblemeAResoudre->NombreDeContraintes, origin, destination);
                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=');
            }
        }

        for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
              = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];

            if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_HORAIRE)
                continue;

            int nbInterco
              = MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante;
            int nombreDeTermes = 0;
            for (int index = 0; index < nbInterco; index++)
            {
                int interco = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
                double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
                int Offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];
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
                        .NumeroDeVariableDeLInterconnexion[interco];

                if (var >= 0)
                {
                    Pi[nombreDeTermes] = poids;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
            }

            int nbClusters
              = MatriceDesContraintesCouplantes.NombreDePaliersDispatchDansLaContrainteCouplante;
            for (int index = 0; index < nbClusters; index++)
            {
                int pays = MatriceDesContraintesCouplantes.PaysDuPalierDispatch[index];
                const PALIERS_THERMIQUES& PaliersThermiquesDuPays
                  = problemeHebdo->PaliersThermiquesDuPays[pays];
                const int palier
                  = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                      [MatriceDesContraintesCouplantes.NumeroDuPalierDispatch[index]];
                double poids = MatriceDesContraintesCouplantes.PoidsDuPalierDispatch[index];
                int Offset
                  = MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch[index];
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
                        .NumeroDeVariableDuPalierThermique[palier];

                if (var >= 0)
                {
                    Pi[nombreDeTermes] = poids;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
            }
            constraintNamer.BindingConstraintHour(
              ProblemeAResoudre->NombreDeContraintes,
              MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante);
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre,
              Pi,
              Colonne,
              nombreDeTermes,
              MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante);
        }
    }

    for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         cntCouplante++)
    {
        const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
          = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];
        if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_JOURNALIERE)
            continue;

        int nbInterco
          = MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante;
        int nbClusters
          = MatriceDesContraintesCouplantes.NombreDePaliersDispatchDansLaContrainteCouplante;
        int pdtDebut = 0;
        while (pdtDebut < nombreDePasDeTempsPourUneOptimisation)
        {
            int jour = problemeHebdo->NumeroDeJourDuPasDeTemps[pdtDebut];
            int nombreDeTermes = 0;

            for (int index = 0; index < nbInterco; index++)
            {
                int interco = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
                double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
                int Offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];

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
                            .NumeroDeVariableDeLInterconnexion[interco];
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
                int pays = MatriceDesContraintesCouplantes.PaysDuPalierDispatch[index];
                const PALIERS_THERMIQUES& PaliersThermiquesDuPays
                  = problemeHebdo->PaliersThermiquesDuPays[pays];
                const int palier
                  = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                      [MatriceDesContraintesCouplantes.NumeroDuPalierDispatch[index]];
                double poids = MatriceDesContraintesCouplantes.PoidsDuPalierDispatch[index];
                int Offset
                  = MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch[index];

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
                            .NumeroDeVariableDuPalierThermique[palier];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = poids;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
            }

            assert(cntCouplante >= 0);

            constraintNamer.UpdateTimeStep(jour);
            constraintNamer.BindingConstraintDay(
              ProblemeAResoudre->NombreDeContraintes,
              MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante);
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre,
              Pi,
              Colonne,
              nombreDeTermes,
              MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante);
            pdtDebut += nombreDePasDeTempsDUneJournee;
        }
    }

    if (nombreDePasDeTempsPourUneOptimisation > nombreDePasDeTempsDUneJournee)
    {
        int semaine = problemeHebdo->weekInTheYear;
        for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
              = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];
            if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante
                != CONTRAINTE_HEBDOMADAIRE)
                continue;

            int nbInterco
              = MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante;
            int nombreDeTermes = 0;
            for (int index = 0; index < nbInterco; index++)
            {
                int interco = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
                double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
                int Offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];
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
                            .NumeroDeVariableDeLInterconnexion[interco];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = poids;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
            }

            int nbClusters
              = MatriceDesContraintesCouplantes.NombreDePaliersDispatchDansLaContrainteCouplante;
            for (int index = 0; index < nbClusters; index++)
            {
                int pays = MatriceDesContraintesCouplantes.PaysDuPalierDispatch[index];
                const PALIERS_THERMIQUES& PaliersThermiquesDuPays
                  = problemeHebdo->PaliersThermiquesDuPays[pays];
                const int palier
                  = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                      [MatriceDesContraintesCouplantes.NumeroDuPalierDispatch[index]];
                double poids = MatriceDesContraintesCouplantes.PoidsDuPalierDispatch[index];
                int Offset
                  = MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch[index];
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
                            .NumeroDeVariableDuPalierThermique[palier];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = poids;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
            }

            constraintNamer.UpdateTimeStep(semaine);
            constraintNamer.BindingConstraintWeek(
              ProblemeAResoudre->NombreDeContraintes,
              MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante);
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre,
              Pi,
              Colonne,
              nombreDeTermes,
              MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante);
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        constraintNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        bool presenceHydro
          = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable;
        bool TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes;
        bool presencePompage
          = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable;

        int nombreDeTermes = 0;
        if (presenceHydro && !TurbEntreBornes)
        {
            if (presencePompage)
            {
                for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
                {
                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt]
                            .NumeroDeVariablesDeLaProdHyd[pays];

                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = 1.0;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt]
                            .NumeroDeVariablesDePompage[pays];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes]
                          = problemeHebdo->CaracteristiquesHydrauliques[pays].PumpingRatio;
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
                            .NumeroDeVariablesDeLaProdHyd[pays];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = 1.0;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
            }

            constraintNamer.UpdateTimeStep(problemeHebdo->weekInTheYear);
            constraintNamer.HydroPower(ProblemeAResoudre->NombreDeContraintes);
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=');
        }
    }

    if (problemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
    {
        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            if (!problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
                continue;

            constraintNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                int timeStepInYear = problemeHebdo->weekInTheYear * 168 + pdt;
                constraintNamer.UpdateTimeStep(timeStepInYear);
                const auto& CorrespondanceVarNativesVarOptim
                  = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
                int nombreDeTermes = 0;
                var = CorrespondanceVarNativesVarOptim.NumeroDeVariablesDeLaProdHyd[pays];
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
                                 .NumeroDeVariablesDeLaProdHyd[pays];
                    var1 >= 0)
                {
                    Pi[nombreDeTermes] = -1.0;
                    Colonne[nombreDeTermes] = var1;
                    nombreDeTermes++;
                }

                if (int var2 = CorrespondanceVarNativesVarOptim
                                 .NumeroDeVariablesVariationHydALaBaisse[pays];
                    var2 >= 0)
                {
                    Pi[nombreDeTermes] = -1.0;
                    Colonne[nombreDeTermes] = var2;
                    nombreDeTermes++;
                }

                if (int var3 = CorrespondanceVarNativesVarOptim
                                 .NumeroDeVariablesVariationHydALaHausse[pays];
                    var3 >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var3;
                    nombreDeTermes++;
                }

                constraintNamer.HydroPowerSmoothingUsingVariationSum(
                  ProblemeAResoudre->NombreDeContraintes);
                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=');
            }
        }
    }
    else if (problemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
    {
        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            if (!problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
                continue;

            constraintNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                int timeStepInYear = problemeHebdo->weekInTheYear * 168 + pdt;
                constraintNamer.UpdateTimeStep(timeStepInYear);
                const auto& CorrespondanceVarNativesVarOptim
                  = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
                int nombreDeTermes = 0;
                var = CorrespondanceVarNativesVarOptim.NumeroDeVariablesDeLaProdHyd[pays];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
                int var1 = problemeHebdo->CorrespondanceVarNativesVarOptim[0]
                             .NumeroDeVariablesVariationHydALaBaisse[pays];
                if (var1 >= 0)
                {
                    Pi[nombreDeTermes] = -1.0;
                    Colonne[nombreDeTermes] = var1;
                    nombreDeTermes++;
                }

                constraintNamer.HydroPowerSmoothingUsingVariationMaxDown(
                  ProblemeAResoudre->NombreDeContraintes);
                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '<');

                nombreDeTermes = 0;
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
                var1 = problemeHebdo->CorrespondanceVarNativesVarOptim[0]
                         .NumeroDeVariablesVariationHydALaHausse[pays];
                if (var1 >= 0)
                {
                    Pi[nombreDeTermes] = -1.0;
                    Colonne[nombreDeTermes] = var1;
                    nombreDeTermes++;
                }

                constraintNamer.HydroPowerSmoothingUsingVariationMaxUp(
                  ProblemeAResoudre->NombreDeContraintes);
                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '>');
            }
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const bool presenceHydro
          = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable;
        const bool presencePompage
          = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable;
        const bool TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes;
        constraintNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        if (presenceHydro && (TurbEntreBornes || presencePompage))
        {
            int nombreDeTermes = 0;
            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt]
                        .NumeroDeVariablesDeLaProdHyd[pays];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
            }

            constraintNamer.UpdateTimeStep(problemeHebdo->weekInTheYear);
            constraintNamer.MinHydroPower(ProblemeAResoudre->NombreDeContraintes);
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '>');
        }

        if (presenceHydro
            && (TurbEntreBornes
            || problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable))
        {
            int nombreDeTermes = 0;
            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                constraintNamer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
                var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt]
                        .NumeroDeVariablesDeLaProdHyd[pays];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
            }

            constraintNamer.UpdateTimeStep(problemeHebdo->weekInTheYear);
            constraintNamer.MaxHydroPower(ProblemeAResoudre->NombreDeContraintes);
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '<');
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        constraintNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable)
        {
            int nombreDeTermes = 0;
            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt]
                        .NumeroDeVariablesDePompage[pays];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }
            }

            constraintNamer.UpdateTimeStep(problemeHebdo->weekInTheYear);
            constraintNamer.MaxPumping(ProblemeAResoudre->NombreDeContraintes);
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '<');
        }
    }

    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        const auto& CorrespondanceVarNativesVarOptim = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];

        int timeStepInYear = problemeHebdo->weekInTheYear * 168 + pdt;
        constraintNamer.UpdateTimeStep(timeStepInYear);
        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            constraintNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            if (problemeHebdo->CaracteristiquesHydrauliques[pays].SuiviNiveauHoraire)
            {
                int nombreDeTermes = 0;

                var = CorrespondanceVarNativesVarOptim.NumeroDeVariablesDeNiveau[pays];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }

                if (pdt > 0)
                {
                    int var1 = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt - 1]
                                 .NumeroDeVariablesDeNiveau[pays];
                    if (var1 >= 0)
                    {
                        Pi[nombreDeTermes] = -1.0;
                        Colonne[nombreDeTermes] = var1;
                        nombreDeTermes++;
                    }
                }

                var = CorrespondanceVarNativesVarOptim.NumeroDeVariablesDeLaProdHyd[pays];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.0;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }

                var = CorrespondanceVarNativesVarOptim.NumeroDeVariablesDePompage[pays];
                if (var >= 0)
                {
                    Pi[nombreDeTermes]
                      = problemeHebdo->CaracteristiquesHydrauliques[pays].PumpingRatio;
                    Pi[nombreDeTermes] *= -1;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }

                var = CorrespondanceVarNativesVarOptim.NumeroDeVariablesDeDebordement[pays];
                if (var >= 0)
                {
                    Pi[nombreDeTermes] = 1.;
                    Colonne[nombreDeTermes] = var;
                    nombreDeTermes++;
                }

                constraintNamer.AreaHydroLevel(ProblemeAResoudre->NombreDeContraintes);
                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=');
            }
        }
    }

    /* For each area with ad hoc properties, two possible sets of two additional constraints */
    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const auto& week = problemeHebdo->weekInTheYear;
        constraintNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        constraintNamer.UpdateTimeStep(week * 168 + nombreDePasDeTempsPourUneOptimisation - 1);

        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue
            && problemeHebdo->CaracteristiquesHydrauliques[pays].DirectLevelAccess)
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
                    .NumeroDeVariablesDeNiveau[pays];
            if (var >= 0)
            {
                Pi[nombreDeTermes] = -1.0;
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;
            }

            constraintNamer.FinalStockEquivalent(ProblemeAResoudre->NombreDeContraintes);
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=');
        }
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
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

            constraintNamer.FinalStockExpression(ProblemeAResoudre->NombreDeContraintes);
            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=');
        }
    }

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    {
        OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaireCoutsDeDemarrage(problemeHebdo,
                                                                                false);
    }

    // Export structure
    if (exportStructure && firstWeekOfSimulation)
    {
        OPT_ExportInterco(writer, problemeHebdo);
        OPT_ExportAreaName(writer, problemeHebdo->NomsDesPays);
    }

    return;
}
