/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "antares/solver/simulation/sim_structure_donnees.h"
#include "antares/solver/simulation/sim_extern_variables_globales.h"

#include "antares/solver/optimisation/opt_fonctions.h"

#include <antares/study/study.h>

using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;

void OPT_InitialiserLeSecondMembreDuProblemeLineaireReserves(PROBLEME_HEBDO* problemeHebdo,
                                                             int PremierPdtDeLIntervalle,
                                                             int DernierPdtDeLIntervalle)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    std::vector<double>& SecondMembre = ProblemeAResoudre->SecondMembre;

    std::vector<double*>& AdresseOuPlacerLaValeurDesCoutsMarginaux
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux;

    auto& areaReserves = problemeHebdo->allReserves;

    for (int pdtJour = 0, pdtHebdo = PremierPdtDeLIntervalle; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        const CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
          = problemeHebdo->CorrespondanceCntNativesCntOptim[pdtJour];

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            int pdtGlobal = problemeHebdo->weekInTheYear * problemeHebdo->NombreDePasDeTempsDUneJournee * problemeHebdo->NombreDeJours + pdtJour;
            auto areaReservesUp = areaReserves.thermalAreaReserves[pays].areaCapacityReservationsUp;
            for (const auto& areaReserveUp : areaReservesUp)
            {
                int cnt
                  = CorrespondanceCntNativesCntOptim
                      .NumeroDeContrainteDesContraintesDeBesoinEnReserves[areaReserveUp
                                                                            .globalReserveIndex];
                if (cnt >= 0)
                {
                    SecondMembre[cnt] = areaReserveUp.need.at(pdtGlobal);
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
                }
            }

            auto areaReservesDown
              = areaReserves.thermalAreaReserves[pays].areaCapacityReservationsDown;
            for (const auto& areaReserveDown : areaReservesDown)
            {
                int cnt
                  = CorrespondanceCntNativesCntOptim
                      .NumeroDeContrainteDesContraintesDeBesoinEnReserves[areaReserveDown
                                                                            .globalReserveIndex];
                if (cnt >= 0)
                {
                    SecondMembre[cnt] = areaReserveDown.need.at(pdtGlobal);
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
                }
            }

            for (uint32_t cluster = 0;
                 cluster < problemeHebdo->PaliersThermiquesDuPays[pays].NombreDePaliersThermiques;
                 cluster++)
            {
                int globalClusterIdx = problemeHebdo->PaliersThermiquesDuPays[pays]
                                         .NumeroDuPalierDansLEnsembleDesPaliersThermiques[cluster];
                int cnt1 = CorrespondanceCntNativesCntOptim
                      .NumeroDeContrainteDesContraintesDePuissanceMinDuPalier[globalClusterIdx];
                if (cnt1 >= 0)
                {
                    SecondMembre[cnt1] = problemeHebdo->PaliersThermiquesDuPays[pays]
                                          .PuissanceDisponibleEtCout[cluster]
                                          .PuissanceMinDuPalierThermiqueRef[pdtJour];
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt1] = nullptr;
                }
                
                int cnt2 = CorrespondanceCntNativesCntOptim
                      .NumeroDeContrainteDesContraintesDePuissanceMaxDuPalier[globalClusterIdx];
                if (cnt2 >= 0)
                {
                    SecondMembre[cnt2] = problemeHebdo->PaliersThermiquesDuPays[pays]
                                          .PuissanceDisponibleEtCout[cluster]
                                          .PuissanceDisponibleDuPalierThermiqueRef[pdtJour];
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt2] = nullptr;
                }
            }
        }
    }

    return;
}
