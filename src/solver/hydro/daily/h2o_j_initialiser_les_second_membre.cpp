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

#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

void H2O_J_InitialiserLeSecondMembre(DONNEES_MENSUELLES* DonneesMensuelles, int NumeroDeProbleme)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;

    CORRESPONDANCE_DES_CONTRAINTES& CorrespondanceDesContraintes = ProblemeHydraulique
                                                                     .CorrespondanceDesContraintes
                                                                       [NumeroDeProbleme];

    int NumeroDeContrainteDEnergieMensuelle = CorrespondanceDesContraintes
                                                .NumeroDeContrainteDEnergieMensuelle;

    std::vector<double>& SecondMembre = ProblemeHydraulique
                                          .ProblemeLineairePartieVariable[NumeroDeProbleme]
                                          .SecondMembre;

    SecondMembre[NumeroDeContrainteDEnergieMensuelle] = DonneesMensuelles->TurbineDuMois;

    const int NbPdt = ProblemeHydraulique.NbJoursDUnProbleme[NumeroDeProbleme];
    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        int Cnt = CorrespondanceDesContraintes.NumeroDeContrainteSurXi[Pdt];
        SecondMembre[Cnt] = DonneesMensuelles->TurbineCible[Pdt];
    }

    return;
}
