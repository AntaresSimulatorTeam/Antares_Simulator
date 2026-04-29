// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/monthly/h2o_m_donnees_annuelles.h"
#include "antares/solver/hydro/monthly/h2o_m_fonctions.h"

namespace DonneesOptimisationMensuelle
{
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

    double CoutDepassementVolume = DonneesAnnuelles.CoutDepassementVolume;
    double overflowfCost = DonneesAnnuelles.overflowfCost;
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

    for (unsigned Pdt = 0; Pdt < nbMonths; Pdt++)
    {
        Var = CorrespondanceDesVariables.NumeroDeVariableVolume[Pdt];
        CoutLineaire[Var] = 0.0;
    }

    for (unsigned Pdt = 0; Pdt < nbMonths; Pdt++)
    {
        Var = CorrespondanceDesVariables.NumeroDeVariableTurbine[Pdt];
        Xmax[Var] = TurbineMax[Pdt];
        Xmin[Var] = TurbineMin[Pdt];
        CoutLineaire[Var] = 0.0;
    }

    for (unsigned Pdt = 0; Pdt < nbMonths; Pdt++)
    {
        Var = CorrespondanceDesVariables.NumeroDeVariableOverflow[Pdt];
        CoutLineaire[Var] = overflowfCost;
    }

    for (unsigned Pdt = 0; Pdt < nbMonths; Pdt++)
    {
        Var = CorrespondanceDesVariables.NumeroDeVariableDepassementVolumeMax[Pdt];
        CoutLineaire[Var] = CoutDepassementVolume;
    }

    for (unsigned Pdt = 0; Pdt < nbMonths; Pdt++)
    {
        Var = CorrespondanceDesVariables.NumeroDeVariableDepassementVolumeMin[Pdt];
        CoutLineaire[Var] = CoutDepassementVolume;
    }

    Var = CorrespondanceDesVariables.NumeroDeLaVariableViolMaxVolumeMin;
    CoutLineaire[Var] = DonneesAnnuelles.CoutViolMaxDuVolumeMin;

    for (unsigned Pdt = 0; Pdt < nbMonths; Pdt++)
    {
        Var = CorrespondanceDesVariables.NumeroDeVariableDEcartPositifAuTurbineCible[Pdt];
        CoutLineaire[Var] = 1.0;
    }

    for (unsigned Pdt = 0; Pdt < nbMonths; Pdt++)
    {
        Var = CorrespondanceDesVariables.NumeroDeVariableDEcartNegatifAuTurbineCible[Pdt];
        CoutLineaire[Var] = 1.0;
    }

    Var = CorrespondanceDesVariables.NumeroDeLaVariableXi;
    CoutLineaire[Var] = 1.0;
}
} // namespace DonneesOptimisationMensuelle
