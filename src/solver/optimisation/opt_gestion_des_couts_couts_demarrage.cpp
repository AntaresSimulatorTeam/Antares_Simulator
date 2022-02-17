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

#include <yuni/yuni.h>
#include <yuni/core/math.h>
#include <limits>
#include <antares/study/area/scratchpad.h>
#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"
#include "../aleatoire/alea_fonctions.h"

#include "spx_constantes_externes.h"

void OPT_InitialiserLesCoutsLineaireCoutsDeDemarrage(PROBLEME_HEBDO* ProblemeHebdo,
                                                     const int PremierPdtDeLIntervalle,
                                                     const int DernierPdtDeLIntervalle)
{
    int PdtJour;
    int Pays;
    int Palier;
    int Var;
    int Index;
    int PdtHebdo;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    double* CoutLineaire;
    PALIERS_THERMIQUES* PaliersThermiquesDuPays;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    CoutLineaire = ProblemeAResoudre->CoutLineaire;

    for (PdtHebdo = PremierPdtDeLIntervalle, PdtJour = 0; PdtHebdo < DernierPdtDeLIntervalle;
         PdtHebdo++, PdtJour++)
    {
        CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[PdtJour];

        for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
        {
            PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];

            for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++)
            {
                Palier
                  = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];

                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[Palier];
                if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                {
                    CoutLineaire[Var]
                      = PaliersThermiquesDuPays->CoutFixeDeMarcheDUnGroupeDuPalierThermique[Index];
                }

                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[Palier];
                if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                {
                    CoutLineaire[Var]
                      = PaliersThermiquesDuPays->CoutDArretDUnGroupeDuPalierThermique[Index];
                }

                Var
                  = CorrespondanceVarNativesVarOptim
                      ->NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique[Palier];
                if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                    CoutLineaire[Var] = 0;

                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[Palier];
                if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                {
#if SUBSTITUTION_DE_LA_VARIABLE_MPLUS != OUI_ANTARES
                    CoutLineaire[Var]
                      = PaliersThermiquesDuPays->CoutDeDemarrageDUnGroupeDuPalierThermique[Index];
#else
                    CoutLineaire[Var] = 0;
#endif
                }
            }
        }
    }

#if SUBSTITUTION_DE_LA_VARIABLE_MPLUS == OUI_ANTARES
    for (PdtHebdo = PremierPdtDeLIntervalle, PdtJour = 0; PdtHebdo < DernierPdtDeLIntervalle;
         PdtHebdo++, PdtJour++)
    {
        CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[PdtJour];

        for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
        {
            PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];

            for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++)
            {
                Palier
                  = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];

                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[Palier];
                if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                {
                    CoutDeDemarrage
                      = PaliersThermiquesDuPays->CoutDeDemarrageDUnGroupeDuPalierThermique[Index];

                    Var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[Palier];
                    if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                        CoutLineaire[Var] += CoutDeDemarrage;

                    PdtJourMoins1 = PdtJour - 1;
                    if (PdtJourMoins1 < 0)
                        PdtJourMoins1 = NombreDePasDeTempsPourUneOptimisation + PdtJourMoins1;
                    Var = ProblemeHebdo->CorrespondanceVarNativesVarOptim[PdtJourMoins1]
                            ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[Palier];
                    if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                        CoutLineaire[Var] -= CoutDeDemarrage;

                    Var
                      = CorrespondanceVarNativesVarOptim
                          ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[Palier];
                    if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                        CoutLineaire[Var] += CoutDeDemarrage;
                }
            }
        }
    }
#endif

#if GROSSES_VARIABLES == OUI_ANTARES
    for (PdtHebdo = PremierPdtDeLIntervalle, PdtJour = 0; PdtHebdo < DernierPdtDeLIntervalle;
         PdtHebdo++, PdtJour++)
    {
        CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[PdtJour];
        for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
        {
            Var = CorrespondanceVarNativesVarOptim->NumeroDeGrosseVariableDefaillancePositive[Pays];
            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[Var] = 0;
            }
            Var = CorrespondanceVarNativesVarOptim->NumeroDeGrosseVariableDefaillanceNegative[Pays];
            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[Var] = 0;
            }

            if (ContrainteDeReserveJMoins1ParZone == OUI_ANTARES)
            {
                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeGrosseVariableDefaillanceEnReserve[Pays];
                if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                {
                    ProblemeAResoudre->CoutLineaire[Var] = 0;
                }
            }
        }
    }
#endif

    return;
}
