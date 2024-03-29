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

void H2O_M_OptimiserUneAnnee(DONNEES_ANNUELLES& DonneesAnnuelles, int NumeroDeReservoir)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesAnnuelles.ProblemeHydraulique;

    if (NumeroDeReservoir < 0 || NumeroDeReservoir > ProblemeHydraulique.NombreDeReservoirs)
    {
        DonneesAnnuelles.ResultatsValides = EMERGENCY_SHUT_DOWN;
        return;
    }

    DonneesAnnuelles.ResultatsValides = NON;

    H2O_M_InitialiserBornesEtCoutsDesVariables(DonneesAnnuelles);
    H2O_M_AjouterBruitAuCout(DonneesAnnuelles);
    H2O_M_InitialiserLeSecondMembre(DonneesAnnuelles);
    H2O_M_ResoudreLeProblemeLineaire(DonneesAnnuelles, NumeroDeReservoir);

    return;
}
