// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/monthly/h2o_m_donnees_annuelles.h"
#include "antares/solver/hydro/monthly/h2o_m_fonctions.h"

namespace DonneesOptimisationMensuelle
{
DONNEES_ANNUELLES H2O_M_Instanciation(int NombreDeReservoirs)
{
    DONNEES_ANNUELLES DonneesAnnuelles{};

    DonneesAnnuelles.TurbineMax.assign(nbMonths, 0.);
    DonneesAnnuelles.TurbineMin.assign(nbMonths, 0.);
    DonneesAnnuelles.TurbineCible.assign(nbMonths, 0.);
    DonneesAnnuelles.Turbine.assign(nbMonths, 0.);
    DonneesAnnuelles.overflow.assign(nbMonths, 0.);
    DonneesAnnuelles.Apport.assign(nbMonths, 0.);

    DonneesAnnuelles.Volume.assign(nbMonths, 0.);
    DonneesAnnuelles.VolumeMin.assign(nbMonths, 0.);
    DonneesAnnuelles.VolumeMax.assign(nbMonths, 0.);

    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesAnnuelles.ProblemeHydraulique;

    ProblemeHydraulique.NombreDeReservoirs = NombreDeReservoirs;

    ProblemeHydraulique.ProblemeSpx.resize(NombreDeReservoirs);

    CORRESPONDANCE_DES_VARIABLES& CorrespondanceDesVariables = ProblemeHydraulique
                                                                 .CorrespondanceDesVariables;
    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe = ProblemeHydraulique
                                                                  .ProblemeLineairePartieFixe;
    PROBLEME_LINEAIRE_PARTIE_VARIABLE& ProblemeLineairePartieVariable
      = ProblemeHydraulique.ProblemeLineairePartieVariable;

    CorrespondanceDesVariables.NumeroDeVariableVolume.assign(nbMonths, 0);
    CorrespondanceDesVariables.NumeroDeVariableTurbine.assign(nbMonths, 0);
    CorrespondanceDesVariables.NumeroDeVariableOverflow.assign(nbMonths, 0);
    CorrespondanceDesVariables.NumeroDeVariableDepassementVolumeMin.assign(nbMonths, 0);
    CorrespondanceDesVariables.NumeroDeVariableDepassementVolumeMax.assign(nbMonths, 0);
    CorrespondanceDesVariables.NumeroDeVariableDEcartPositifAuTurbineCible.assign(nbMonths, 0);
    CorrespondanceDesVariables.NumeroDeVariableDEcartNegatifAuTurbineCible.assign(nbMonths, 0);

    int NombreDeVariables = 0;
    NombreDeVariables += nbMonths;
    NombreDeVariables += nbMonths;
    NombreDeVariables += nbMonths;
    NombreDeVariables += nbMonths;
    NombreDeVariables += nbMonths; // For overflows
    NombreDeVariables += 1;
    NombreDeVariables += nbMonths;
    NombreDeVariables += nbMonths;
    NombreDeVariables += 1;

    ProblemeLineairePartieFixe.NombreDeVariables = NombreDeVariables;

    ProblemeLineairePartieFixe.CoutLineaire.assign(NombreDeVariables, 0.);
    ProblemeLineairePartieFixe.CoutLineaireBruite.assign(NombreDeVariables, 0.);

    ProblemeLineairePartieFixe.TypeDeVariable.assign(NombreDeVariables, 0);

    int NombreDeContraintes = 0;
    NombreDeContraintes += nbMonths;
    NombreDeContraintes += 1;
    NombreDeContraintes += nbMonths;
    NombreDeContraintes += nbMonths;
    NombreDeContraintes += nbMonths;
    NombreDeContraintes += nbMonths;
    NombreDeContraintes += nbMonths;

    ProblemeLineairePartieFixe.NombreDeContraintes = NombreDeContraintes;
    ProblemeLineairePartieFixe.Sens.assign(NombreDeContraintes, 0);

    ProblemeLineairePartieFixe.IndicesDebutDeLigne.assign(NombreDeContraintes, 0);
    ProblemeLineairePartieFixe.NombreDeTermesDesLignes.assign(NombreDeContraintes, 0);

    int NombreDeTermesAlloues = 0;
    NombreDeTermesAlloues += 3 * nbMonths;
    NombreDeTermesAlloues += nbMonths; // For overflows
    NombreDeTermesAlloues += 2;
    NombreDeTermesAlloues += 2 * nbMonths;
    NombreDeTermesAlloues += 2 * nbMonths;
    NombreDeTermesAlloues += 2 * nbMonths;
    NombreDeTermesAlloues += 3 * nbMonths;
    NombreDeTermesAlloues += 3 * nbMonths;

    ProblemeLineairePartieFixe.NombreDeTermesAlloues = NombreDeTermesAlloues;

    ProblemeLineairePartieFixe.CoefficientsDeLaMatriceDesContraintes.assign(NombreDeTermesAlloues,
                                                                            0.);

    ProblemeLineairePartieFixe.IndicesColonnes.assign(NombreDeTermesAlloues, 0);

    ProblemeLineairePartieVariable.Xmin.assign(NombreDeVariables, 0.);
    ProblemeLineairePartieVariable.Xmax.assign(NombreDeVariables, 0.);
    ProblemeLineairePartieVariable.SecondMembre.assign(NombreDeContraintes, 0.);

    ProblemeLineairePartieVariable.AdresseOuPlacerLaValeurDesVariablesOptimisees
      .assign(NombreDeVariables, nullptr);

    ProblemeLineairePartieVariable.X.assign(NombreDeVariables, 0.);

    ProblemeLineairePartieVariable.PositionDeLaVariable.assign(NombreDeVariables, 0);
    ProblemeLineairePartieVariable.ComplementDeLaBase.assign(NombreDeContraintes, 0);

    ProblemeLineairePartieVariable.CoutsReduits.assign(NombreDeVariables, 0.);
    ProblemeLineairePartieVariable.CoutsMarginauxDesContraintes.assign(NombreDeContraintes, 0.);

    H2O_M_ConstruireLesVariables(DonneesAnnuelles);

    H2O_M_ConstruireLesContraintes(DonneesAnnuelles);

    return DonneesAnnuelles;
}
} // namespace DonneesOptimisationMensuelle
