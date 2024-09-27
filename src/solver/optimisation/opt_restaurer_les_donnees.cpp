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

void OPT_RestaurerLesDonnees(PROBLEME_HEBDO* problemeHebdo)
{
    const std::vector<int>& NumeroDeJourDuPasDeTemps = problemeHebdo->NumeroDeJourDuPasDeTemps;
    const int DernierPasDeTemps = problemeHebdo->NombreDePasDeTemps;

    for (int pdt = 0; pdt < DernierPasDeTemps; pdt++)
    {
        for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
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
        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            ENERGIES_ET_PUISSANCES_HYDRAULIQUES& CaracteristiquesHydrauliques
              = problemeHebdo->CaracteristiquesHydrauliques[pays];

            if (!CaracteristiquesHydrauliques.PresenceDHydrauliqueModulable)
            {
                continue;
            }

            double& hourlyPmax = CaracteristiquesHydrauliques
                                   .ContrainteDePmaxHydrauliqueHoraire[pdt];
            hourlyPmax = CaracteristiquesHydrauliques.ContrainteDePmaxHydrauliqueHoraireRef[pdt];
            if (CaracteristiquesHydrauliques.SansHeuristique)
            {
                continue;
            }
            int jour = NumeroDeJourDuPasDeTemps[pdt];
            double PmaxHydEcretee = CaracteristiquesHydrauliques.CntEnergieH2OParJour[jour];
            PmaxHydEcretee *= problemeHebdo->CoefficientEcretementPMaxHydraulique[pays];
            PmaxHydEcretee /= (double)problemeHebdo->NombreDePasDeTempsDUneJournee;
            // The primary generating power allowance may need to be uplifted to match pumping power
            // allowance
            if (CaracteristiquesHydrauliques.PresenceDePompageModulable)
            {
                // Hydro generating power required to make use of energy stored from pumps
                double PmaxHydUplift = CaracteristiquesHydrauliques
                                         .ContrainteDePmaxPompageHoraire[pdt];
                PmaxHydUplift *= problemeHebdo->CoefficientEcretementPMaxHydraulique[pays];
                PmaxHydEcretee = std::max(PmaxHydUplift, PmaxHydEcretee);
            }

            // The generating power allowance cannot exceed the maximum available generating power
            hourlyPmax = std::min(PmaxHydEcretee, hourlyPmax);
        }
    }

    for (int pdt = 0; pdt < DernierPasDeTemps; pdt++)
    {
        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo
                                                            ->PaliersThermiquesDuPays[pays];
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
        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo
                                                            ->PaliersThermiquesDuPays[pays];
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
                        > PuissanceDisponibleEtCout.PuissanceDisponibleDuPalierThermiqueRef[pdt])
                    {
                        PuissanceDisponibleEtCout.PuissanceDisponibleDuPalierThermique[pdt]
                          = PuissanceDisponibleEtCout.PuissanceDisponibleDuPalierThermiqueRef[pdt];
                        PuissanceDisponibleEtCout.PuissanceMinDuPalierThermique[pdt]
                          = PuissanceDisponibleEtCout.PuissanceDisponibleDuPalierThermiqueRef[pdt];
                    }
                }
            }
        }
    }
}
