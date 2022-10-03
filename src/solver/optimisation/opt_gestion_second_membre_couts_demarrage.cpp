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
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"

#include "spx_constantes_externes.h"
#include "../simulation/sim_structure_probleme_adequation.h"

#include <antares/logs.h>
#include <antares/study.h>
#include <antares/emergency.h>
using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;

void OPT_InitialiserLeSecondMembreDuProblemeLineaireCoutsDeDemarrage(PROBLEME_HEBDO* ProblemeHebdo,
                                                                     int PremierPdtDeLIntervalle,
                                                                     int DernierPdtDeLIntervalle)
{
    int Cnt;
    int PdtJour;
    int PdtHebdo;
    int Pays;
    double* SecondMembre;
    int Index;
    int Palier;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    double** AdresseOuPlacerLaValeurDesCoutsMarginaux;
    int* NombreMaxDeGroupesEnMarcheDuPalierThermique;
    int DureeMinimaleDArretDUnGroupeDuPalierThermique;
    int NombreDePasDeTempsPourUneOptimisation;
    int t1moins1;
    int k;
    int t1;

    double DemandeDeReservePrimaire;

    PALIERS_THERMIQUES* PaliersThermiquesDuPays;
    CORRESPONDANCES_DES_CONTRAINTES* CorrespondanceCntNativesCntOptim;

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    SecondMembre = ProblemeAResoudre->SecondMembre;

    AdresseOuPlacerLaValeurDesCoutsMarginaux
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux;

    NombreDePasDeTempsPourUneOptimisation = ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
    {
        PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];

        for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++)
        {
            NombreMaxDeGroupesEnMarcheDuPalierThermique
              = PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Index]
                  ->NombreMaxDeGroupesEnMarcheDuPalierThermique;
            DureeMinimaleDArretDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays->DureeMinimaleDArretDUnGroupeDuPalierThermique[Index];
            Palier
              = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];

            for (PdtJour = 0, PdtHebdo = PremierPdtDeLIntervalle;
                 PdtHebdo < DernierPdtDeLIntervalle;
                 PdtHebdo++, PdtJour++)
            {
                CorrespondanceCntNativesCntOptim
                  = ProblemeHebdo->CorrespondanceCntNativesCntOptim[PdtJour];

#if VARIABLES_MMOINS_MOINS_BORNEES_DES_2_COTES != OUI_ANTARES
                Cnt = CorrespondanceCntNativesCntOptim
                        ->NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne
                          [Palier];
                if (Cnt >= 0)
                {
                    t1 = PdtHebdo;
                    t1moins1 = t1 - 1;
                    if (t1moins1 < 0)
                        t1moins1 = NombreDePasDeTempsPourUneOptimisation + t1moins1;
                    if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                          - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                        > 0)
                    {
                        SecondMembre[Cnt] = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                                            - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];
                    }
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = NULL;
                }
#endif

                Cnt = CorrespondanceCntNativesCntOptim
                        ->NumeroDeContrainteDesContraintesDeDureeMinDArret[Palier];
                if (Cnt >= 0)
                {
                    t1 = PdtHebdo - DureeMinimaleDArretDUnGroupeDuPalierThermique;
                    if (t1 < 0)
                        t1 = NombreDePasDeTempsPourUneOptimisation + t1;
                    SecondMembre[Cnt] = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];
                    for (k = PdtHebdo - DureeMinimaleDArretDUnGroupeDuPalierThermique + 1;
                         k <= PdtHebdo;
                         k++)
                    {
                        t1 = k;

                        if (t1 < 0)
                            t1 = NombreDePasDeTempsPourUneOptimisation + t1;

                        t1moins1 = t1 - 1;

                        if (t1moins1 < 0)
                            t1moins1 = NombreDePasDeTempsPourUneOptimisation + t1moins1;

                        if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                              - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                            > 0)
                        {
                            SecondMembre[Cnt]
                              += NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                                 - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1];
                        }
                    }
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = NULL;
                }
            }
        }

        for (PdtHebdo = 0; PdtHebdo < NombreDePasDeTempsPourUneOptimisation; PdtHebdo++)
        {
            CorrespondanceCntNativesCntOptim
                  = ProblemeHebdo->CorrespondanceCntNativesCntOptim[PdtHebdo];

            Cnt = CorrespondanceCntNativesCntOptim
                    ->NumeroDeContrainteDesReservesPays[Pays];
            DemandeDeReservePrimaire = ProblemeHebdo
                                        ->DemandeDeReservePrimaire[Pays];
            
            if (Cnt >= 0)
            {
                SecondMembre[Cnt] = DemandeDeReservePrimaire;
                AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt] = NULL;
            }
        }
    }

    return;
}
