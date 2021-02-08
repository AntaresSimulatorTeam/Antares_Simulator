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

#include "h2o_m_donnees_annuelles.h"
#include "h2o_m_fonctions.h"

void H2O_M_InitialiserLeSecondMembre(DONNEES_ANNUELLES* DonneesAnnuelles)
{
    int Pdt;
    int Cnt;
    int NbPdt;
    double* TurbineCible;
    double* SecondMembre;
    double ChgmtSens;

    PROBLEME_HYDRAULIQUE* ProblemeHydraulique;
    PROBLEME_LINEAIRE_PARTIE_VARIABLE* ProblemeLineairePartieVariable;

    ChgmtSens = -1;

    NbPdt = DonneesAnnuelles->NombreDePasDeTemps;

    TurbineCible = DonneesAnnuelles->TurbineCible;

    ProblemeHydraulique = DonneesAnnuelles->ProblemeHydraulique;

    ProblemeLineairePartieVariable = ProblemeHydraulique->ProblemeLineairePartieVariable;
    SecondMembre = ProblemeLineairePartieVariable->SecondMembre;

    Cnt = 0;

    for (Pdt = 1; Pdt < NbPdt; Pdt++)
    {
        SecondMembre[Cnt] = DonneesAnnuelles->Apport[Pdt - 1];
        Cnt++;
    }

    SecondMembre[Cnt] = DonneesAnnuelles->Volume[0] - DonneesAnnuelles->Apport[NbPdt - 1];
    Cnt++;

    for (Pdt = 1; Pdt < NbPdt; Pdt++)
    {
        SecondMembre[Cnt] = DonneesAnnuelles->VolumeMax[Pdt];
        Cnt++;

        SecondMembre[Cnt] = DonneesAnnuelles->VolumeMin[Pdt] * ChgmtSens;
        Cnt++;
    }

    for (Pdt = 1; Pdt < NbPdt; Pdt++)
    {
        SecondMembre[Cnt] = 0.;
        Cnt++;
    }

    for (Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        SecondMembre[Cnt] = TurbineCible[Pdt];
        Cnt++;

        SecondMembre[Cnt] = 0.0 * ChgmtSens;
        Cnt++;
    }

    return;
}
