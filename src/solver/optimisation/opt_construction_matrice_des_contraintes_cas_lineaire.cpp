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
#include <math.h>
#include <sstream>

#include "opt_structure_probleme_a_resoudre.h"
#include "opt_export_structure.h"

#include "../simulation/simulation.h"
#include "../utils/filename.h"
#include "opt_fonctions.h"

#include <antares/study.h>

using namespace Antares::Data;

void OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaire(PROBLEME_HEBDO* problemeHebdo)
{
    int Interco;
    int Index;
    int Pays;
    int Pdt;
    int var;
    int Palier;
    int layerindex;
    int NbInterco, NbClusters;
    int CntCouplante;
    double Poids;
    int NombreDeTermes;
    int PdtDebut;
    int NombreDePasDeTempsDUneJournee;
    int NombreDePasDeTempsPourUneOptimisation;
    int Jour;
    int Semaine;
    int* NumeroDeJourDuPasDeTemps;
    int Pdt1;
    int var1;
    int var2;
    int var3;
    int Offset;
    char Simulation;

    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    CORRESPONDANCES_DES_CONTRAINTES* CorrespondanceCntNativesCntOptim;
    PALIERS_THERMIQUES* PaliersThermiquesDuPays;
    COUTS_DE_TRANSPORT* CoutDeTransport;
    CONTRAINTES_COUPLANTES* MatriceDesContraintesCouplantes;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;

    CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES* CorrespondanceCntNativesCntOptimJournalieres;
    CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES* CorrespondanceCntNativesCntOptimHebdomadaires;

    double* Pi;
    int* Colonne;

    std::string NomDeLaContrainte;

    Study::Ptr study = Study::Current::Get();
    const bool exportStructure = problemeHebdo->ExportStructure;
    const bool firstWeekOfSimulation = problemeHebdo->firstWeekOfSimulation;

    ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    NombreDePasDeTempsDUneJournee = problemeHebdo->NombreDePasDeTempsDUneJournee;
    NombreDePasDeTempsPourUneOptimisation = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
    NumeroDeJourDuPasDeTemps = problemeHebdo->NumeroDeJourDuPasDeTemps;

    Pi = ProblemeAResoudre->Pi;
    Colonne = ProblemeAResoudre->Colonne;

    ProblemeAResoudre->NombreDeContraintes = 0;
    ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;

    // For now only variable are exported, can't define name for constraints export
    int nvars = ProblemeAResoudre->NombreDeVariables;
    std::vector<std::string> varname;
    varname.assign(nvars, "");

    for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
    {
        int timeStepInYear = problemeHebdo->weekInTheYear * 168 + Pdt;

        CorrespondanceVarNativesVarOptim = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];
        CorrespondanceCntNativesCntOptim = problemeHebdo->CorrespondanceCntNativesCntOptim[Pdt];

        for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
        {
            NombreDeTermes = 0;

            Interco = problemeHebdo->IndexDebutIntercoOrigine[Pays];
            while (Interco >= 0)
            {
                var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
                if (var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;

                    if (exportStructure)
                    {
                        OPT_Export_add_variable(
                          varname,
                          var,
                          Enum::ExportStructDict::ValeurDeNTCOrigineVersExtremite,
                          timeStepInYear, // TODO[FOM] remove
                          Pays,
                          Interco);
                    }
                }
                Interco = problemeHebdo->IndexSuivantIntercoOrigine[Interco];
            }
            Interco = problemeHebdo->IndexDebutIntercoExtremite[Pays];
            while (Interco >= 0)
            {
                var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
                if (var >= 0)
                {
                    Pi[NombreDeTermes] = -1.0;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;

                    if (exportStructure)
                    {
                        OPT_Export_add_variable(
                          varname,
                          var,
                          Enum::ExportStructDict::ValeurDeNTCOrigineVersExtremite,
                          timeStepInYear, // TODO[FOM] remove
                          Pays,
                          Interco);
                    }
                }
                Interco = problemeHebdo->IndexSuivantIntercoExtremite[Interco];
            }

            PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[Pays];
            for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++)
            {
                Palier
                  = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
                var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[Palier];
                if (var >= 0)
                {
                    Pi[NombreDeTermes] = -1.0;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;

                    if (exportStructure)
                    {
                        OPT_Export_add_variable(varname,
                                                var,
                                                Enum::ExportStructDict::PalierThermique,
                                                timeStepInYear, // TODO[FOM] remove
                                                Pays,
                                                Palier);
                    }
                }
            }

            var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
            if (var >= 0)
            {
                Pi[NombreDeTermes] = -1.0;
                Colonne[NombreDeTermes] = var;
                NombreDeTermes++;

                if (exportStructure)
                {
                    OPT_Export_add_variable(
                      varname, var, Enum::ExportStructDict::ProdHyd, timeStepInYear, Pays);
                }
            }

            var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDePompage[Pays];
            if (var >= 0)
            {
                Pi[NombreDeTermes] = 1.0;
                Colonne[NombreDeTermes] = var;
                NombreDeTermes++;
            }

            var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[Pays];
            if (var >= 0)
            {
                Pi[NombreDeTermes] = -1.0;
                Colonne[NombreDeTermes] = var;
                NombreDeTermes++;

                if (exportStructure)
                {
                    OPT_Export_add_variable(varname,
                                            var,
                                            Enum::ExportStructDict::DefaillancePositive,
                                            timeStepInYear, // TODO[FOM] remove
                                            Pays);
                }
            }
            var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[Pays];
            if (var >= 0)
            {
                Pi[NombreDeTermes] = 1.0;
                Colonne[NombreDeTermes] = var;
                NombreDeTermes++;

                if (exportStructure)
                {
                    OPT_Export_add_variable(varname,
                                            var,
                                            Enum::ExportStructDict::DefaillanceNegative,
                                            timeStepInYear, // TODO[FOM] remove
                                            Pays);
                }
            }

            CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesBilansPays[Pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=');

            NombreDeTermes = 0;

            PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[Pays];
            for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++)
            {
                Palier
                  = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
                var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[Palier];
                if (var >= 0)
                {
                    Pi[NombreDeTermes] = -1.0;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;

                    if (exportStructure)
                    {
                        OPT_Export_add_variable(varname,
                                                var,
                                                Enum::ExportStructDict::PalierThermique,
                                                timeStepInYear, // TODO[FOM] remove
                                                Pays,
                                                Palier);
                    }
                }
            }

            var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
            if (var >= 0)
            {
                Pi[NombreDeTermes] = -problemeHebdo->DefaillanceNegativeUtiliserHydro[Pays];
                Colonne[NombreDeTermes] = var;
                NombreDeTermes++;

                if (exportStructure)
                {
                    OPT_Export_add_variable(
                      varname, var, Enum::ExportStructDict::ProdHyd, timeStepInYear, Pays);
                }
            }

            var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[Pays];
            if (var >= 0)
            {
                Pi[NombreDeTermes] = 1.0;
                Colonne[NombreDeTermes] = var;
                NombreDeTermes++;

                if (exportStructure)
                {
                    OPT_Export_add_variable(varname,
                                            var,
                                            Enum::ExportStructDict::DefaillanceNegative,
                                            timeStepInYear, // TODO[FOM] remove
                                            Pays);
                }
            }

            CorrespondanceCntNativesCntOptim->NumeroDeContraintePourEviterLesChargesFictives[Pays]
              = ProblemeAResoudre->NombreDeContraintes;

            NomDeLaContrainte = "fict_load::" + std::to_string(timeStepInYear + 1)
                                + "::" + problemeHebdo->NomsDesPays[Pays];

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '<', NomDeLaContrainte);
        }

        for (Interco = 0; Interco < problemeHebdo->NombreDInterconnexions; Interco++)
        {
            CoutDeTransport = problemeHebdo->CoutDeTransport[Interco];
            if (CoutDeTransport->IntercoGereeAvecDesCouts == OUI_ANTARES)
            {
                NombreDeTermes = 0;
                var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
                if (var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;

                    if (exportStructure)
                    {
                        OPT_Export_add_variable(varname,
                                                var,
                                                Enum::ExportStructDict::DefaillanceNegative,
                                                timeStepInYear, // TODO[FOM] remove
                                                Pays);
                    }
                }
                var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco];
                if (var >= 0)
                {
                    Pi[NombreDeTermes] = -1.0;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;

                    if (exportStructure)
                    {
                        OPT_Export_add_variable(
                          varname,
                          var,
                          Enum::ExportStructDict::CoutOrigineVersExtremiteDeLInterconnexion,
                          timeStepInYear, // TODO[FOM] remove
                          Interco);
                    }
                }
                var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco];
                if (var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;

                    if (exportStructure)
                    {
                        OPT_Export_add_variable(
                          varname,
                          var,
                          Enum::ExportStructDict::CoutExtremiteVersOrigineDeLInterconnexion,
                          timeStepInYear, // TODO[FOM] remove
                          Interco);
                    }
                }

                CorrespondanceCntNativesCntOptim->NumeroDeContrainteDeDissociationDeFlux[Interco]
                  = ProblemeAResoudre->NombreDeContraintes;

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=');
            }
        }

        for (CntCouplante = 0; CntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             CntCouplante++)
        {
            MatriceDesContraintesCouplantes
              = problemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];

            if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante == CONTRAINTE_HORAIRE)
            {
                NbInterco = MatriceDesContraintesCouplantes
                              ->NombreDInterconnexionsDansLaContrainteCouplante;
                NombreDeTermes = 0;
                for (Index = 0; Index < NbInterco; Index++)
                {
                    Interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[Index];
                    Poids = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[Index];
                    Offset = MatriceDesContraintesCouplantes->OffsetTemporelSurLInterco[Index];

                    if (Offset >= 0)
                    {
                        Pdt1 = (Pdt + Offset) % NombreDePasDeTempsPourUneOptimisation;
                    }
                    else
                    {
                        Pdt1 = (Pdt + Offset + problemeHebdo->NombreDePasDeTemps)
                               % NombreDePasDeTempsPourUneOptimisation;
                    }

                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]
                            ->NumeroDeVariableDeLInterconnexion[Interco];

                    if (var >= 0)
                    {
                        Pi[NombreDeTermes] = Poids;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;
                    }
                }

                NbClusters = MatriceDesContraintesCouplantes
                               ->NombreDePaliersDispatchDansLaContrainteCouplante;
                for (Index = 0; Index < NbClusters; Index++)
                {
                    Pays = MatriceDesContraintesCouplantes->PaysDuPalierDispatch[Index];
                    PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[Pays];
                    Palier
                      = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques
                          [MatriceDesContraintesCouplantes->NumeroDuPalierDispatch[Index]];
                    Poids = MatriceDesContraintesCouplantes->PoidsDuPalierDispatch[Index];
                    Offset
                      = MatriceDesContraintesCouplantes->OffsetTemporelSurLePalierDispatch[Index];

                    if (Offset >= 0)
                    {
                        Pdt1 = (Pdt + Offset) % NombreDePasDeTempsPourUneOptimisation;
                    }
                    else
                    {
                        Pdt1 = (Pdt + Offset + problemeHebdo->NombreDePasDeTemps)
                               % NombreDePasDeTempsPourUneOptimisation;
                    }

                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]
                            ->NumeroDeVariableDuPalierThermique[Palier];

                    if (var >= 0)
                    {
                        Pi[NombreDeTermes] = Poids;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;

                        if (exportStructure)
                        {
                            OPT_Export_add_variable(
                              varname,
                              var,
                              Enum::ExportStructDict::CorrespondanceVarNativesVarOptim,
                              Pdt1, // TODO[FOM] remove
                              Palier);
                        }
                    }
                }

                CorrespondanceCntNativesCntOptim
                  ->NumeroDeContrainteDesContraintesCouplantes[CntCouplante]
                  = ProblemeAResoudre->NombreDeContraintes;

                NomDeLaContrainte = "bc::hourly::" + std::to_string(timeStepInYear + 1) + "::"
                                    + MatriceDesContraintesCouplantes->NomDeLaContrainteCouplante;

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre,
                  Pi,
                  Colonne,
                  NombreDeTermes,
                  MatriceDesContraintesCouplantes->SensDeLaContrainteCouplante,
                  NomDeLaContrainte);
            }
        }
    }

    for (CntCouplante = 0; CntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         CntCouplante++)
    {
        MatriceDesContraintesCouplantes
          = problemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
        if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante == CONTRAINTE_JOURNALIERE)
        {
            NbInterco
              = MatriceDesContraintesCouplantes->NombreDInterconnexionsDansLaContrainteCouplante;
            NbClusters
              = MatriceDesContraintesCouplantes->NombreDePaliersDispatchDansLaContrainteCouplante;
            PdtDebut = 0;
            while (PdtDebut < NombreDePasDeTempsPourUneOptimisation)
            {
                Jour = NumeroDeJourDuPasDeTemps[PdtDebut];
                CorrespondanceCntNativesCntOptimJournalieres
                  = problemeHebdo->CorrespondanceCntNativesCntOptimJournalieres[Jour];
                NombreDeTermes = 0;

                for (Index = 0; Index < NbInterco; Index++)
                {
                    Interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[Index];
                    Poids = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[Index];
                    Offset = MatriceDesContraintesCouplantes->OffsetTemporelSurLInterco[Index];

                    for (Pdt = PdtDebut; Pdt < PdtDebut + NombreDePasDeTempsDUneJournee; Pdt++)
                    {
                        if (Offset >= 0)
                        {
                            Pdt1 = (Pdt + Offset) % NombreDePasDeTempsPourUneOptimisation;
                        }
                        else
                        {
                            Pdt1 = (Pdt + Offset + problemeHebdo->NombreDePasDeTemps)
                                   % NombreDePasDeTempsPourUneOptimisation;
                        }

                        var = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]
                                ->NumeroDeVariableDeLInterconnexion[Interco];
                        if (var >= 0)
                        {
                            Pi[NombreDeTermes] = Poids;
                            Colonne[NombreDeTermes] = var;
                            NombreDeTermes++;
                        }
                    }
                }

                for (Index = 0; Index < NbClusters; Index++)
                {
                    Pays = MatriceDesContraintesCouplantes->PaysDuPalierDispatch[Index];
                    PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[Pays];
                    Palier
                      = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques
                          [MatriceDesContraintesCouplantes->NumeroDuPalierDispatch[Index]];
                    Poids = MatriceDesContraintesCouplantes->PoidsDuPalierDispatch[Index];
                    Offset
                      = MatriceDesContraintesCouplantes->OffsetTemporelSurLePalierDispatch[Index];

                    for (Pdt = PdtDebut; Pdt < PdtDebut + NombreDePasDeTempsDUneJournee; Pdt++)
                    {
                        if (Offset >= 0)
                        {
                            Pdt1 = (Pdt + Offset) % NombreDePasDeTempsPourUneOptimisation;
                        }
                        else
                        {
                            Pdt1 = (Pdt + Offset + problemeHebdo->NombreDePasDeTemps)
                                   % NombreDePasDeTempsPourUneOptimisation;
                        }

                        var = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]
                                ->NumeroDeVariableDuPalierThermique[Palier];
                        if (var >= 0)
                        {
                            Pi[NombreDeTermes] = Poids;
                            Colonne[NombreDeTermes] = var;
                            NombreDeTermes++;
                        }
                    }
                }

                assert(CntCouplante >= 0);
                CorrespondanceCntNativesCntOptimJournalieres
                  ->NumeroDeContrainteDesContraintesCouplantes[CntCouplante]
                  = ProblemeAResoudre->NombreDeContraintes;

                NomDeLaContrainte = "bc::daily::" + std::to_string(Jour + 1) + "::"
                                    + MatriceDesContraintesCouplantes->NomDeLaContrainteCouplante;

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre,
                  Pi,
                  Colonne,
                  NombreDeTermes,
                  MatriceDesContraintesCouplantes->SensDeLaContrainteCouplante,
                  NomDeLaContrainte);
                PdtDebut += NombreDePasDeTempsDUneJournee;
            }
        }
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
                NbInterco = MatriceDesContraintesCouplantes
                              ->NombreDInterconnexionsDansLaContrainteCouplante;
                NombreDeTermes = 0;
                for (Index = 0; Index < NbInterco; Index++)
                {
                    Interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[Index];
                    Poids = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[Index];
                    Offset = MatriceDesContraintesCouplantes->OffsetTemporelSurLInterco[Index];
                    for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
                    {
                        if (Offset >= 0)
                        {
                            Pdt1 = (Pdt + Offset) % NombreDePasDeTempsPourUneOptimisation;
                        }
                        else
                        {
                            Pdt1 = (Pdt + Offset + problemeHebdo->NombreDePasDeTemps)
                                   % NombreDePasDeTempsPourUneOptimisation;
                        }

                        var = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]
                                ->NumeroDeVariableDeLInterconnexion[Interco];
                        if (var >= 0)
                        {
                            Pi[NombreDeTermes] = Poids;
                            Colonne[NombreDeTermes] = var;
                            NombreDeTermes++;
                        }
                    }
                }

                NbClusters = MatriceDesContraintesCouplantes
                               ->NombreDePaliersDispatchDansLaContrainteCouplante;
                for (Index = 0; Index < NbClusters; Index++)
                {
                    Pays = MatriceDesContraintesCouplantes->PaysDuPalierDispatch[Index];
                    PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[Pays];
                    Palier
                      = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques
                          [MatriceDesContraintesCouplantes->NumeroDuPalierDispatch[Index]];
                    Poids = MatriceDesContraintesCouplantes->PoidsDuPalierDispatch[Index];
                    Offset
                      = MatriceDesContraintesCouplantes->OffsetTemporelSurLePalierDispatch[Index];
                    for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
                    {
                        if (Offset >= 0)
                        {
                            Pdt1 = (Pdt + Offset) % NombreDePasDeTempsPourUneOptimisation;
                        }
                        else
                        {
                            Pdt1 = (Pdt + Offset + problemeHebdo->NombreDePasDeTemps)
                                   % NombreDePasDeTempsPourUneOptimisation;
                        }

                        var = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]
                                ->NumeroDeVariableDuPalierThermique[Palier];
                        if (var >= 0)
                        {
                            Pi[NombreDeTermes] = Poids;
                            Colonne[NombreDeTermes] = var;
                            NombreDeTermes++;
                        }
                    }
                }

                CorrespondanceCntNativesCntOptimHebdomadaires
                  ->NumeroDeContrainteDesContraintesCouplantes[CntCouplante]
                  = ProblemeAResoudre->NombreDeContraintes;

                NomDeLaContrainte = std::string("bc::weekly::")
                                    + MatriceDesContraintesCouplantes->NomDeLaContrainteCouplante;

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre,
                  Pi,
                  Colonne,
                  NombreDeTermes,
                  MatriceDesContraintesCouplantes->SensDeLaContrainteCouplante,
                  NomDeLaContrainte);
            }
        }
    }

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        char presenceHydro
          = problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable;
        char TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[Pays]->TurbinageEntreBornes;
        char presencePompage
          = problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDePompageModulable;
        if (presenceHydro == OUI_ANTARES && TurbEntreBornes == NON_ANTARES)
        {
            NombreDeTermes = 0;
            if (presencePompage == NON_ANTARES)
            {
                for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
                {
                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                            ->NumeroDeVariablesDeLaProdHyd[Pays];
                    if (var >= 0)
                    {
                        Pi[NombreDeTermes] = 1.0;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;
                    }
                }
            }
            if (presencePompage == OUI_ANTARES)
            {
                for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
                {
                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                            ->NumeroDeVariablesDeLaProdHyd[Pays];
                    if (var >= 0)
                    {
                        Pi[NombreDeTermes] = 1.0;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;
                    }
                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                            ->NumeroDeVariablesDePompage[Pays];
                    if (var >= 0)
                    {
                        Pi[NombreDeTermes]
                          = problemeHebdo->CaracteristiquesHydrauliques[Pays]->PumpingRatio;
                        Pi[NombreDeTermes] *= -1.0;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;
                    }
                }
            }

            problemeHebdo->NumeroDeContrainteEnergieHydraulique[Pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=');
        }
        else
            problemeHebdo->NumeroDeContrainteEnergieHydraulique[Pays] = -1;
    }

    if (problemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
    {
        for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
        {
            if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable
                != OUI_ANTARES)
                continue;

            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                CorrespondanceVarNativesVarOptim
                  = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];
                NombreDeTermes = 0;
                var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
                if (var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;
                }
                Pdt1 = Pdt + 1;
                if (Pdt1 >= NombreDePasDeTempsPourUneOptimisation)
                    Pdt1 = 0;
                var1 = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]
                         ->NumeroDeVariablesDeLaProdHyd[Pays];
                if (var1 >= 0)
                {
                    Pi[NombreDeTermes] = -1.0;
                    Colonne[NombreDeTermes] = var1;
                    NombreDeTermes++;
                }
                var2
                  = CorrespondanceVarNativesVarOptim->NumeroDeVariablesVariationHydALaBaisse[Pays];
                if (var2 >= 0)
                {
                    Pi[NombreDeTermes] = -1.0;
                    Colonne[NombreDeTermes] = var2;
                    NombreDeTermes++;
                }
                var3
                  = CorrespondanceVarNativesVarOptim->NumeroDeVariablesVariationHydALaHausse[Pays];
                if (var3 >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = var3;
                    NombreDeTermes++;
                }

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=');
            }
        }
    }
    else if (problemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
    {
        for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
        {
            if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable
                != OUI_ANTARES)
                continue;

            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                CorrespondanceVarNativesVarOptim
                  = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];
                NombreDeTermes = 0;
                var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
                if (var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;
                }
                var1 = problemeHebdo->CorrespondanceVarNativesVarOptim[0]
                         ->NumeroDeVariablesVariationHydALaBaisse[Pays];
                if (var1 >= 0)
                {
                    Pi[NombreDeTermes] = -1.0;
                    Colonne[NombreDeTermes] = var1;
                    NombreDeTermes++;
                }

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '<');

                NombreDeTermes = 0;
                if (var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;
                }
                var1 = problemeHebdo->CorrespondanceVarNativesVarOptim[0]
                         ->NumeroDeVariablesVariationHydALaHausse[Pays];
                if (var1 >= 0)
                {
                    Pi[NombreDeTermes] = -1.0;
                    Colonne[NombreDeTermes] = var1;
                    NombreDeTermes++;
                }

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '>');
            }
        }
    }

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        const char presenceHydro
          = problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable;
        const char presencePompage
          = problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDePompageModulable;
        const char TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[Pays]->TurbinageEntreBornes;
        if (presenceHydro == OUI_ANTARES
            && (TurbEntreBornes == OUI_ANTARES || presencePompage == OUI_ANTARES))
        {
            NombreDeTermes = 0;
            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                var = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                        ->NumeroDeVariablesDeLaProdHyd[Pays];
                if (var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;
                }
            }

            problemeHebdo->NumeroDeContrainteMinEnergieHydraulique[Pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '>');
        }
        else
            problemeHebdo->NumeroDeContrainteMinEnergieHydraulique[Pays] = -1;

        if (presenceHydro == OUI_ANTARES
            && (TurbEntreBornes == OUI_ANTARES
                || problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDePompageModulable
                     == OUI_ANTARES))
        {
            NombreDeTermes = 0;
            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                var = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                        ->NumeroDeVariablesDeLaProdHyd[Pays];
                if (var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;
                }
            }

            problemeHebdo->NumeroDeContrainteMaxEnergieHydraulique[Pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '<');
        }
        else
            problemeHebdo->NumeroDeContrainteMaxEnergieHydraulique[Pays] = -1;
    }

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDePompageModulable
            == OUI_ANTARES)
        {
            NombreDeTermes = 0;
            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                var = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                        ->NumeroDeVariablesDePompage[Pays];
                if (var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;
                }
            }

            problemeHebdo->NumeroDeContrainteMaxPompage[Pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '<');
        }
        else
            problemeHebdo->NumeroDeContrainteMaxPompage[Pays] = -1;
    }

    for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
    {
        CorrespondanceVarNativesVarOptim = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];
        CorrespondanceCntNativesCntOptim = problemeHebdo->CorrespondanceCntNativesCntOptim[Pdt];

        int timeStepInYear = problemeHebdo->weekInTheYear * 168 + Pdt;

        for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
        {
            if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->SuiviNiveauHoraire
                == OUI_ANTARES)
            {
                NombreDeTermes = 0;

                var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeNiveau[Pays];
                if (var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;
                }

                if (Pdt > 0)
                {
                    var1 = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt - 1]
                             ->NumeroDeVariablesDeNiveau[Pays];
                    if (var1 >= 0)
                    {
                        Pi[NombreDeTermes] = -1.0;
                        Colonne[NombreDeTermes] = var1;
                        NombreDeTermes++;
                    }
                }

                var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
                if (var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;
                }

                var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDePompage[Pays];
                if (var >= 0)
                {
                    Pi[NombreDeTermes]
                      = problemeHebdo->CaracteristiquesHydrauliques[Pays]->PumpingRatio;
                    Pi[NombreDeTermes] *= -1;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;
                }

                var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeDebordement[Pays];
                if (var >= 0)
                {
                    Pi[NombreDeTermes] = 1.;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;
                }

                CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesNiveauxPays[Pays]
                  = ProblemeAResoudre->NombreDeContraintes;

                NomDeLaContrainte = "hydro_level::" + std::to_string(timeStepInYear + 1)
                                    + "::" + problemeHebdo->NomsDesPays[Pays];

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=', NomDeLaContrainte);
            }
            else
                CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesNiveauxPays[Pays] = -1;
        }
    }

    /* For each area with ad hoc properties, two possible sets of two additional constraints */
    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->AccurateWaterValue == OUI_ANTARES
            && problemeHebdo->CaracteristiquesHydrauliques[Pays]->DirectLevelAccess == OUI_ANTARES)
        /*  equivalence constraint : StockFinal- Niveau[T]= 0*/
        {
            NombreDeTermes = 0;
            var = problemeHebdo->NumeroDeVariableStockFinal[Pays];
            if (var >= 0)
            {
                Pi[NombreDeTermes] = 1.0;
                Colonne[NombreDeTermes] = var;
                NombreDeTermes++;
            }
            var = problemeHebdo
                    ->CorrespondanceVarNativesVarOptim[NombreDePasDeTempsPourUneOptimisation - 1]
                    ->NumeroDeVariablesDeNiveau[Pays];
            if (var >= 0)
            {
                Pi[NombreDeTermes] = -1.0;
                Colonne[NombreDeTermes] = var;
                NombreDeTermes++;
            }
            problemeHebdo->NumeroDeContrainteEquivalenceStockFinal[Pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=');
        }
        if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->AccurateWaterValue == OUI_ANTARES)
        /*  expression constraint : - StockFinal +sum (stocklayers) = 0*/
        {
            NombreDeTermes = 0;
            var = problemeHebdo->NumeroDeVariableStockFinal[Pays];
            if (var >= 0)
            {
                Pi[NombreDeTermes] = -1.0;
                Colonne[NombreDeTermes] = var;
                NombreDeTermes++;
            }
            for (layerindex = 0; layerindex < 100; layerindex++)
            {
                var = problemeHebdo->NumeroDeVariableDeTrancheDeStock[Pays][layerindex];

                if (var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;
                }
            }

            problemeHebdo->NumeroDeContrainteExpressionStockFinal[Pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=');
        }
    }

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage == OUI_ANTARES)
    {
        Simulation = NON_ANTARES;
        OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaireCoutsDeDemarrage(problemeHebdo,
                                                                                Simulation);
    }

    // Export structure
    if (exportStructure)
    {
        if (firstWeekOfSimulation)
        {
            OPT_ExportInterco(study->resultWriter, problemeHebdo);
            OPT_ExportAreaName(study->resultWriter, study->areas);
        }

        const auto filename = getFilenameWithExtension(
          "variables", "txt", problemeHebdo->year, problemeHebdo->weekInTheYear, 0);
        OPT_ExportVariables(study->resultWriter, varname, filename);
    }

    return;
}
