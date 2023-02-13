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
#include "../simulation/sim_extern_variables_globales.h"

#include <yuni/io/file.h>
#include "opt_fonctions.h"

constexpr double ZERO_PMIN = 1.e-2;

double OPT_CalculerAireMaxPminJour(int PremierPdt,
                                   int DernierPdt,
                                   int MUTetMDT,
                                   int NombreDePasDeTemps,
                                   int* NbGrpCourbeGuide,
                                   int* NbGrpOpt)
{
    double Cout = 0.0;
    int NbMx = 0;

    for (int i = 0; i < PremierPdt; i++)
    {
        if (NbGrpCourbeGuide[i] > NbMx)
            NbMx = NbGrpCourbeGuide[i];
    }

    for (int i = DernierPdt; i < NombreDePasDeTemps; i++)
    {
        if (NbGrpCourbeGuide[i] > NbMx)
            NbMx = NbGrpCourbeGuide[i];
    }

    for (int i = 0; i < PremierPdt; i++)
    {
        NbGrpOpt[i] = NbMx;
        Cout += (double)(NbGrpOpt[i] - NbGrpCourbeGuide[i]);
    }

    for (int i = DernierPdt; i < NombreDePasDeTemps; i++)
    {
        NbGrpOpt[i] = NbMx;
        Cout += (double)(NbGrpOpt[i] - NbGrpCourbeGuide[i]);
    }

    int i = PremierPdt;
    while (i < DernierPdt)
    {
        NbMx = 0;
        int countMUT = 0;
        for (int countMUT = 0; countMUT < MUTetMDT && i < DernierPdt; countMUT++, i++)
        {
            if (NbGrpCourbeGuide[i] > NbMx)
                NbMx = NbGrpCourbeGuide[i];
        }

        i -= countMUT;
        for (countMUT = 0; countMUT < MUTetMDT && i < DernierPdt; countMUT++, i++)
        {
            NbGrpOpt[i] = NbMx;
            Cout += (double)(NbGrpOpt[i] - NbGrpCourbeGuide[i]);
        }
    }

    return (Cout);
}

void OPT_CalculerLesPminThermiquesEnFonctionDeMUTetMDT(PROBLEME_HEBDO* problemeHebdo)
{
    int NombreDePasDeTemps = problemeHebdo->NombreDePasDeTemps;
    int* NbGrpCourbeGuide = problemeHebdo->NbGrpCourbeGuide;
    int* NbGrpOpt = problemeHebdo->NbGrpOpt;

    for (int Pays = 0; Pays < problemeHebdo->NombreDePays; ++Pays)
    {
        RESULTATS_HORAIRES* ResultatsHoraires = problemeHebdo->ResultatsHoraires[Pays];
        PALIERS_THERMIQUES* PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[Pays];
        double* PminDuPalierThermiquePendantUneHeure
          = PaliersThermiquesDuPays->PminDuPalierThermiquePendantUneHeure;
        double* TailleUnitaireDUnGroupeDuPalierThermique
          = PaliersThermiquesDuPays->TailleUnitaireDUnGroupeDuPalierThermique;
        int* minUpDownTime = PaliersThermiquesDuPays->minUpDownTime;

        PRODUCTION_THERMIQUE_OPTIMALE** ProductionThermiqueOptimale = ResultatsHoraires->ProductionThermique;

        for (int Palier = 0; Palier < PaliersThermiquesDuPays->NombreDePaliersThermiques; Palier++)
        {
            PDISP_ET_COUTS_HORAIRES_PAR_PALIER* PuissanceDispoEtCout
                = PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Palier];
            double* PuissanceMinDuPalierThermique = PuissanceDispoEtCout->PuissanceMinDuPalierThermique;
            double* PuissanceDisponibleDuPalierThermique
              = PuissanceDispoEtCout->PuissanceDisponibleDuPalierThermique;

            if (fabs(PminDuPalierThermiquePendantUneHeure[Palier]) < ZERO_PMIN)
                continue;

            for (int Pdt = 0; Pdt < NombreDePasDeTemps; Pdt++)
            {
                double P = ProductionThermiqueOptimale[Pdt]->ProductionThermiqueDuPalier[Palier];

                NbGrpCourbeGuide[Pdt] = 0;
                if (fabs(P) < ZERO_PMIN)
                    continue;

                if (TailleUnitaireDUnGroupeDuPalierThermique[Palier] > ZERO_PMIN)
                    NbGrpCourbeGuide[Pdt]
                      = (int)ceil(P / TailleUnitaireDUnGroupeDuPalierThermique[Palier]);
                else
                    NbGrpCourbeGuide[Pdt] = (int)ceil(P);
            }

            double EcartOpt = LINFINI_ANTARES;
            int MUTetMDT = minUpDownTime[Palier];

            int iOpt = -1;

            int IntervalleDAjustement = MUTetMDT;
            if (NombreDePasDeTemps - MUTetMDT < IntervalleDAjustement)
                IntervalleDAjustement = NombreDePasDeTemps - MUTetMDT;

            if (IntervalleDAjustement < 0)
                IntervalleDAjustement = 0;

            for (int i = 0; i <= IntervalleDAjustement; i++)
            {
                int PremierPdt = i;
                int DernierPdt = NombreDePasDeTemps - IntervalleDAjustement + i;
                double Ecart = OPT_CalculerAireMaxPminJour(
                  PremierPdt, DernierPdt, MUTetMDT, NombreDePasDeTemps, NbGrpCourbeGuide, NbGrpOpt);
                if (Ecart < EcartOpt)
                {
                    EcartOpt = Ecart;
                    iOpt = i;
                }
            }

            if (iOpt < 0)
                continue;

            int PremierPdt = iOpt;
            int DernierPdt = NombreDePasDeTemps - IntervalleDAjustement + iOpt;

            OPT_CalculerAireMaxPminJour(PremierPdt, DernierPdt, MUTetMDT, NombreDePasDeTemps, NbGrpCourbeGuide, NbGrpOpt);

            for (int Pdt = 0; Pdt < NombreDePasDeTemps; Pdt++)
            {
                if (PminDuPalierThermiquePendantUneHeure[Palier] * NbGrpOpt[Pdt]
                    > PuissanceMinDuPalierThermique[Pdt])
                    PuissanceMinDuPalierThermique[Pdt]
                      = PminDuPalierThermiquePendantUneHeure[Palier] * NbGrpOpt[Pdt];

                if (PuissanceMinDuPalierThermique[Pdt] > PuissanceDisponibleDuPalierThermique[Pdt])
                    PuissanceMinDuPalierThermique[Pdt] = PuissanceDisponibleDuPalierThermique[Pdt];
            }
        }
    }

    return;
}
