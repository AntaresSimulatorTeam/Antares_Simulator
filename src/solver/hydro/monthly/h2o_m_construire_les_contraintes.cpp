// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/monthly/h2o_m_donnees_annuelles.h"
#include "antares/solver/hydro/monthly/h2o_m_fonctions.h"

namespace DonneesOptimisationMensuelle
{
void H2O_M_ConstruireLesContraintes(DONNEES_ANNUELLES& DonneesAnnuelles)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesAnnuelles.ProblemeHydraulique;
    CORRESPONDANCE_DES_VARIABLES& CorrespondanceDesVariables = ProblemeHydraulique
                                                                 .CorrespondanceDesVariables;
    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe = ProblemeHydraulique
                                                                  .ProblemeLineairePartieFixe;

    auto& CoefficientsDeLaMatriceDesContraintes = ProblemeLineairePartieFixe
                                                    .CoefficientsDeLaMatriceDesContraintes;
    auto& IndicesColonnes = ProblemeLineairePartieFixe.IndicesColonnes;
    auto& IndicesDebutDeLigne = ProblemeLineairePartieFixe.IndicesDebutDeLigne;
    auto& Sens = ProblemeLineairePartieFixe.Sens;
    auto& NombreDeTermesDesLignes = ProblemeLineairePartieFixe.NombreDeTermesDesLignes;

    auto& NumeroDeVariableVolume = CorrespondanceDesVariables.NumeroDeVariableVolume;
    auto& NumeroDeVariableTurbine = CorrespondanceDesVariables.NumeroDeVariableTurbine;
    auto& NumeroDeVariableOverflow = CorrespondanceDesVariables.NumeroDeVariableOverflow;
    auto& NumeroDeVariableDepassementVolumeMax = CorrespondanceDesVariables
                                                   .NumeroDeVariableDepassementVolumeMax;
    auto& NumeroDeVariableDepassementVolumeMin = CorrespondanceDesVariables
                                                   .NumeroDeVariableDepassementVolumeMin;
    auto& NumeroDeVariableDEcartPositifAuTurbineCible
      = CorrespondanceDesVariables.NumeroDeVariableDEcartPositifAuTurbineCible;
    auto& NumeroDeVariableDEcartNegatifAuTurbineCible
      = CorrespondanceDesVariables.NumeroDeVariableDEcartNegatifAuTurbineCible;
    int NumeroDeLaVariableXi = CorrespondanceDesVariables.NumeroDeLaVariableXi;

    double ChgmtSens = -1.0;
    int NombreDeContraintes = 0;
    int il = 0;

    for (unsigned Pdt = 1; Pdt < nbMonths; Pdt++)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = NumeroDeVariableVolume[Pdt];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
        IndicesColonnes[il] = NumeroDeVariableVolume[Pdt - 1];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = NumeroDeVariableTurbine[Pdt - 1];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = NumeroDeVariableOverflow[Pdt - 1];
        il++;

        Sens[NombreDeContraintes] = '=';
        NombreDeTermesDesLignes[NombreDeContraintes] = 4;
        NombreDeContraintes++;
    }

    IndicesDebutDeLigne[NombreDeContraintes] = il;

    CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
    IndicesColonnes[il] = NumeroDeVariableVolume[nbMonths - 1];
    il++;

    CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
    IndicesColonnes[il] = NumeroDeVariableTurbine[nbMonths - 1];
    il++;

    CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
    IndicesColonnes[il] = NumeroDeVariableOverflow[nbMonths - 1];
    il++;

    Sens[NombreDeContraintes] = '=';
    NombreDeTermesDesLignes[NombreDeContraintes] = 3;
    NombreDeContraintes++;

    for (unsigned Pdt = 1; Pdt < nbMonths; Pdt++)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = NumeroDeVariableVolume[Pdt];
        il++;

        ProblemeLineairePartieFixe.CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
        ProblemeLineairePartieFixe.IndicesColonnes[il] = NumeroDeVariableDepassementVolumeMax[Pdt];
        il++;

        Sens[NombreDeContraintes] = '<';
        NombreDeTermesDesLignes[NombreDeContraintes] = 2;
        NombreDeContraintes++;

        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0 * ChgmtSens;
        IndicesColonnes[il] = NumeroDeVariableVolume[Pdt];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0 * ChgmtSens;
        IndicesColonnes[il] = NumeroDeVariableDepassementVolumeMin[Pdt];
        il++;

        Sens[NombreDeContraintes] = '<';
        NombreDeTermesDesLignes[NombreDeContraintes] = 2;
        NombreDeContraintes++;
    }

    for (unsigned Pdt = 1; Pdt < nbMonths; Pdt++)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = NumeroDeVariableDepassementVolumeMin[Pdt];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
        IndicesColonnes[il] = CorrespondanceDesVariables.NumeroDeLaVariableViolMaxVolumeMin;
        il++;

        Sens[NombreDeContraintes] = '<';
        NombreDeTermesDesLignes[NombreDeContraintes] = 2;
        NombreDeContraintes++;
    }

    for (unsigned Pdt = 0; Pdt < nbMonths; Pdt++)
    {
        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = NumeroDeVariableTurbine[Pdt];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
        IndicesColonnes[il] = NumeroDeVariableDEcartPositifAuTurbineCible[Pdt];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
        IndicesColonnes[il] = NumeroDeVariableDEcartNegatifAuTurbineCible[Pdt];
        il++;

        Sens[NombreDeContraintes] = '=';
        NombreDeTermesDesLignes[NombreDeContraintes] = 3;
        NombreDeContraintes++;

        IndicesDebutDeLigne[NombreDeContraintes] = il;

        CoefficientsDeLaMatriceDesContraintes[il] = 1.0 * ChgmtSens;
        IndicesColonnes[il] = NumeroDeLaVariableXi;
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = -1.0 * ChgmtSens;
        IndicesColonnes[il] = NumeroDeVariableDEcartPositifAuTurbineCible[Pdt];
        il++;

        CoefficientsDeLaMatriceDesContraintes[il] = -1.0 * ChgmtSens;
        IndicesColonnes[il] = NumeroDeVariableDEcartNegatifAuTurbineCible[Pdt];
        il++;

        Sens[NombreDeContraintes] = '<';
        NombreDeTermesDesLignes[NombreDeContraintes] = 3;
        NombreDeContraintes++;
    }

    ProblemeLineairePartieFixe.NombreDeContraintes = NombreDeContraintes;
}
} // namespace DonneesOptimisationMensuelle
