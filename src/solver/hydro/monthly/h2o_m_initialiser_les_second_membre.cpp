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

#include "antares/solver/hydro/monthly/h2o_m_donnees_annuelles.h"
#include "antares/solver/hydro/monthly/h2o_m_fonctions.h"

void H2O_M_InitialiserLeSecondMembre(DONNEES_ANNUELLES& DonneesAnnuelles)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesAnnuelles.ProblemeHydraulique;
    PROBLEME_LINEAIRE_PARTIE_VARIABLE& ProblemeLineairePartieVariable
      = ProblemeHydraulique.ProblemeLineairePartieVariable;

    double ChgmtSens = -1;
    const int NbPdt = DonneesAnnuelles.NombreDePasDeTemps;
    int Cnt = 0;

    auto& TurbineCible = DonneesAnnuelles.TurbineCible;
    auto& SecondMembre = ProblemeLineairePartieVariable.SecondMembre;

    for (int Pdt = 1; Pdt < NbPdt; Pdt++)
    {
        SecondMembre[Cnt] = DonneesAnnuelles.Apport[Pdt - 1];
        Cnt++;
    }

    SecondMembre[Cnt] = DonneesAnnuelles.Volume[0] - DonneesAnnuelles.Apport[NbPdt - 1];
    Cnt++;

    for (int Pdt = 1; Pdt < NbPdt; Pdt++)
    {
        SecondMembre[Cnt] = DonneesAnnuelles.VolumeMax[Pdt];
        Cnt++;

        SecondMembre[Cnt] = DonneesAnnuelles.VolumeMin[Pdt] * ChgmtSens;
        Cnt++;
    }

    for (int Pdt = 1; Pdt < NbPdt; Pdt++)
    {
        SecondMembre[Cnt] = 0.;
        Cnt++;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        SecondMembre[Cnt] = TurbineCible[Pdt];
        Cnt++;

        SecondMembre[Cnt] = 0.0 * ChgmtSens;
        Cnt++;
    }

    return;
}
