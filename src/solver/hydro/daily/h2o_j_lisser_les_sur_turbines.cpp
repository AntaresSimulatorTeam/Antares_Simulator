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

#include "h2o_j_donnees_mensuelles.h"
#include "h2o_j_fonctions.h"

#include <algorithm>

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
        flag[Pdt] = (Turbine[Pdt] - TurbineCible[Pdt] > ZERO);

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
        if (flag[Pdt])
            SurTurbineARepartir += Turbine[Pdt] - TurbineCible[Pdt];

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
        flag[Pdt] = (TurbineMax[Pdt] - TurbineCible[Pdt] > ZERO);;

    int NbCycles = 0;
BoucleDeRepartition:

    const int Np = std::count(flag.begin(), flag.end(), true);

    if (Np == 0)
        return;

    double MargeMin = 0.;
    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
        MargeMin += TurbineMax[Pdt];

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
        if (flag[Pdt] && TurbineMax[Pdt] - TurbineCible[Pdt] < MargeMin)
            MargeMin = TurbineMax[Pdt] - TurbineCible[Pdt];

    double Xmoy = SurTurbineARepartir / Np;
    double SurTurbine;
    if (Xmoy <= MargeMin)
        SurTurbine = Xmoy;
    else
        SurTurbine = MargeMin;

    bool limiteAtteinte = false;
    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        if (!flag[Pdt])
            continue;

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
            goto BoucleDeRepartition;
    }

    return;
}
