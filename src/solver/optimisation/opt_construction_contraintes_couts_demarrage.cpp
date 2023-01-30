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

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_structure_probleme_economique.h"
#include "../simulation/sim_structure_probleme_adequation.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"

#include <antares/study.h>
#include <antares/study/area/scratchpad.h>
#include "../simulation/sim_structure_donnees.h"

void OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaireCoutsDeDemarrage(
  PROBLEME_HEBDO* problemeHebdo,
  char Simulation)
{
    int Index;
    int Pays;
    int Pdt;
    int Palier;
    int NombreDeTermes;
    int NombreDePasDeTempsPourUneOptimisation;
    int DureeMinimaleDeMarcheDUnGroupeDuPalierThermique;
    int DureeMinimaleDArretDUnGroupeDuPalierThermique;
    int k;
    int t1;
    double PminDUnGroupeDuPalierThermique;
    double PmaxDUnGroupeDuPalierThermique;
    int NbTermesContraintesPourLesCoutsDeDemarrage;
    int var;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    CORRESPONDANCES_DES_CONTRAINTES* CorrespondanceCntNativesCntOptim;
    PALIERS_THERMIQUES* PaliersThermiquesDuPays;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    double* Pi;
    int* Colonne;
    int Pdtmoins1;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptimTmoins1;

    ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    NombreDePasDeTempsPourUneOptimisation = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    Pi = ProblemeAResoudre->Pi;
    Colonne = ProblemeAResoudre->Colonne;

    NbTermesContraintesPourLesCoutsDeDemarrage = 0;

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[Pays];

        for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++)
        {
            PminDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays->PminDUnGroupeDuPalierThermique[Index];
            PmaxDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays->PmaxDUnGroupeDuPalierThermique[Index];
            Palier
              = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];

            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                CorrespondanceVarNativesVarOptim
                  = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];

                NombreDeTermes = 0;

                if (Simulation == NON_ANTARES)
                {
                    var
                      = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[Palier];
                    if (var >= 0)
                    {
                        Pi[NombreDeTermes] = 1.0;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;
                    }
                }
                else
                    NbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[Palier];
                    if (var >= 0)
                    {
                        Pi[NombreDeTermes] = -PmaxDUnGroupeDuPalierThermique;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;
                    }
                }
                else
                    NbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    if (NombreDeTermes > 0)
                    {
                        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                          ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '<');
                    }
                }
                else
                    ProblemeAResoudre->NombreDeContraintes += 1;

                NombreDeTermes = 0;

                if (Simulation == NON_ANTARES)
                {
                    var
                      = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[Palier];
                    if (var >= 0)
                    {
                        Pi[NombreDeTermes] = 1.0;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;
                    }
                }
                else
                    NbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[Palier];
                    if (var >= 0)
                    {
                        Pi[NombreDeTermes] = -PminDUnGroupeDuPalierThermique;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;
                    }
                }
                else
                    NbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    if (NombreDeTermes > 0)
                    {
                        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                          ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '>');
                    }
                }
                else
                    ProblemeAResoudre->NombreDeContraintes += 1;
            }
        }
    }

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[Pays];

        for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++)
        {
            Palier
              = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];

            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                CorrespondanceVarNativesVarOptim
                  = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];

                Pdtmoins1 = Pdt - 1;
                if (Pdtmoins1 < 0)
                    Pdtmoins1 = NombreDePasDeTempsPourUneOptimisation + Pdtmoins1;
                CorrespondanceVarNativesVarOptimTmoins1
                  = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdtmoins1];

                NombreDeTermes = 0;
                if (Simulation == NON_ANTARES)
                {
                    var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[Palier];
                    if (var >= 0)
                    {
                        Pi[NombreDeTermes] = 1.0;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;
                    }
                }
                else
                    NbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    var = CorrespondanceVarNativesVarOptimTmoins1
                            ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[Palier];
                    if (var >= 0)
                    {
                        Pi[NombreDeTermes] = -1;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;
                    }
                }
                else
                    NbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    var
                      = CorrespondanceVarNativesVarOptim
                          ->NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[Palier];
                    if (var >= 0)
                    {
                        Pi[NombreDeTermes] = -1;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;
                    }
                }
                else
                    NbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    var
                      = CorrespondanceVarNativesVarOptim
                          ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[Palier];
                    if (var >= 0)
                    {
                        Pi[NombreDeTermes] = 1;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;
                    }
                }
                else
                    NbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    if (NombreDeTermes > 0)
                    {
                        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                          ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=');
                    }
                }
                else
                    ProblemeAResoudre->NombreDeContraintes += 1;
            }
        }
    }

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[Pays];

        for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++)
        {
            Palier
              = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];

            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                CorrespondanceCntNativesCntOptim
                  = problemeHebdo->CorrespondanceCntNativesCntOptim[Pdt];
                CorrespondanceCntNativesCntOptim
                  ->NumeroDeContrainteDesContraintesDeDureeMinDeMarche[Palier]
                  = -1;

                NombreDeTermes = 0;
                if (Simulation == NON_ANTARES)
                {
                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                            ->NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique
                              [Palier];
                    if (var >= 0)
                    {
                        Pi[NombreDeTermes] = 1.0;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;
                    }
                }
                else
                    NbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    var
                      = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                          ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[Palier];
                    if (var >= 0)
                    {
                        Pi[NombreDeTermes] = -1.0;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;
                    }
                }
                else
                    NbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    if (NombreDeTermes > 0)
                    {
                        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                          ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '<');
                    }
                }
                else
                    ProblemeAResoudre->NombreDeContraintes += 1;
            }
        }
    }

