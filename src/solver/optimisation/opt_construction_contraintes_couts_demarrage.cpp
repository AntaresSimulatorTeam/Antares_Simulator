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
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    CORRESPONDANCES_DES_CONTRAINTES* CorrespondanceCntNativesCntOptim;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptimTmoins1;

    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    int nombreDePasDeTempsPourUneOptimisation = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    double* Pi = ProblemeAResoudre->Pi;
    int* Colonne = ProblemeAResoudre->Colonne;

    int nbTermesContraintesPourLesCoutsDeDemarrage = 0;

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        PALIERS_THERMIQUES* PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[pays];

        for (int index = 0; index < PaliersThermiquesDuPays->NombreDePaliersThermiques; index++)
        {
            double pminDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays->pminDUnGroupeDuPalierThermique[index];
            double pmaxDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays->PmaxDUnGroupeDuPalierThermique[index];
            const int palier
              = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                CorrespondanceVarNativesVarOptim
                  = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];

                int nombreDeTermes = 0;

                if (Simulation == NON_ANTARES)
                {
                    int var
                      = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[palier];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = 1.0;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
                else
                    nbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    int var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[palier];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = -pmaxDUnGroupeDuPalierThermique;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
                else
                    nbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    if (nombreDeTermes > 0)
                    {
                        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                          ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '<');
                    }
                }
                else
                    ProblemeAResoudre->NombreDeContraintes += 1;

                nombreDeTermes = 0;

                if (Simulation == NON_ANTARES)
                {
                    int var
                      = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[palier];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = 1.0;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
                else
                    nbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    int var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[palier];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = -pminDUnGroupeDuPalierThermique;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
                else
                    nbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    if (nombreDeTermes > 0)
                    {
                        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                          ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '>');
                    }
                }
                else
                    ProblemeAResoudre->NombreDeContraintes += 1;
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        PALIERS_THERMIQUES* PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[pays];

        for (int index = 0; index < PaliersThermiquesDuPays->NombreDePaliersThermiques; index++)
        {
            const int palier
              = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                CorrespondanceVarNativesVarOptim
                  = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];

                int Pdtmoins1 = pdt - 1;
                if (Pdtmoins1 < 0)
                    Pdtmoins1 = nombreDePasDeTempsPourUneOptimisation + Pdtmoins1;
                CorrespondanceVarNativesVarOptimTmoins1
                  = problemeHebdo->CorrespondanceVarNativesVarOptim[Pdtmoins1];

                int nombreDeTermes = 0;
                if (Simulation == NON_ANTARES)
                {
                    int var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[palier];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = 1.0;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
                else
                    nbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    int var = CorrespondanceVarNativesVarOptimTmoins1
                            ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[palier];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = -1;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
                else
                    nbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    int var
                      = CorrespondanceVarNativesVarOptim
                          ->NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[palier];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = -1;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
                else
                    nbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    int var
                      = CorrespondanceVarNativesVarOptim
                          ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[palier];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = 1;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
                else
                    nbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    if (nombreDeTermes > 0)
                    {
                        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                          ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=');
                    }
                }
                else
                    ProblemeAResoudre->NombreDeContraintes += 1;
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        PALIERS_THERMIQUES* PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[pays];

        for (int index = 0; index < PaliersThermiquesDuPays->NombreDePaliersThermiques; index++)
        {
            const int palier
              = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                CorrespondanceCntNativesCntOptim
                  = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
                CorrespondanceCntNativesCntOptim
                  ->NumeroDeContrainteDesContraintesDeDureeMinDeMarche[palier]
                  = -1;

                int nombreDeTermes = 0;
                if (Simulation == NON_ANTARES)
                {
                    int var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt]
                            ->NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique
                              [palier];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = 1.0;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
                else
                    nbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    int var
                      = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt]
                          ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[palier];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = -1.0;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
                else
                    nbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    if (nombreDeTermes > 0)
                    {
                        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                          ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '<');
                    }
                }
                else
                    ProblemeAResoudre->NombreDeContraintes += 1;
            }
        }
    }

#if VARIABLES_MMOINS_MOINS_BORNEES_DES_2_COTES != OUI_ANTARES
    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        PALIERS_THERMIQUES* PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[pays];

        for (int index = 0; index < PaliersThermiquesDuPays->NombreDePaliersThermiques; index++)
        {
            const int palier
              = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                CorrespondanceCntNativesCntOptim
                  = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
                CorrespondanceCntNativesCntOptim
                  ->NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne[palier]
                  = -1;

                int nombreDeTermes = 0;
                int var = 0;
                if (Simulation == NON_ANTARES)
                {
                    var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt]
                            ->NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique
                              [palier];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = 1.0;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
                else
                    nbTermesContraintesPourLesCoutsDeDemarrage++;

                if (Simulation == NON_ANTARES)
                {
                    if (nombreDeTermes > 0)
                    {
                        CorrespondanceCntNativesCntOptim
                          ->NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne
                            [palier]
                          = ProblemeAResoudre->NombreDeContraintes;

                        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                          ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '<');
                    }
                }
                else
                    ProblemeAResoudre->NombreDeContraintes += 1;
            }
        }
    }
