/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: MPL 2.0
*/

#include "h2o_m_donnees_annuelles.h"
#include "h2o_m_fonctions.h"

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
