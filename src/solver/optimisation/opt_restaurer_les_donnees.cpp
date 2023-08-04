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
#include <iostream>

void OPT_RestaurerLesDonnees(const PROBLEME_HEBDO* problemeHebdo)
{
    const std::vector<int>& NumeroDeJourDuPasDeTemps = problemeHebdo->NumeroDeJourDuPasDeTemps;
    const int DernierPasDeTemps = problemeHebdo->NombreDePasDeTemps;

    for (int pdt = 0; pdt < DernierPasDeTemps; pdt++)
    {
        for (int interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
        {
            if (COUTS_DE_TRANSPORT& CoutDeTransport = problemeHebdo->CoutDeTransport[interco];
                    CoutDeTransport.IntercoGereeAvecDesCouts)
            {
                CoutDeTransport.CoutDeTransportOrigineVersExtremite[pdt]
                  = CoutDeTransport.CoutDeTransportOrigineVersExtremiteRef[pdt];
                CoutDeTransport.CoutDeTransportExtremiteVersOrigine[pdt]
                  = CoutDeTransport.CoutDeTransportExtremiteVersOrigineRef[pdt];
            }
        }
    }

    for (int pdt = 0; pdt < DernierPasDeTemps; pdt++)
    {
        const CONSOMMATIONS_ABATTUES& ConsommationsAbattuesRef
          = problemeHebdo->ConsommationsAbattuesRef[pdt];
        CONSOMMATIONS_ABATTUES& ConsommationsAbattues = problemeHebdo->ConsommationsAbattues[pdt];
        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            ConsommationsAbattues.ConsommationAbattueDuPays[pays]
              = ConsommationsAbattuesRef.ConsommationAbattueDuPays[pays];
        }
    }

    for (int pdt = 0; pdt < DernierPasDeTemps; pdt++)
    {
        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            ENERGIES_ET_PUISSANCES_HYDRAULIQUES& CaracteristiquesHydrauliques
              = problemeHebdo->CaracteristiquesHydrauliques[pays];

            if (!CaracteristiquesHydrauliques.PresenceDHydrauliqueModulable)
                continue;

            CaracteristiquesHydrauliques.ContrainteDePmaxHydrauliqueHoraire[pdt]
              = CaracteristiquesHydrauliques.ContrainteDePmaxHydrauliqueHoraireRef[pdt];
            if (CaracteristiquesHydrauliques.SansHeuristique)
                continue;
            int jour = NumeroDeJourDuPasDeTemps[pdt];
            double PmaxHydEcretee = CaracteristiquesHydrauliques.CntEnergieH2OParJour[jour];
            PmaxHydEcretee *= problemeHebdo->CoefficientEcretementPMaxHydraulique[pays];
            PmaxHydEcretee /= (double)problemeHebdo->NombreDePasDeTempsDUneJournee;
            // The primary generating power allowance may need to be uplifted to match pumping power
            // allowance
            if (CaracteristiquesHydrauliques.PresenceDePompageModulable)
            {
                // Hydro generating power required to make use of energy stored from pumps
                double PmaxHydUplift
                  = CaracteristiquesHydrauliques.ContrainteDePmaxPompageHoraire[pdt];
                PmaxHydUplift *= problemeHebdo->CoefficientEcretementPMaxHydraulique[pays];

                if (PmaxHydEcretee < PmaxHydUplift)
                    PmaxHydEcretee = PmaxHydUplift;
            }

            // The generating power allowance cannot exceed the maximum available generating power
            if (PmaxHydEcretee
                < CaracteristiquesHydrauliques.ContrainteDePmaxHydrauliqueHoraire[pdt])
            {
                CaracteristiquesHydrauliques.ContrainteDePmaxHydrauliqueHoraire[pdt]
                  = PmaxHydEcretee;
            }
        }
    }

    for (int pdt = 0; pdt < DernierPasDeTemps; pdt++)
    {
        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays
              = problemeHebdo->PaliersThermiquesDuPays[pays];
            for (int palier = 0; palier < PaliersThermiquesDuPays.NombreDePaliersThermiques;
                 palier++)
            {
                PDISP_ET_COUTS_HORAIRES_PAR_PALIER& PuissanceDisponibleEtCout
                  = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[palier];
                PuissanceDisponibleEtCout.PuissanceMinDuPalierThermique[pdt]
                  = PuissanceDisponibleEtCout.PuissanceMinDuPalierThermiqueRef[pdt];
            }
        }
    }

    for (int pdt = 0; pdt < DernierPasDeTemps; pdt++)
    {
        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays
              = problemeHebdo->PaliersThermiquesDuPays[pays];
            for (int palier = 0; palier < PaliersThermiquesDuPays.NombreDePaliersThermiques;
                 palier++)
            {
                PDISP_ET_COUTS_HORAIRES_PAR_PALIER& PuissanceDisponibleEtCout
                  = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[palier];
                PuissanceDisponibleEtCout.PuissanceDisponibleDuPalierThermique[pdt]
                  = PuissanceDisponibleEtCout.PuissanceDisponibleDuPalierThermiqueRef[pdt];

                if (PuissanceDisponibleEtCout.PuissanceMinDuPalierThermique[pdt]
                    > PuissanceDisponibleEtCout.PuissanceDisponibleDuPalierThermique[pdt])
                {
                    PuissanceDisponibleEtCout.PuissanceDisponibleDuPalierThermique[pdt]
                      = PuissanceDisponibleEtCout.PuissanceMinDuPalierThermique[pdt];

                    if (PuissanceDisponibleEtCout.PuissanceDisponibleDuPalierThermique[pdt]
                        > PuissanceDisponibleEtCout
                            .PuissanceDisponibleDuPalierThermiqueRef[pdt])
                    {
                        PuissanceDisponibleEtCout.PuissanceDisponibleDuPalierThermique[pdt]
                          = PuissanceDisponibleEtCout
                              .PuissanceDisponibleDuPalierThermiqueRef[pdt];
                        PuissanceDisponibleEtCout.PuissanceMinDuPalierThermique[pdt]
                          = PuissanceDisponibleEtCout
                              .PuissanceDisponibleDuPalierThermiqueRef[pdt];
                    }
                }

                PuissanceDisponibleEtCout.CoutHoraireDeProductionDuPalierThermique[pdt]
                  = PuissanceDisponibleEtCout.CoutHoraireDeProductionDuPalierThermiqueRef[pdt];
            }
        }
    }

    return;
}
