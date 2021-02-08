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

#include "h2o_j_donnees_mensuelles.h"
#include "h2o_j_fonctions.h"

#define ZERO 1.e-9

void H2O_J_LisserLesSurTurbines(DONNEES_MENSUELLES* DonneesMensuelles, int NumeroDeProbleme)
{
    int Pdt;
    int Cnt;
    int NbPdt;
    double* TurbineCible;
    double* Turbine;
    double* TurbineMax;
    double X;
    int Np;
    char* Flag;
    char LimiteAtteinte;
    int NbCycles;
    double SurTurbineARepartir;
    double Xmoy;
    double MargeMin;
    double SurTurbine;
    PROBLEME_HYDRAULIQUE* ProblemeHydraulique;
    PROBLEME_LINEAIRE_PARTIE_VARIABLE* ProblemeLineairePartieVariable;

    ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;
    ProblemeLineairePartieVariable
      = ProblemeHydraulique->ProblemeLineairePartieVariable[NumeroDeProbleme];

    TurbineMax = DonneesMensuelles->TurbineMax;
    Turbine = DonneesMensuelles->Turbine;
    TurbineCible = DonneesMensuelles->TurbineCible;

    Flag = (char*)ProblemeLineairePartieVariable->Xmax;

    NbPdt = ProblemeHydraulique->NbJoursDUnProbleme[NumeroDeProbleme];

    SurTurbineARepartir = 0.0;
    for (Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        Flag[Pdt] = 0;
        if (Turbine[Pdt] - TurbineCible[Pdt] > ZERO)
            Flag[Pdt] = 1;
    }

    for (Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        if (Flag[Pdt] == 1)
            SurTurbineARepartir += Turbine[Pdt] - TurbineCible[Pdt];
    }

    for (Pdt = 0; Pdt < NbPdt; Pdt++)
        Flag[Pdt] = 0;
    for (Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        if (TurbineMax[Pdt] - TurbineCible[Pdt] > ZERO)
            Flag[Pdt] = 1;
    }

    NbCycles = 0;
BoucleDeRepartition:

    Np = 0;
    for (Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        if (Flag[Pdt] == 1)
            Np++;
    }
    if (Np <= 0)
    {
        return;
    }

    MargeMin = 0.;
    for (Pdt = 0; Pdt < NbPdt; Pdt++)
        MargeMin += TurbineMax[Pdt];
    for (Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        if (Flag[Pdt] == 1)
        {
            if (TurbineMax[Pdt] - TurbineCible[Pdt] < MargeMin)
                MargeMin = TurbineMax[Pdt] - TurbineCible[Pdt];
        }
    }

    Xmoy = SurTurbineARepartir / Np;
    if (Xmoy <= MargeMin)
        SurTurbine = Xmoy;
    else
        SurTurbine = MargeMin;

    LimiteAtteinte = 0;
    for (Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        if (Flag[Pdt] == 0)
            continue;

        Turbine[Pdt] = TurbineCible[Pdt] + SurTurbine;
        if (TurbineMax[Pdt] - Turbine[Pdt] <= ZERO)
        {
            SurTurbineARepartir -= SurTurbine;
            LimiteAtteinte = 1;
            Flag[Pdt] = 0;
        }
    }

    if (LimiteAtteinte == 1 && SurTurbineARepartir > 0.0)
    {
        NbCycles++;
        if (NbCycles <= NbPdt)
            goto BoucleDeRepartition;
    }

    return;
}
