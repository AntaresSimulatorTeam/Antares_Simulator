// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/monthly/h2o_m_donnees_annuelles.h"
#include "antares/solver/hydro/monthly/h2o_m_fonctions.h"

namespace DonneesOptimisationMensuelle
{
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
} // namespace DonneesOptimisationMensuelle
