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

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

void OPT_InitialiserLeSecondMembreDuProblemeLineaireCoutsDeDemarrage(PROBLEME_HEBDO* problemeHebdo,
                                                                     int PremierPdtDeLIntervalle,
                                                                     int DernierPdtDeLIntervalle)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    std::vector<double>& SecondMembre = ProblemeAResoudre->SecondMembre;

    std::vector<double*>& AdresseOuPlacerLaValeurDesCoutsMarginaux
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux;

    int NombreDePasDeTempsPourUneOptimisation = problemeHebdo
                                                  ->NombreDePasDeTempsPourUneOptimisation;

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo
                                                              ->PaliersThermiquesDuPays[pays];

        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            const std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
              = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[index]
                  .NombreMaxDeGroupesEnMarcheDuPalierThermique;
            const int DureeMinimaleDArretDUnGroupeDuPalierThermique
              = PaliersThermiquesDuPays.DureeMinimaleDArretDUnGroupeDuPalierThermique[index];
            const int palier = PaliersThermiquesDuPays
                                 .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

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
                    {
                        t1 = NombreDePasDeTempsPourUneOptimisation + t1;
                    }
                    SecondMembre[cnt] = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];
                    for (int k = pdtHebdo - DureeMinimaleDArretDUnGroupeDuPalierThermique + 1;
                         k <= pdtHebdo;
                         k++)
                    {
                        t1 = k;

                        if (t1 < 0)
                        {
                            t1 = NombreDePasDeTempsPourUneOptimisation + t1;
                        }

                        int t1moins1 = t1 - 1;

                        if (t1moins1 < 0)
                        {
                            t1moins1 = NombreDePasDeTempsPourUneOptimisation + t1moins1;
                        }

                        if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                              - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                            > 0)
                        {
                            SecondMembre[cnt] += NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                                                 - NombreMaxDeGroupesEnMarcheDuPalierThermique
                                                   [t1moins1];
                        }
                    }
                    AdresseOuPlacerLaValeurDesCoutsMarginaux[cnt] = nullptr;
                }
            }
        }
    }
}
