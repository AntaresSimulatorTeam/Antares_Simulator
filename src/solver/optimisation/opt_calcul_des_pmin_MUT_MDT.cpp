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

#include <math.h>

#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_extern_variables_globales.h"
#include "antares/solver/simulation/sim_structure_donnees.h"
#include "antares/solver/simulation/simulation.h"

constexpr double ZERO_PMIN = 1.e-2;

double OPT_CalculerAireMaxPminJour(int PremierPdt,
                                   int DernierPdt,
                                   int MUTetMDT,
                                   int NombreDePasDeTemps,
                                   std::vector<int>& NbGrpCourbeGuide,
                                   std::vector<int>& NbGrpOpt)
{
    double Cout = 0.0;
    int NbMx = 0;

    for (int hour = 0; hour < PremierPdt; hour++)
    {
        if (NbGrpCourbeGuide[hour] > NbMx)
        {
            NbMx = NbGrpCourbeGuide[hour];
        }
    }

    for (int hour = DernierPdt; hour < NombreDePasDeTemps; hour++)
    {
        if (NbGrpCourbeGuide[hour] > NbMx)
        {
            NbMx = NbGrpCourbeGuide[hour];
        }
    }

    for (int hour = 0; hour < PremierPdt; hour++)
    {
        NbGrpOpt[hour] = NbMx;
        Cout += (double)(NbGrpOpt[hour] - NbGrpCourbeGuide[hour]);
    }

    for (int hour = DernierPdt; hour < NombreDePasDeTemps; hour++)
    {
        NbGrpOpt[hour] = NbMx;
        Cout += (double)(NbGrpOpt[hour] - NbGrpCourbeGuide[hour]);
    }

    int hour = PremierPdt;
    while (hour < DernierPdt)
    {
        NbMx = 0;
        int countMUT = 0;
        for (countMUT = 0; countMUT < MUTetMDT && hour < DernierPdt; countMUT++, hour++)
        {
            if (NbGrpCourbeGuide[hour] > NbMx)
            {
                NbMx = NbGrpCourbeGuide[hour];
            }
        }

        hour -= countMUT;
        for (countMUT = 0; countMUT < MUTetMDT && hour < DernierPdt; countMUT++, hour++)
        {
            NbGrpOpt[hour] = NbMx;
            Cout += (double)(NbGrpOpt[hour] - NbGrpCourbeGuide[hour]);
        }
    }

    return (Cout);
}

void OPT_CalculerLesPminThermiquesEnFonctionDeMUTetMDT(PROBLEME_HEBDO* problemeHebdo)
{
    int NombreDePasDeTemps = problemeHebdo->NombreDePasDeTemps;
    std::vector<int>& NbGrpCourbeGuide = problemeHebdo->NbGrpCourbeGuide;
    std::vector<int>& NbGrpOpt = problemeHebdo->NbGrpOpt;

    for (uint32_t Pays = 0; Pays < problemeHebdo->NombreDePays; ++Pays)
    {
        const RESULTATS_HORAIRES& ResultatsHoraires = problemeHebdo->ResultatsHoraires[Pays];
        PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[Pays];
        const std::vector<double>& PminDuPalierThermiquePendantUneHeure
          = PaliersThermiquesDuPays.PminDuPalierThermiquePendantUneHeure;
        const std::vector<double>& TailleUnitaireDUnGroupeDuPalierThermique
          = PaliersThermiquesDuPays.TailleUnitaireDUnGroupeDuPalierThermique;
        const std::vector<int>& minUpDownTime = PaliersThermiquesDuPays.minUpDownTime;

        const std::vector<PRODUCTION_THERMIQUE_OPTIMALE>& ProductionThermiqueOptimale
          = ResultatsHoraires.ProductionThermique;

        for (int Palier = 0; Palier < PaliersThermiquesDuPays.NombreDePaliersThermiques; Palier++)
        {
            PDISP_ET_COUTS_HORAIRES_PAR_PALIER& PuissanceDispoEtCout = PaliersThermiquesDuPays
                                                                         .PuissanceDisponibleEtCout
                                                                           [Palier];
            std::vector<double>& PuissanceMinDuPalierThermique = PuissanceDispoEtCout
                                                                   .PuissanceMinDuPalierThermique;
            const std::vector<double>& PuissanceDisponibleDuPalierThermique
              = PuissanceDispoEtCout.PuissanceDisponibleDuPalierThermique;

            if (fabs(PminDuPalierThermiquePendantUneHeure[Palier]) < ZERO_PMIN)
            {
                continue;
            }

            for (int Pdt = 0; Pdt < NombreDePasDeTemps; Pdt++)
            {
                double P = ProductionThermiqueOptimale[Pdt].ProductionThermiqueDuPalier[Palier];

                NbGrpCourbeGuide[Pdt] = 0;
                if (fabs(P) < ZERO_PMIN)
                {
                    continue;
                }

                if (TailleUnitaireDUnGroupeDuPalierThermique[Palier] > ZERO_PMIN)
                {
                    NbGrpCourbeGuide[Pdt] = (int)ceil(
                      P / TailleUnitaireDUnGroupeDuPalierThermique[Palier]);
                }
                else
                {
                    NbGrpCourbeGuide[Pdt] = (int)ceil(P);
                }
            }

            double EcartOpt = LINFINI_ANTARES;
            int MUTetMDT = minUpDownTime[Palier];

            int iOpt = -1;

            int IntervalleDAjustement = MUTetMDT;
            if (NombreDePasDeTemps - MUTetMDT < IntervalleDAjustement)
            {
                IntervalleDAjustement = NombreDePasDeTemps - MUTetMDT;
            }

            if (IntervalleDAjustement < 0)
            {
                IntervalleDAjustement = 0;
            }

            for (int hour = 0; hour <= IntervalleDAjustement; hour++)
            {
                int PremierPdt = hour;
                int DernierPdt = NombreDePasDeTemps - IntervalleDAjustement + hour;
                double Ecart = OPT_CalculerAireMaxPminJour(PremierPdt,
                                                           DernierPdt,
                                                           MUTetMDT,
                                                           NombreDePasDeTemps,
                                                           NbGrpCourbeGuide,
                                                           NbGrpOpt);
                if (Ecart < EcartOpt)
                {
                    EcartOpt = Ecart;
                    iOpt = hour;
                }
            }

            if (iOpt < 0)
            {
                continue;
            }

            int PremierPdt = iOpt;
            int DernierPdt = NombreDePasDeTemps - IntervalleDAjustement + iOpt;

            OPT_CalculerAireMaxPminJour(PremierPdt,
                                        DernierPdt,
                                        MUTetMDT,
                                        NombreDePasDeTemps,
                                        NbGrpCourbeGuide,
                                        NbGrpOpt);

            for (int Pdt = 0; Pdt < NombreDePasDeTemps; Pdt++)
            {
                if (PminDuPalierThermiquePendantUneHeure[Palier] * NbGrpOpt[Pdt]
                    > PuissanceMinDuPalierThermique[Pdt])
                {
                    PuissanceMinDuPalierThermique[Pdt] = PminDuPalierThermiquePendantUneHeure
                                                           [Palier]
                                                         * NbGrpOpt[Pdt];
                }

                if (PuissanceMinDuPalierThermique[Pdt] > PuissanceDisponibleDuPalierThermique[Pdt])
                {
                    PuissanceMinDuPalierThermique[Pdt] = PuissanceDisponibleDuPalierThermique[Pdt];
                }
            }
        }
    }

    return;
}
