/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "h2o_m_donnees_annuelles.h"
#include "h2o_m_fonctions.h"

void H2O_M_ConstruireLesContraintes(DONNEES_ANNUELLES& DonneesAnnuelles)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesAnnuelles.ProblemeHydraulique;
    CORRESPONDANCE_DES_VARIABLES& CorrespondanceDesVariables
        = ProblemeHydraulique.CorrespondanceDesVariables;
    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe
        = ProblemeHydraulique.ProblemeLineairePartieFixe;

    auto& CoefficientsDeLaMatriceDesContraintes
      = ProblemeLineairePartieFixe.CoefficientsDeLaMatriceDesContraintes;
    auto& IndicesColonnes = ProblemeLineairePartieFixe.IndicesColonnes;
    auto& IndicesDebutDeLigne = ProblemeLineairePartieFixe.IndicesDebutDeLigne;
    auto& Sens = ProblemeLineairePartieFixe.Sens;
    auto& NombreDeTermesDesLignes = ProblemeLineairePartieFixe.NombreDeTermesDesLignes;

    auto& NumeroDeVariableVolume = CorrespondanceDesVariables.NumeroDeVariableVolume;
    auto& NumeroDeVariableTurbine = CorrespondanceDesVariables.NumeroDeVariableTurbine;
    auto& NumeroDeVariableDepassementVolumeMax
      = CorrespondanceDesVariables.NumeroDeVariableDepassementVolumeMax;
    auto& NumeroDeVariableDepassementVolumeMin
      = CorrespondanceDesVariables.NumeroDeVariableDepassementVolumeMin;
    auto& NumeroDeVariableDEcartPositifAuTurbineCible
      = CorrespondanceDesVariables.NumeroDeVariableDEcartPositifAuTurbineCible;
    auto& NumeroDeVariableDEcartNegatifAuTurbineCible
      = CorrespondanceDesVariables.NumeroDeVariableDEcartNegatifAuTurbineCible;
    int NumeroDeLaVariableXi = CorrespondanceDesVariables.NumeroDeLaVariableXi;

    double ChgmtSens = -1.0;
    int NbPdt = DonneesAnnuelles.NombreDePasDeTemps;
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
