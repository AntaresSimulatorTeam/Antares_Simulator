// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/monthly/h2o_m_donnees_annuelles.h"
#include "antares/solver/hydro/monthly/h2o_m_fonctions.h"

namespace DonneesOptimisationMensuelle
{
void H2O_M_InitialiserLeSecondMembre(DONNEES_ANNUELLES& DonneesAnnuelles)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesAnnuelles.ProblemeHydraulique;
    PROBLEME_LINEAIRE_PARTIE_VARIABLE& ProblemeLineairePartieVariable
      = ProblemeHydraulique.ProblemeLineairePartieVariable;

    double ChgmtSens = -1;
    int Cnt = 0;

    auto& TurbineCible = DonneesAnnuelles.TurbineCible;
    auto& SecondMembre = ProblemeLineairePartieVariable.SecondMembre;

    for (unsigned Pdt = 1; Pdt < nbMonths; Pdt++)
    {
        SecondMembre[Cnt] = DonneesAnnuelles.Apport[Pdt - 1];
        Cnt++;
    }

    SecondMembre[Cnt] = DonneesAnnuelles.Volume[0] - DonneesAnnuelles.Apport[nbMonths - 1];
    Cnt++;

    for (unsigned Pdt = 1; Pdt < nbMonths; Pdt++)
    {
        SecondMembre[Cnt] = DonneesAnnuelles.VolumeMax[Pdt];
        Cnt++;

        SecondMembre[Cnt] = DonneesAnnuelles.VolumeMin[Pdt] * ChgmtSens;
        Cnt++;
    }

    for (unsigned Pdt = 1; Pdt < nbMonths; Pdt++)
    {
        SecondMembre[Cnt] = 0.;
        Cnt++;
    }

    for (unsigned Pdt = 0; Pdt < nbMonths; Pdt++)
    {
        SecondMembre[Cnt] = TurbineCible[Pdt];
        Cnt++;

        SecondMembre[Cnt] = 0.0 * ChgmtSens;
        Cnt++;
    }
}
} // namespace DonneesOptimisationMensuelle
