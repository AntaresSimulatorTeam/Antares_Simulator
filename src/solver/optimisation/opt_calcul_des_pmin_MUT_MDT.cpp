// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <cmath>
#include <functional>

#include "antares/solver/optimisation/thermalHeuristicsCore.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

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

void OPT_CalculerLaCourbeGuideDuNombreDeGroupes(int NombreDePasDeTemps,
                                                const std::function<double(int)>& production,
                                                double TailleUnitaireDUnGroupe,
                                                std::vector<int>& NbGrpCourbeGuide)
{
    for (int Pdt = 0; Pdt < NombreDePasDeTemps; Pdt++)
    {
        double P = production(Pdt);

        NbGrpCourbeGuide[Pdt] = 0;
        if (fabs(P) < ZERO_PMIN)
        {
            continue;
        }

        if (TailleUnitaireDUnGroupe > ZERO_PMIN)
        {
            NbGrpCourbeGuide[Pdt] = (int)ceil(P / TailleUnitaireDUnGroupe);
        }
        else
        {
            NbGrpCourbeGuide[Pdt] = (int)ceil(P);
        }
    }
}

bool OPT_CalculerLeNombreDeGroupesParBlocsDeMUTetMDT(int NombreDePasDeTemps,
                                                     int MUTetMDT,
                                                     std::vector<int>& NbGrpCourbeGuide,
                                                     std::vector<int>& NbGrpOpt)
{
    double EcartOpt = LINFINI_ANTARES;
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
        return false;
    }

    int PremierPdt = iOpt;
    int DernierPdt = NombreDePasDeTemps - IntervalleDAjustement + iOpt;

    OPT_CalculerAireMaxPminJour(PremierPdt,
                                DernierPdt,
                                MUTetMDT,
                                NombreDePasDeTemps,
                                NbGrpCourbeGuide,
                                NbGrpOpt);
    return true;
}

void OPT_CalculerLesPminThermiquesEnFonctionDeMUTetMDT(int NombreDePasDeTemps,
                                                       std::vector<int>& NbGrpCourbeGuide,
                                                       std::vector<int>& NbGrpOpt,
                                                       RESULTATS_HORAIRES& ResultatsHoraires,
                                                       PALIERS_THERMIQUES& PaliersThermiquesDuPays,
                                                       int indexPalier)
{
    const std::vector<double>& PminDuPalierThermiquePendantUneHeure
      = PaliersThermiquesDuPays.PminDuPalierThermiquePendantUneHeure;
    const std::vector<double>& TailleUnitaireDUnGroupeDuPalierThermique
      = PaliersThermiquesDuPays.TailleUnitaireDUnGroupeDuPalierThermique;
    const std::vector<int>& minUpDownTime = PaliersThermiquesDuPays.minUpDownTime;

    const std::vector<PRODUCTION_THERMIQUE_OPTIMALE>& ProductionThermiqueOptimale
      = ResultatsHoraires.ProductionThermique;

    PDISP_ET_COUTS_HORAIRES_PAR_PALIER& PuissanceDispoEtCout = PaliersThermiquesDuPays
                                                                 .PuissanceDisponibleEtCout
                                                                   [indexPalier];
    std::vector<double>& PuissanceMinDuPalierThermique = PuissanceDispoEtCout
                                                           .PuissanceMinDuPalierThermique;
    const std::vector<double>& PuissanceDisponibleDuPalierThermique
      = PuissanceDispoEtCout.PuissanceDisponibleDuPalierThermique;

    if (fabs(PminDuPalierThermiquePendantUneHeure[indexPalier]) < ZERO_PMIN)
    {
        return;
    }

    OPT_CalculerLaCourbeGuideDuNombreDeGroupes(
      NombreDePasDeTemps,
      [&](int Pdt)
      { return ProductionThermiqueOptimale[Pdt].ProductionThermiqueDuPalier[indexPalier]; },
      TailleUnitaireDUnGroupeDuPalierThermique[indexPalier],
      NbGrpCourbeGuide);

    if (!OPT_CalculerLeNombreDeGroupesParBlocsDeMUTetMDT(NombreDePasDeTemps,
                                                         minUpDownTime[indexPalier],
                                                         NbGrpCourbeGuide,
                                                         NbGrpOpt))
    {
        return;
    }

    for (int Pdt = 0; Pdt < NombreDePasDeTemps; Pdt++)
    {
        if (PminDuPalierThermiquePendantUneHeure[indexPalier] * NbGrpOpt[Pdt]
            > PuissanceMinDuPalierThermique[Pdt])
        {
            PuissanceMinDuPalierThermique[Pdt] = PminDuPalierThermiquePendantUneHeure[indexPalier]
                                                 * NbGrpOpt[Pdt];
        }

        if (PuissanceMinDuPalierThermique[Pdt] > PuissanceDisponibleDuPalierThermique[Pdt])
        {
            PuissanceMinDuPalierThermique[Pdt] = PuissanceDisponibleDuPalierThermique[Pdt];
        }
    }
}
