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
    const int NbPdt = DonneesAnnuelles.NombreDePasDeTemps;
    int NombreDeContraintes = 0;
    int il = 0;

    for (int Pdt = 1; Pdt < NbPdt; Pdt++)
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

        Sens[NombreDeContraintes] = '=';
        NombreDeTermesDesLignes[NombreDeContraintes] = 3;
        NombreDeContraintes++;
    }

    IndicesDebutDeLigne[NombreDeContraintes] = il;

    CoefficientsDeLaMatriceDesContraintes[il] = 1.0;
    IndicesColonnes[il] = NumeroDeVariableVolume[NbPdt - 1];
    il++;

    CoefficientsDeLaMatriceDesContraintes[il] = -1.0;
    IndicesColonnes[il] = NumeroDeVariableTurbine[NbPdt - 1];
    il++;

    Sens[NombreDeContraintes] = '=';
    NombreDeTermesDesLignes[NombreDeContraintes] = 2;
    NombreDeContraintes++;

    for (int Pdt = 1; Pdt < NbPdt; Pdt++)
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

    for (int Pdt = 1; Pdt < NbPdt; Pdt++)
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

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
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

    return;
}
