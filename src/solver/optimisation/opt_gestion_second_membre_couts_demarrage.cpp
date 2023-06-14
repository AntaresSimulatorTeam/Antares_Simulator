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

void OPT_InitialiserLeSecondMembreDuProblemeLineaireCoutsDeDemarrage(PROBLEME_HEBDO* problemeHebdo,
                                                                     int PremierPdtDeLIntervalle,
                                                                     int DernierPdtDeLIntervalle)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    double* SecondMembre = ProblemeAResoudre->SecondMembre;

    double** AdresseOuPlacerLaValeurDesCoutsMarginaux
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux;

    int NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];

        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            const int* NombreMaxDeGroupesEnMarcheDuPalierThermique
              = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[index]
                  ->NombreMaxDeGroupesEnMarcheDuPalierThermique;
            const int DureeMinimaleDArretDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays.DureeMinimaleDArretDUnGroupeDuPalierThermique[index];
            const int palier
              = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

            for (int pdtJour = 0, pdtHebdo = PremierPdtDeLIntervalle;
                 pdtHebdo < DernierPdtDeLIntervalle;
                 pdtHebdo++, pdtJour++)
            {
                const CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
                  = problemeHebdo->CorrespondanceCntNativesCntOptim[pdtJour];
                int cnt = CorrespondanceCntNativesCntOptim
                        .NumeroDeContrainteDesContraintesDeDureeMinDArret[palier];
                if (cnt >= 0)
                {
                    int t1 = pdtHebdo - DureeMinimaleDArretDUnGroupeDuPalierThermique;
                    if (t1 < 0)
                        t1 = NombreDePasDeTempsPourUneOptimisation + t1;
                    SecondMembre[cnt] = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];
                    for (int k = pdtHebdo - DureeMinimaleDArretDUnGroupeDuPalierThermique + 1;
                         k <= pdtHebdo;
                         k++)
                    {
                        t1 = k;

                        if (t1 < 0)
                            t1 = NombreDePasDeTempsPourUneOptimisation + t1;

                        int t1moins1 = t1 - 1;

                        if (t1moins1 < 0)
                            t1moins1 = NombreDePasDeTempsPourUneOptimisation + t1moins1;

                        if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                              - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                            > 0)
                        {
                            SecondMembre[cnt]
                              += NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                                 - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1];
                        }
                    }
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
                }
            }
        }
    }

    return;
}