#endif

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        PALIERS_THERMIQUES* PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[pays];

        for (int index = 0; index < PaliersThermiquesDuPays->NombreDePaliersThermiques; index++)
        {
            int DureeMinimaleDeMarcheDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays->DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[index];
            if (DureeMinimaleDeMarcheDUnGroupeDuPalierThermique <= 0)
                continue;
            const int palier
              = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                CorrespondanceCntNativesCntOptim
                  = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
                CorrespondanceCntNativesCntOptim
                  ->NumeroDeContrainteDesContraintesDeDureeMinDeMarche[palier]
                  = -1;

                int nombreDeTermes = 0;
                if (Simulation == NON_ANTARES)
                {
                    int var = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt]
                            ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[palier];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = 1.0;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
                else
                    nbTermesContraintesPourLesCoutsDeDemarrage++;

                for (int k = pdt - DureeMinimaleDeMarcheDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
                {
                    int t1 = k;
                    if (t1 < 0)
                        t1 = nombreDePasDeTempsPourUneOptimisation + t1;

                    if (Simulation == NON_ANTARES)
                    {
                        int var = problemeHebdo->CorrespondanceVarNativesVarOptim[t1]
                                ->NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique
                                  [palier];
                        if (var >= 0)
                        {
                            Pi[nombreDeTermes] = -1.0;
                            Colonne[nombreDeTermes] = var;
                            nombreDeTermes++;
                        }
                    }
                    else
                        nbTermesContraintesPourLesCoutsDeDemarrage++;

                    if (Simulation == NON_ANTARES)
                    {
                        int var
                          = problemeHebdo->CorrespondanceVarNativesVarOptim[t1]
                              ->NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique
                                [palier];
                        if (var >= 0)
                        {
                            Pi[nombreDeTermes] = 1.0;
                            Colonne[nombreDeTermes] = var;
                            nombreDeTermes++;
                        }
                    }
                    else
                        nbTermesContraintesPourLesCoutsDeDemarrage++;
                }

                if (Simulation == NON_ANTARES)
                {
                    if (nombreDeTermes > 1)
                    {
                        CorrespondanceCntNativesCntOptim
                          ->NumeroDeContrainteDesContraintesDeDureeMinDeMarche[palier]
                          = ProblemeAResoudre->NombreDeContraintes;

                        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                          ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '>');
                    }
                }
                else
                    ProblemeAResoudre->NombreDeContraintes += 1;
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        PALIERS_THERMIQUES* PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[pays];

        for (int index = 0; index < PaliersThermiquesDuPays->NombreDePaliersThermiques; index++)
        {
            int DureeMinimaleDArretDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays->DureeMinimaleDArretDUnGroupeDuPalierThermique[index];
            const int palier
              = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                CorrespondanceCntNativesCntOptim
                  = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
                CorrespondanceCntNativesCntOptim
                  ->NumeroDeContrainteDesContraintesDeDureeMinDArret[palier]
                  = -1;

                CorrespondanceVarNativesVarOptim
                  = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];

                if (Simulation == NON_ANTARES)
                {
                    CorrespondanceCntNativesCntOptim
                      ->NumeroDeContrainteDesContraintesDeDureeMinDArret[palier]
                      = -1;
                }

                int nombreDeTermes = 0;
                if (Simulation == NON_ANTARES)
                {
                    int var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[palier];
                    if (var >= 0)
                    {
                        Pi[nombreDeTermes] = 1.0;
                        Colonne[nombreDeTermes] = var;
                        nombreDeTermes++;
                    }
                }
                else
                    nbTermesContraintesPourLesCoutsDeDemarrage++;

                for (int k = pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
                {
                    int t1 = k;
                    if (t1 < 0)
                        t1 = nombreDePasDeTempsPourUneOptimisation + t1;

                    CorrespondanceVarNativesVarOptim
                      = problemeHebdo->CorrespondanceVarNativesVarOptim[t1];
                    if (Simulation == NON_ANTARES)
                    {
                        int var = CorrespondanceVarNativesVarOptim
                                ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique
                                  [palier];
                        if (var >= 0)
                        {
                            Pi[nombreDeTermes] = 1.0;
                            Colonne[nombreDeTermes] = var;
                            nombreDeTermes++;
                        }
                    }
                    else
                        nbTermesContraintesPourLesCoutsDeDemarrage++;
                }
                if (Simulation == NON_ANTARES)
                {
                    if (nombreDeTermes > 1)
                    {
                        CorrespondanceCntNativesCntOptim
                          ->NumeroDeContrainteDesContraintesDeDureeMinDArret[palier]
                          = ProblemeAResoudre->NombreDeContraintes;

                        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                          ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '<');
                    }
                }
                else
                    ProblemeAResoudre->NombreDeContraintes += 1;
            }
        }
    }

    if (Simulation == OUI_ANTARES)
        problemeHebdo->NbTermesContraintesPourLesCoutsDeDemarrage
          = nbTermesContraintesPourLesCoutsDeDemarrage;

    return;
}