#if VARIABLES_MMOINS_MOINS_BORNEES_DES_2_COTES != OUI_ANTARES
    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[Pays];

        for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++)
        {
            Palier
              = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];

            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                CorrespondanceCntNativesCntOptim
                  = problemeHebdo->CorrespondanceCntNativesCntOptim[Pdt];
                CorrespondanceCntNativesCntOptim
                  ->NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne[Palier]
                  = -1;

                NombreDeTermes = 0;
                var = 0;
                if (Simulation == NON_ANTARES)
                {
                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                            ->NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique
                              [Palier];
                    if (var >= 0)
                    {
                        Pi[NombreDeTermes] = 1.0;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;
                    }
                }
                else
                    NbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    if (NombreDeTermes > 0)
                    {
                        CorrespondanceCntNativesCntOptim
                          ->NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne
                            [Palier]
                          = ProblemeAResoudre->NombreDeContraintes;

                        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                          ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '<');
                    }
                }
                else
                    ProblemeAResoudre->NombreDeContraintes += 1;
            }
        }
    }
#endif

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[Pays];

        for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++)
        {
            DureeMinimaleDeMarcheDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays->DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[Index];
            if (DureeMinimaleDeMarcheDUnGroupeDuPalierThermique <= 0)
                continue;
            Palier
              = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];

            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                CorrespondanceCntNativesCntOptim
                  = problemeHebdo->CorrespondanceCntNativesCntOptim[Pdt];
                CorrespondanceCntNativesCntOptim
                  ->NumeroDeContrainteDesContraintesDeDureeMinDeMarche[Palier]
                  = -1;

                NombreDeTermes = 0;
                if (Simulation == NON_ANTARES)
                {
                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt]
                            ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[Palier];
                    if (var >= 0)
                    {
                        Pi[NombreDeTermes] = 1.0;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;
                    }
                }
                else
                    NbTermesContraintesPourLesCoutsDeDemarrage++;

                for (k = Pdt - DureeMinimaleDeMarcheDUnGroupeDuPalierThermique + 1; k <= Pdt; k++)
                {
                    t1 = k;
                    if (t1 < 0)
                        t1 = NombreDePasDeTempsPourUneOptimisation + t1;

                    if (Simulation == NON_ANTARES)
                    {
                        var = problemeHebdo->CorrespondanceVarNativesVarOptim[t1]
                                ->NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique
                                  [Palier];
                        if (var >= 0)
                        {
                            Pi[NombreDeTermes] = -1.0;
                            Colonne[NombreDeTermes] = var;
                            NombreDeTermes++;
                        }
                    }
                    else
                        NbTermesContraintesPourLesCoutsDeDemarrage++;

                    if (Simulation == NON_ANTARES)
                    {
                        var
                          = problemeHebdo->CorrespondanceVarNativesVarOptim[t1]
                              ->NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique
                                [Palier];
                        if (var >= 0)
                        {
                            Pi[NombreDeTermes] = 1.0;
                            Colonne[NombreDeTermes] = var;
                            NombreDeTermes++;
                        }
                    }
                    else
                        NbTermesContraintesPourLesCoutsDeDemarrage++;
                }

                if (Simulation == NON_ANTARES)
                {
                    if (NombreDeTermes > 1)
                    {
                        CorrespondanceCntNativesCntOptim
                          ->NumeroDeContrainteDesContraintesDeDureeMinDeMarche[Palier]
                          = ProblemeAResoudre->NombreDeContraintes;

                        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                          ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '>');
                    }
                }
                else
                    ProblemeAResoudre->NombreDeContraintes += 1;
            }
        }
    }

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[Pays];

        for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++)
        {
            DureeMinimaleDArretDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays->DureeMinimaleDArretDUnGroupeDuPalierThermique[Index];
            Palier
              = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];

            for (Pdt = 0; Pdt < NombreDePasDeTempsPourUneOptimisation; Pdt++)
            {
                CorrespondanceCntNativesCntOptim
                  = problemeHebdo->CorrespondanceCntNativesCntOptim[Pdt];
                CorrespondanceCntNativesCntOptim
                  ->NumeroDeContrainteDesContraintesDeDureeMinDArret[Palier]
                  = -1;

                CorrespondanceVarNativesVarOptim
                  = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdt];

                if (Simulation == NON_ANTARES)
                {
                    CorrespondanceCntNativesCntOptim
                      ->NumeroDeContrainteDesContraintesDeDureeMinDArret[Palier]
                      = -1;
                }

                NombreDeTermes = 0;
                if (Simulation == NON_ANTARES)
                {
                    var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[Palier];
                    if (var >= 0)
                    {
                        Pi[NombreDeTermes] = 1.0;
                        Colonne[NombreDeTermes] = var;
                        NombreDeTermes++;
                    }
                }
                else
                    NbTermesContraintesPourLesCoutsDeDemarrage++;

                for (k = Pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique + 1; k <= Pdt; k++)
                {
                    t1 = k;
                    if (t1 < 0)
                        t1 = NombreDePasDeTempsPourUneOptimisation + t1;

                    CorrespondanceVarNativesVarOptim
                      = problemeHebdo->CorrespondanceVarNativesVarOptim[t1];
                    if (Simulation == NON_ANTARES)
                    {
                        var = CorrespondanceVarNativesVarOptim
                                ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique
                                  [Palier];
                        if (var >= 0)
                        {
                            Pi[NombreDeTermes] = 1.0;
                            Colonne[NombreDeTermes] = var;
                            NombreDeTermes++;
                        }
                    }
                    else
                        NbTermesContraintesPourLesCoutsDeDemarrage++;
                }
                if (Simulation == NON_ANTARES)
                {
                    if (NombreDeTermes > 1)
                    {
                        CorrespondanceCntNativesCntOptim
                          ->NumeroDeContrainteDesContraintesDeDureeMinDArret[Palier]
                          = ProblemeAResoudre->NombreDeContraintes;

                        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                          ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '<');
                    }
                }
                else
                    ProblemeAResoudre->NombreDeContraintes += 1;
            }
        }
    }

    if (Simulation == OUI_ANTARES)
        problemeHebdo->NbTermesContraintesPourLesCoutsDeDemarrage
          = NbTermesContraintesPourLesCoutsDeDemarrage;

    return;
}
