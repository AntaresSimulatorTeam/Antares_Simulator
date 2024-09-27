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

#include <algorithm>

#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

#define ZERO 1.e-9

void H2O_J_LisserLesSurTurbines(DONNEES_MENSUELLES* DonneesMensuelles, int NumeroDeProbleme)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;

    auto& Turbine = DonneesMensuelles->Turbine;
    const auto& TurbineMax = DonneesMensuelles->TurbineMax;
    const auto& TurbineCible = DonneesMensuelles->TurbineCible;

    const int NbPdt = ProblemeHydraulique.NbJoursDUnProbleme[NumeroDeProbleme];

    std::vector<bool> flag(NbPdt);

    double SurTurbineARepartir = 0.0;
    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        flag[Pdt] = (Turbine[Pdt] - TurbineCible[Pdt] > ZERO);
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        if (flag[Pdt])
        {
            SurTurbineARepartir += Turbine[Pdt] - TurbineCible[Pdt];
        }
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        flag[Pdt] = (TurbineMax[Pdt] - TurbineCible[Pdt] > ZERO);
    };

    int NbCycles = 0;
BoucleDeRepartition:

    const int Np = std::count(flag.begin(), flag.end(), true);

    if (Np == 0)
    {
        return;
    }

    double MargeMin = 0.;
    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        MargeMin += TurbineMax[Pdt];
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        if (flag[Pdt] && TurbineMax[Pdt] - TurbineCible[Pdt] < MargeMin)
        {
            MargeMin = TurbineMax[Pdt] - TurbineCible[Pdt];
        }
    }

    double Xmoy = SurTurbineARepartir / Np;
    double SurTurbine;
    if (Xmoy <= MargeMin)
    {
        SurTurbine = Xmoy;
    }
    else
    {
        SurTurbine = MargeMin;
    }

    bool limiteAtteinte = false;
    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        if (!flag[Pdt])
        {
            continue;
        }

        Turbine[Pdt] = TurbineCible[Pdt] + SurTurbine;
        if (TurbineMax[Pdt] - Turbine[Pdt] <= ZERO)
        {
            SurTurbineARepartir -= SurTurbine;
            limiteAtteinte = true;
            flag[Pdt] = false;
        }
    }

    if (limiteAtteinte && SurTurbineARepartir > 0.0)
    {
        NbCycles++;
        if (NbCycles <= NbPdt)
        {
            goto BoucleDeRepartition;
        }
    }

    return;
}
