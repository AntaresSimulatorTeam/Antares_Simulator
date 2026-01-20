// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/monthly/h2o_m_donnees_annuelles.h"
#include "antares/solver/hydro/monthly/h2o_m_fonctions.h"

#include "spx_constantes_externes.h"

namespace DonneesOptimisationMensuelle
{
void H2O_M_ConstruireLesVariables(DONNEES_ANNUELLES& DonneesAnnuelles)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesAnnuelles.ProblemeHydraulique;
    PROBLEME_LINEAIRE_PARTIE_VARIABLE& ProblemeLineairePartieVariable
      = ProblemeHydraulique.ProblemeLineairePartieVariable;
    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe = ProblemeHydraulique
                                                                  .ProblemeLineairePartieFixe;
    CORRESPONDANCE_DES_VARIABLES& CorrespondanceDesVariables = ProblemeHydraulique
                                                                 .CorrespondanceDesVariables;
    int Var = 0;

    for (unsigned Pdt = 0; Pdt < nbMonths; Pdt++)
    {
        CorrespondanceDesVariables.NumeroDeVariableVolume[Pdt] = Var;
        ProblemeLineairePartieVariable.Xmin[Var] = 0.0;
        ProblemeLineairePartieVariable.Xmax[Var] = 1.0;
        ProblemeLineairePartieFixe.TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
        ProblemeLineairePartieVariable.AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(
          DonneesAnnuelles.Volume[Pdt]);
        Var++;
    }

    for (unsigned Pdt = 0; Pdt < nbMonths; Pdt++)
    {
        CorrespondanceDesVariables.NumeroDeVariableTurbine[Pdt] = Var;
        ProblemeLineairePartieVariable.Xmin[Var] = 0.0;
        ProblemeLineairePartieVariable.Xmax[Var] = 0.0;
        ProblemeLineairePartieFixe.TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
        ProblemeLineairePartieVariable.AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(
          DonneesAnnuelles.Turbine[Pdt]);
        Var++;
    }

    for (unsigned Pdt = 0; Pdt < nbMonths; Pdt++)
    {
        CorrespondanceDesVariables.NumeroDeVariableOverflow[Pdt] = Var;
        ProblemeLineairePartieVariable.Xmin[Var] = 0.0;
        ProblemeLineairePartieVariable.Xmax[Var] = LINFINI;
        ProblemeLineairePartieFixe.TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
        ProblemeLineairePartieVariable.AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(
          DonneesAnnuelles.overflow[Pdt]);
        Var++;
    }

    for (unsigned Pdt = 0; Pdt < nbMonths; Pdt++)
    {
        CorrespondanceDesVariables.NumeroDeVariableDepassementVolumeMax[Pdt] = Var;
        ProblemeLineairePartieVariable.Xmin[Var] = 0.0;
        ProblemeLineairePartieVariable.Xmax[Var] = LINFINI;
        ProblemeLineairePartieFixe.TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
        Var++;
    }

    for (unsigned Pdt = 0; Pdt < nbMonths; Pdt++)
    {
        CorrespondanceDesVariables.NumeroDeVariableDepassementVolumeMin[Pdt] = Var;
        ProblemeLineairePartieVariable.Xmin[Var] = 0.0;
        ProblemeLineairePartieVariable.Xmax[Var] = LINFINI;
        ProblemeLineairePartieFixe.TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
        Var++;
    }

    CorrespondanceDesVariables.NumeroDeLaVariableViolMaxVolumeMin = Var;
    ProblemeLineairePartieVariable.Xmin[Var] = 0.0;
    ProblemeLineairePartieVariable.Xmax[Var] = LINFINI;
    ProblemeLineairePartieFixe.TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
    Var++;

    for (unsigned Pdt = 0; Pdt < nbMonths; Pdt++)
    {
        CorrespondanceDesVariables.NumeroDeVariableDEcartPositifAuTurbineCible[Pdt] = Var;
        ProblemeLineairePartieVariable.Xmin[Var] = 0.0;
        ProblemeLineairePartieVariable.Xmax[Var] = LINFINI;
        ProblemeLineairePartieFixe.TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
        Var++;
    }

    for (unsigned Pdt = 0; Pdt < nbMonths; Pdt++)
    {
        CorrespondanceDesVariables.NumeroDeVariableDEcartNegatifAuTurbineCible[Pdt] = Var;
        ProblemeLineairePartieVariable.Xmin[Var] = 0.0;
        ProblemeLineairePartieVariable.Xmax[Var] = LINFINI;
        ProblemeLineairePartieFixe.TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
        Var++;
    }

    CorrespondanceDesVariables.NumeroDeLaVariableXi = Var;
    ProblemeLineairePartieVariable.Xmin[Var] = -LINFINI;
    ProblemeLineairePartieVariable.Xmax[Var] = LINFINI;
    ProblemeLineairePartieFixe.TypeDeVariable[Var] = VARIABLE_NON_BORNEE;
    Var++;

    ProblemeLineairePartieFixe.NombreDeVariables = Var;

    return;
}
} // namespace DonneesOptimisationMensuelle
