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
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_structure_probleme_economique.h"
#include "../simulation/sim_structure_probleme_adequation.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"

#include <antares/study.h>
#include <antares/study/area/scratchpad.h>
#include "../simulation/sim_structure_donnees.h"

using namespace Antares::Data;

void OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaire(PROBLEME_HEBDO* ProblemeHebdo,
                                                             uint numSpace)
{
    int Interco;
    int Index;
    int Pays;
    int Pdt;
    int Var;
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
    int Var1;
    int Var2;
    int Var3;
    int Offset;
    int Zone;
    int NombreDeZonesDeReserveJMoins1;
    int* NumeroDeZoneDeReserveJMoins1;
    char ContrainteDeReserveJMoins1ParZone;
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
    bool exportStructure = ProblemeHebdo->ExportStructure;

    if (exportStructure)
    {
        OPT_ExportInterco(*study, ProblemeHebdo, numSpace);
        OPT_ExportAreaName(*study, ProblemeHebdo, numSpace);
    }

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    NombreDeZonesDeReserveJMoins1 = ProblemeHebdo->NombreDeZonesDeReserveJMoins1;
    NumeroDeZoneDeReserveJMoins1 = ProblemeHebdo->NumeroDeZoneDeReserveJMoins1;
    ContrainteDeReserveJMoins1ParZone = ProblemeHebdo->ContrainteDeReserveJMoins1ParZone;

    NombreDePasDeTempsDUneJournee = ProblemeHebdo->NombreDePasDeTempsDUneJournee;
    NombreDePasDeTempsPourUneOptimisation = ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation;
    NumeroDeJourDuPasDeTemps = ProblemeHebdo->NumeroDeJourDuPasDeTemps;

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
        int timeStepInYear = study->runtime->weekInTheYear[numSpace] * 168 + Pdt;

        CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];
        CorrespondanceCntNativesCntOptim = ProblemeHebdo->CorrespondanceCntNativesCntOptim[Pdt];

        for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
        {
            NombreDeTermes = 0;

            Interco = ProblemeHebdo->IndexDebutIntercoOrigine[Pays];
            while (Interco >= 0)
            {
                Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;

                    if (exportStructure)
                        OPT_Export_add_variable(
                          varname,
                          Var,
                          Enum::ExportStructDict::ValeurDeNTCOrigineVersExtremite,
                          Pays,
                          Interco,
                          timeStepInYear);
                }
                Interco = ProblemeHebdo->IndexSuivantIntercoOrigine[Interco];
            }
            Interco = ProblemeHebdo->IndexDebutIntercoExtremite[Pays];
            while (Interco >= 0)
            {
                Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = -1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;

                    if (exportStructure)
                        OPT_Export_add_variable(
                          varname,
                          Var,
                          Enum::ExportStructDict::ValeurDeNTCOrigineVersExtremite,
                          Pays,
                          Interco,
                          timeStepInYear);
                }
                Interco = ProblemeHebdo->IndexSuivantIntercoExtremite[Interco];
            }

            PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
            for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++)
            {
                Palier
                  = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
                Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[Palier];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = -1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;

                    if (exportStructure)
                        OPT_Export_add_variable(
                          varname, Var, Enum::ExportStructDict::PalierThermique, Pays, Palier, timeStepInYear);
                }
            }

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
            if (Var >= 0)
            {
                Pi[NombreDeTermes] = -1.0;
                Colonne[NombreDeTermes] = Var;
                NombreDeTermes++;

                if (exportStructure)
                    OPT_Export_add_variable(
                      varname, Var, Enum::ExportStructDict::ProdHyd, Pays, timeStepInYear);
            }

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDePompage[Pays];
            if (Var >= 0)
            {
                Pi[NombreDeTermes] = 1.0;
                Colonne[NombreDeTermes] = Var;
                NombreDeTermes++;
            }

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[Pays];
            if (Var >= 0)
            {
                Pi[NombreDeTermes] = -1.0;
                Colonne[NombreDeTermes] = Var;
                NombreDeTermes++;

                if (exportStructure)
                    OPT_Export_add_variable(
                      varname, Var, Enum::ExportStructDict::DefaillancePositive, Pays, timeStepInYear);
            }
            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[Pays];
            if (Var >= 0)
            {
                Pi[NombreDeTermes] = 1.0;
                Colonne[NombreDeTermes] = Var;
                NombreDeTermes++;

                if (exportStructure)
                    OPT_Export_add_variable(
                      varname, Var, Enum::ExportStructDict::DefaillanceNegative, Pays, timeStepInYear);
            }

            CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesBilansPays[Pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=');

            NombreDeTermes = 0;

            PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
            for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++)
            {
                Palier
                  = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
                Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[Palier];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = -1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;

                    if (exportStructure)
                        OPT_Export_add_variable(
                          varname, Var, Enum::ExportStructDict::PalierThermique, Pays, Palier, timeStepInYear);
                }
            }

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
            if (Var >= 0)
            {
                Pi[NombreDeTermes] = -ProblemeHebdo->DefaillanceNegativeUtiliserHydro[Pays];
                Colonne[NombreDeTermes] = Var;
                NombreDeTermes++;

                if (exportStructure)
                    OPT_Export_add_variable(
                      varname, Var, Enum::ExportStructDict::ProdHyd, Pays, timeStepInYear);
            }

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[Pays];
            if (Var >= 0)
            {
                Pi[NombreDeTermes] = 1.0;
                Colonne[NombreDeTermes] = Var;
                NombreDeTermes++;

                if (exportStructure)
                    OPT_Export_add_variable(
                      varname, Var, Enum::ExportStructDict::DefaillanceNegative, Pays, timeStepInYear);
            }

            CorrespondanceCntNativesCntOptim->NumeroDeContraintePourEviterLesChargesFictives[Pays]
              = ProblemeAResoudre->NombreDeContraintes;

            NomDeLaContrainte
              = "fict_load::" + std::to_string(timeStepInYear+1) + "::" + ProblemeHebdo->NomsDesPays[Pays];

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '<', NomDeLaContrainte);
        }

        for (Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
        {
            CoutDeTransport = ProblemeHebdo->CoutDeTransport[Interco];
            if (CoutDeTransport->IntercoGereeAvecDesCouts == OUI_ANTARES)
            {
                NombreDeTermes = 0;
                Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;

                    if (exportStructure)
                        OPT_Export_add_variable(
                          varname, Var, Enum::ExportStructDict::DefaillanceNegative, Pays, timeStepInYear, timeStepInYear);
                }
                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = -1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;

                    if (exportStructure)
                        OPT_Export_add_variable(
                          varname,
                          Var,
                          Enum::ExportStructDict::CoutOrigineVersExtremiteDeLInterconnexion,
                          Interco,
                          timeStepInYear);
                }
                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;

                    if (exportStructure)
                        OPT_Export_add_variable(
                          varname,
                          Var,
                          Enum::ExportStructDict::CoutExtremiteVersOrigineDeLInterconnexion,
                          Interco,
                          timeStepInYear);
                }

                CorrespondanceCntNativesCntOptim->NumeroDeContrainteDeDissociationDeFlux[Interco]
                  = ProblemeAResoudre->NombreDeContraintes;

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=');
            }
        }

        for (CntCouplante = 0; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes;
             CntCouplante++)
        {
            MatriceDesContraintesCouplantes
              = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];

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
                        Pdt1 = (Pdt + Offset + ProblemeHebdo->NombreDePasDeTemps)
                               % NombreDePasDeTempsPourUneOptimisation;
                    }

                    Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]
                            ->NumeroDeVariableDeLInterconnexion[Interco];

                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes] = Poids;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;
                    }
                }

                NbClusters = MatriceDesContraintesCouplantes
                               ->NombreDePaliersDispatchDansLaContrainteCouplante;
                for (Index = 0; Index < NbClusters; Index++)
                {
                    Pays = MatriceDesContraintesCouplantes->PaysDuPalierDispatch[Index];
                    PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
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
                        Pdt1 = (Pdt + Offset + ProblemeHebdo->NombreDePasDeTemps)
                               % NombreDePasDeTempsPourUneOptimisation;
                    }

                    Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]
                            ->NumeroDeVariableDuPalierThermique[Palier];

                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes] = Poids;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;

                        if (exportStructure)
                            OPT_Export_add_variable(
                              varname,
                              Var,
                              Enum::ExportStructDict::CorrespondanceVarNativesVarOptim,
                              Palier,
                              Pdt1);
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

        if (ContrainteDeReserveJMoins1ParZone == OUI_ANTARES)
        {
            for (Zone = 0; Zone < NombreDeZonesDeReserveJMoins1; Zone++)
            {
                NombreDeTermes = 0;
                for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
                {
                    if (NumeroDeZoneDeReserveJMoins1[Pays] != Zone)
                        continue;

                    PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
                    for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques;
                         Index++)
                    {
                        Palier = PaliersThermiquesDuPays
                                   ->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
                        Var = CorrespondanceVarNativesVarOptim
                                ->NumeroDeVariableDuPalierThermique[Palier];
                        if (Var >= 0)
                        {
                            Pi[NombreDeTermes] = 1.0;
                            Colonne[NombreDeTermes] = Var;
                            NombreDeTermes++;
                        }
                    }

                    Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes] = 1.0;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;
                    }

                    Var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableDefaillanceEnReserve[Pays];
                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes] = 1.0;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;
                    }
                }

                CorrespondanceCntNativesCntOptim->NumeroPremiereContrainteDeReserveParZone[Zone]
                  = ProblemeAResoudre->NombreDeContraintes;

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '>');

                NombreDeTermes = 0;
                for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
                {
                    if (NumeroDeZoneDeReserveJMoins1[Pays] != Zone)
                        continue;

                    PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
                    for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques;
                         Index++)
                    {
                        Palier = PaliersThermiquesDuPays
                                   ->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
                        Var = CorrespondanceVarNativesVarOptim
                                ->NumeroDeVariableDuPalierThermique[Palier];
                        if (Var >= 0)
                        {
                            Pi[NombreDeTermes] = -1.0;
                            Colonne[NombreDeTermes] = Var;
                            NombreDeTermes++;
                        }
                    }

                    Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes] = -1.0;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;
                    }

                    Var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableDefaillanceEnReserve[Pays];
                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes] = 1.0;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;
                    }
                }

                CorrespondanceCntNativesCntOptim->NumeroDeuxiemeContrainteDeReserveParZone[Zone]
                  = ProblemeAResoudre->NombreDeContraintes;

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '>');
            }
        }
    }

    for (CntCouplante = 0; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes;
         CntCouplante++)
    {
        MatriceDesContraintesCouplantes
          = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
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
                  = ProblemeHebdo->CorrespondanceCntNativesCntOptimJournalieres[Jour];
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
                            Pdt1 = (Pdt + Offset + ProblemeHebdo->NombreDePasDeTemps)
                                   % NombreDePasDeTempsPourUneOptimisation;
                        }

                        Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]
                                ->NumeroDeVariableDeLInterconnexion[Interco];
                        if (Var >= 0)
                        {
                            Pi[NombreDeTermes] = Poids;
                            Colonne[NombreDeTermes] = Var;
                            NombreDeTermes++;
                        }
                    }
                }

                for (Index = 0; Index < NbClusters; Index++)
                {
                    Pays = MatriceDesContraintesCouplantes->PaysDuPalierDispatch[Index];
                    PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
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
                            Pdt1 = (Pdt + Offset + ProblemeHebdo->NombreDePasDeTemps)
                                   % NombreDePasDeTempsPourUneOptimisation;
                        }

                        Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]
                                ->NumeroDeVariableDuPalierThermique[Palier];
                        if (Var >= 0)
                        {
                            Pi[NombreDeTermes] = Poids;
                            Colonne[NombreDeTermes] = Var;
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
                            Pdt1 = (Pdt + Offset + ProblemeHebdo->NombreDePasDeTemps)
                                   % NombreDePasDeTempsPourUneOptimisation;
                        }

                        Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]
                                ->NumeroDeVariableDeLInterconnexion[Interco];
                        if (Var >= 0)
                        {
                            Pi[NombreDeTermes] = Poids;
                            Colonne[NombreDeTermes] = Var;
                            NombreDeTermes++;
                        }
                    }
                }

                NbClusters = MatriceDesContraintesCouplantes
                               ->NombreDePaliersDispatchDansLaContrainteCouplante;
                for (Index = 0; Index < NbClusters; Index++)
                {
                    Pays = MatriceDesContraintesCouplantes->PaysDuPalierDispatch[Index];
                    PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
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
                            Pdt1 = (Pdt + Offset + ProblemeHebdo->NombreDePasDeTemps)
                                   % NombreDePasDeTempsPourUneOptimisation;
                        }

                        Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]
                                ->NumeroDeVariableDuPalierThermique[Palier];
                        if (Var >= 0)
                        {
                            Pi[NombreDeTermes] = Poids;
                            Colonne[NombreDeTermes] = Var;
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

    for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
    {
        char presenceHydro
          = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable;
        char TurbEntreBornes
          = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->TurbinageEntreBornes;
        char presencePompage
          = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDePompageModulable;
        if (presenceHydro == OUI_ANTARES && TurbEntreBornes == NON_ANTARES)
        {
            NombreDeTermes = 0;
            if (presencePompage == NON_ANTARES)
            {
                for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
                {
                    Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                            ->NumeroDeVariablesDeLaProdHyd[Pays];
                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes] = 1.0;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;
                    }
                }
            }
            if (presencePompage == OUI_ANTARES)
            {
                for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
                {
                    Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                            ->NumeroDeVariablesDeLaProdHyd[Pays];
                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes] = 1.0;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;
                    }
                    Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                            ->NumeroDeVariablesDePompage[Pays];
                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes]
                          = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PumpingRatio;
                        Pi[NombreDeTermes] *= -1.0;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;
                    }
                }
            }

            ProblemeHebdo->NumeroDeContrainteEnergieHydraulique[Pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=');
        }
        else
            ProblemeHebdo->NumeroDeContrainteEnergieHydraulique[Pays] = -1;
    }

    if (ProblemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
    {
        for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
        {
            if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable
                != OUI_ANTARES)
                continue;

            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                CorrespondanceVarNativesVarOptim
                  = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];
                NombreDeTermes = 0;
                Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;
                }
                Pdt1 = Pdt + 1;
                if (Pdt1 >= NombreDePasDeTempsPourUneOptimisation)
                    Pdt1 = 0;
                Var1 = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt1]
                         ->NumeroDeVariablesDeLaProdHyd[Pays];
                if (Var1 >= 0)
                {
                    Pi[NombreDeTermes] = -1.0;
                    Colonne[NombreDeTermes] = Var1;
                    NombreDeTermes++;
                }
                Var2
                  = CorrespondanceVarNativesVarOptim->NumeroDeVariablesVariationHydALaBaisse[Pays];
                if (Var2 >= 0)
                {
                    Pi[NombreDeTermes] = -1.0;
                    Colonne[NombreDeTermes] = Var2;
                    NombreDeTermes++;
                }
                Var3
                  = CorrespondanceVarNativesVarOptim->NumeroDeVariablesVariationHydALaHausse[Pays];
                if (Var3 >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = Var3;
                    NombreDeTermes++;
                }

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=');
            }
        }
    }
    else if (ProblemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
    {
        for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
        {
            if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable
                != OUI_ANTARES)
                continue;

            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                CorrespondanceVarNativesVarOptim
                  = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];
                NombreDeTermes = 0;
                Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;
                }
                Var1 = ProblemeHebdo->CorrespondanceVarNativesVarOptim[0]
                         ->NumeroDeVariablesVariationHydALaBaisse[Pays];
                if (Var1 >= 0)
                {
                    Pi[NombreDeTermes] = -1.0;
                    Colonne[NombreDeTermes] = Var1;
                    NombreDeTermes++;
                }

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '<');

                NombreDeTermes = 0;
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;
                }
                Var1 = ProblemeHebdo->CorrespondanceVarNativesVarOptim[0]
                         ->NumeroDeVariablesVariationHydALaHausse[Pays];
                if (Var1 >= 0)
                {
                    Pi[NombreDeTermes] = -1.0;
                    Colonne[NombreDeTermes] = Var1;
                    NombreDeTermes++;
                }

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '>');
            }
        }
    }

    for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
    {
        char presenceHydro
          = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable;
        char TurbEntreBornes
          = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->TurbinageEntreBornes;
        if (presenceHydro == OUI_ANTARES && TurbEntreBornes == OUI_ANTARES)
        {
            NombreDeTermes = 0;
            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                        ->NumeroDeVariablesDeLaProdHyd[Pays];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;
                }
            }

            ProblemeHebdo->NumeroDeContrainteMinEnergieHydraulique[Pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '>');
        }
        else
            ProblemeHebdo->NumeroDeContrainteMinEnergieHydraulique[Pays] = -1;

        if (presenceHydro == OUI_ANTARES && TurbEntreBornes == OUI_ANTARES)
        {
            NombreDeTermes = 0;
            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                        ->NumeroDeVariablesDeLaProdHyd[Pays];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;
                }
            }

            ProblemeHebdo->NumeroDeContrainteMaxEnergieHydraulique[Pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '<');
        }
        else
            ProblemeHebdo->NumeroDeContrainteMaxEnergieHydraulique[Pays] = -1;
    }

    for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
    {
        if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDePompageModulable
            == OUI_ANTARES)
        {
            NombreDeTermes = 0;
            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                        ->NumeroDeVariablesDePompage[Pays];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;
                }
            }

            ProblemeHebdo->NumeroDeContrainteMaxPompage[Pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '<');
        }
        else
            ProblemeHebdo->NumeroDeContrainteMaxPompage[Pays] = -1;
    }

    for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
    {
        CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];
        CorrespondanceCntNativesCntOptim = ProblemeHebdo->CorrespondanceCntNativesCntOptim[Pdt];

        int timeStepInYear = study->runtime->weekInTheYear[numSpace] * 168 + Pdt;

        for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
        {
            if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->SuiviNiveauHoraire
                == OUI_ANTARES)
            {
                NombreDeTermes = 0;

                Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeNiveau[Pays];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;
                }

                if (Pdt > 0)
                {
                    Var1 = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt - 1]
                             ->NumeroDeVariablesDeNiveau[Pays];
                    if (Var1 >= 0)
                    {
                        Pi[NombreDeTermes] = -1.0;
                        Colonne[NombreDeTermes] = Var1;
                        NombreDeTermes++;
                    }
                }

                Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;
                }

                Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDePompage[Pays];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes]
                      = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PumpingRatio;
                    Pi[NombreDeTermes] *= -1;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;
                }

                Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeDebordement[Pays];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = 1.;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;
                }

                CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesNiveauxPays[Pays]
                  = ProblemeAResoudre->NombreDeContraintes;

                NomDeLaContrainte = "hydro_level::" + std::to_string(timeStepInYear + 1)
                                    + "::" + ProblemeHebdo->NomsDesPays[Pays];

                OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                  ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=', NomDeLaContrainte);
            }
            else
                CorrespondanceCntNativesCntOptim->NumeroDeContrainteDesNiveauxPays[Pays] = -1;
        }
    }

    /* For each area with ad hoc properties, two possible sets of two additional constraints */
    for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
    {
        char PumpAvailable
          = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDePompageModulable;
        if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->AccurateWaterValue == OUI_ANTARES
            && ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->DirectLevelAccess == NON_ANTARES)
        /*  bounding constraint : StockFinal- efficiency*sum(Pump) +sum(Gen) <=
         * InflowsForTimeInterval + LevelForTimeInterval*/
        {
            NombreDeTermes = 0;
            Var = ProblemeHebdo->NumeroDeVariableStockFinal[Pays];
            if (Var >= 0)
            {
                Pi[NombreDeTermes] = 1.0;
                Colonne[NombreDeTermes] = Var;
                NombreDeTermes++;
            }

            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                        ->NumeroDeVariablesDeLaProdHyd[Pays];
                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;
                }
            }
            if (PumpAvailable == OUI_ANTARES)
            {
                for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
                {
                    Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                            ->NumeroDeVariablesDePompage[Pays];
                    if (Var >= 0)
                    {
                        Pi[NombreDeTermes]
                          = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PumpingRatio;
                        Pi[NombreDeTermes] *= -1.0;
                        Colonne[NombreDeTermes] = Var;
                        NombreDeTermes++;
                    }
                }
            }

            ProblemeHebdo->NumeroDeContrainteBorneStockFinal[Pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '<');
        }
        if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->AccurateWaterValue == OUI_ANTARES
            && ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->DirectLevelAccess == OUI_ANTARES)
        /*  equivalence constraint : StockFinal- Niveau[T]= 0*/
        {
            NombreDeTermes = 0;
            Var = ProblemeHebdo->NumeroDeVariableStockFinal[Pays];
            if (Var >= 0)
            {
                Pi[NombreDeTermes] = 1.0;
                Colonne[NombreDeTermes] = Var;
                NombreDeTermes++;
            }
            Var = ProblemeHebdo
                    ->CorrespondanceVarNativesVarOptim[NombreDePasDeTempsPourUneOptimisation - 1]
                    ->NumeroDeVariablesDeNiveau[Pays];
            if (Var >= 0)
            {
                Pi[NombreDeTermes] = -1.0;
                Colonne[NombreDeTermes] = Var;
                NombreDeTermes++;
            }
            ProblemeHebdo->NumeroDeContrainteEquivalenceStockFinal[Pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=');
        }
        if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->AccurateWaterValue == OUI_ANTARES)
        /*  expression constraint : - StockFinal +sum (stocklayers) = 0*/
        {
            NombreDeTermes = 0;
            Var = ProblemeHebdo->NumeroDeVariableStockFinal[Pays];
            if (Var >= 0)
            {
                Pi[NombreDeTermes] = -1.0;
                Colonne[NombreDeTermes] = Var;
                NombreDeTermes++;
            }
            for (layerindex = 0; layerindex < 100; layerindex++)
            {
                Var = ProblemeHebdo->NumeroDeVariableDeTrancheDeStock[Pays][layerindex];

                if (Var >= 0)
                {
                    Pi[NombreDeTermes] = 1.0;
                    Colonne[NombreDeTermes] = Var;
                    NombreDeTermes++;
                }
            }

            ProblemeHebdo->NumeroDeContrainteExpressionStockFinal[Pays]
              = ProblemeAResoudre->NombreDeContraintes;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=');
        }
    }

    if (ProblemeHebdo->OptimisationAvecCoutsDeDemarrage == OUI_ANTARES)
    {
        Simulation = NON_ANTARES;
        OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaireCoutsDeDemarrage(ProblemeHebdo,
                                                                                Simulation);
    }

    // Export structure
    if (exportStructure)
    {
        OPT_ExportVariables(*study, varname, "variables", "txt", numSpace);

        // TODO : for now empty constraints.txt file needed
        std::vector<std::string> conname;
        OPT_ExportVariables(*study, conname, "constraints", "txt", numSpace);
    }

    return;
}
