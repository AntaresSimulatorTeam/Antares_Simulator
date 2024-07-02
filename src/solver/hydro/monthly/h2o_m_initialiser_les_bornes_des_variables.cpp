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

void H2O_M_InitialiserBornesEtCoutsDesVariables(DONNEES_ANNUELLES& DonneesAnnuelles)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesAnnuelles.ProblemeHydraulique;
    PROBLEME_LINEAIRE_PARTIE_VARIABLE& ProblemeLineairePartieVariable
      = ProblemeHydraulique.ProblemeLineairePartieVariable;
    const CORRESPONDANCE_DES_VARIABLES& CorrespondanceDesVariables = ProblemeHydraulique
                                                                       .CorrespondanceDesVariables;
    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe = ProblemeHydraulique
                                                                  .ProblemeLineairePartieFixe;

    DonneesAnnuelles.Volume[0] = DonneesAnnuelles.VolumeInitial;

    const int NbPdt = DonneesAnnuelles.NombreDePasDeTemps;
    double CoutDepassementVolume = DonneesAnnuelles.CoutDepassementVolume;
    auto& TurbineMax = DonneesAnnuelles.TurbineMax;
    const auto& TurbineMin = DonneesAnnuelles.TurbineMin;

    auto& Xmin = ProblemeLineairePartieVariable.Xmin;
    auto& Xmax = ProblemeLineairePartieVariable.Xmax;
    auto& X = ProblemeLineairePartieVariable.X;
    auto& CoutLineaire = ProblemeLineairePartieFixe.CoutLineaire;

    int Var = CorrespondanceDesVariables.NumeroDeVariableVolume[0];
    X[Var] = DonneesAnnuelles.Volume[0];
    Xmin[Var] = DonneesAnnuelles.Volume[0];
    Xmax[Var] = DonneesAnnuelles.Volume[0];

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        Var = CorrespondanceDesVariables.NumeroDeVariableVolume[Pdt];
        CoutLineaire[Var] = 0.0;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        Var = CorrespondanceDesVariables.NumeroDeVariableTurbine[Pdt];
        Xmax[Var] = TurbineMax[Pdt];
        Xmin[Var] = TurbineMin[Pdt];
        CoutLineaire[Var] = 0.0;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        Var = CorrespondanceDesVariables.NumeroDeVariableDepassementVolumeMax[Pdt];
        CoutLineaire[Var] = CoutDepassementVolume;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        Var = CorrespondanceDesVariables.NumeroDeVariableDepassementVolumeMin[Pdt];
        CoutLineaire[Var] = CoutDepassementVolume;
    }

    Var = CorrespondanceDesVariables.NumeroDeLaVariableViolMaxVolumeMin;
    CoutLineaire[Var] = DonneesAnnuelles.CoutViolMaxDuVolumeMin;

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        Var = CorrespondanceDesVariables.NumeroDeVariableDEcartPositifAuTurbineCible[Pdt];
        CoutLineaire[Var] = 1.0;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        Var = CorrespondanceDesVariables.NumeroDeVariableDEcartNegatifAuTurbineCible[Pdt];
        CoutLineaire[Var] = 1.0;
    }

    Var = CorrespondanceDesVariables.NumeroDeLaVariableXi;
    CoutLineaire[Var] = 1.0;
}
