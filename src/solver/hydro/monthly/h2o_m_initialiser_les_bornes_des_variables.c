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

void H2O_M_InitialiserBornesEtCoutsDesVariables(DONNEES_ANNUELLES* DonneesAnnuelles)
{
    int Pdt;
    int NbPdt;
    int Var;
    double* Xmin;
    double* Xmax;
    double* X;
    double* TurbineMax;
    double* CoutLineaire;
    double CoutDepassementVolume;

    PROBLEME_HYDRAULIQUE* ProblemeHydraulique;
    PROBLEME_LINEAIRE_PARTIE_FIXE* ProblemeLineairePartieFixe;
    PROBLEME_LINEAIRE_PARTIE_VARIABLE* ProblemeLineairePartieVariable;

    DonneesAnnuelles->Volume[0] = DonneesAnnuelles->VolumeInitial;

    NbPdt = DonneesAnnuelles->NombreDePasDeTemps;
    CoutDepassementVolume = DonneesAnnuelles->CoutDepassementVolume;
    TurbineMax = DonneesAnnuelles->TurbineMax;

    ProblemeHydraulique = DonneesAnnuelles->ProblemeHydraulique;
    ProblemeLineairePartieFixe = ProblemeHydraulique->ProblemeLineairePartieFixe;
    ProblemeLineairePartieVariable = ProblemeHydraulique->ProblemeLineairePartieVariable;

    Xmin = ProblemeLineairePartieVariable->Xmin;
    Xmax = ProblemeLineairePartieVariable->Xmax;
    X = ProblemeLineairePartieVariable->X;
    CoutLineaire = ProblemeLineairePartieFixe->CoutLineaire;

    X[0] = DonneesAnnuelles->Volume[0];
    Xmin[0] = DonneesAnnuelles->Volume[0];
    Xmax[0] = DonneesAnnuelles->Volume[0];

    Var = 0;

    for (Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CoutLineaire[Var] = 0.0;
        Var++;
    }

    for (Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        Xmax[Var] = TurbineMax[Pdt];
        CoutLineaire[Var] = 0.0;
        Var++;
    }

    for (Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CoutLineaire[Var] = CoutDepassementVolume;
        Var++;
    }

    for (Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CoutLineaire[Var] = CoutDepassementVolume;
        Var++;
    }

    CoutLineaire[Var] = DonneesAnnuelles->CoutViolMaxDuVolumeMin;
    Var++;

    for (Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CoutLineaire[Var] = 1.0;
        Var++;
    }

    for (Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CoutLineaire[Var] = 1.0;
        Var++;
    }

    CoutLineaire[Var] = 1.0;
    Var++;

    return;
}
