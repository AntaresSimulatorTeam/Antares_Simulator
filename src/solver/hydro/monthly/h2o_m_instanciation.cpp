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

DONNEES_ANNUELLES H2O_M_Instanciation(int NombreDeReservoirs)
{
    DONNEES_ANNUELLES DonneesAnnuelles{};

    DonneesAnnuelles.NombreDePasDeTemps = 12;
    const int NbPdt = DonneesAnnuelles.NombreDePasDeTemps;

    DonneesAnnuelles.TurbineMax.assign(NbPdt, 0.);
    DonneesAnnuelles.TurbineMin.assign(NbPdt, 0.);
    DonneesAnnuelles.TurbineCible.assign(NbPdt, 0.);
    DonneesAnnuelles.Turbine.assign(NbPdt, 0.);

    DonneesAnnuelles.Apport.assign(NbPdt, 0.);

    DonneesAnnuelles.Volume.assign(NbPdt, 0.);
    DonneesAnnuelles.VolumeMin.assign(NbPdt, 0.);
    DonneesAnnuelles.VolumeMax.assign(NbPdt, 0.);

    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesAnnuelles.ProblemeHydraulique;

    ProblemeHydraulique.NombreDeReservoirs = NombreDeReservoirs;

    ProblemeHydraulique.ProblemeSpx.assign(NombreDeReservoirs, nullptr);

    CORRESPONDANCE_DES_VARIABLES& CorrespondanceDesVariables
        = ProblemeHydraulique.CorrespondanceDesVariables;
    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe
        = ProblemeHydraulique.ProblemeLineairePartieFixe;
    PROBLEME_LINEAIRE_PARTIE_VARIABLE& ProblemeLineairePartieVariable
        = ProblemeHydraulique.ProblemeLineairePartieVariable;

    CorrespondanceDesVariables.NumeroDeVariableVolume.assign(NbPdt, 0);
    CorrespondanceDesVariables.NumeroDeVariableTurbine.assign(NbPdt, 0);
    CorrespondanceDesVariables.NumeroDeVariableDepassementVolumeMin.assign(NbPdt, 0);
    CorrespondanceDesVariables.NumeroDeVariableDepassementVolumeMax.assign(NbPdt, 0);
    CorrespondanceDesVariables.NumeroDeVariableDEcartPositifAuTurbineCible.assign(NbPdt, 0);
    CorrespondanceDesVariables.NumeroDeVariableDEcartNegatifAuTurbineCible.assign(NbPdt, 0);

    int NombreDeVariables = 0;
    NombreDeVariables += NbPdt;
    NombreDeVariables += NbPdt;
    NombreDeVariables += NbPdt;
    NombreDeVariables += NbPdt;
    NombreDeVariables += 1;
    NombreDeVariables += NbPdt;
    NombreDeVariables += NbPdt;
    NombreDeVariables += 1;

    ProblemeLineairePartieFixe.NombreDeVariables = NombreDeVariables;

    ProblemeLineairePartieFixe.CoutLineaire.assign(NombreDeVariables, 0.);
    ProblemeLineairePartieFixe.CoutLineaireBruite.assign(NombreDeVariables, 0.);

    ProblemeLineairePartieFixe.TypeDeVariable.assign(NombreDeVariables, 0);

    int NombreDeContraintes = 0;
    NombreDeContraintes += NbPdt;
    NombreDeContraintes += 1;
    NombreDeContraintes += NbPdt;
    NombreDeContraintes += NbPdt;
    NombreDeContraintes += NbPdt;
    NombreDeContraintes += NbPdt;
    NombreDeContraintes += NbPdt;

    ProblemeLineairePartieFixe.NombreDeContraintes = NombreDeContraintes;
    ProblemeLineairePartieFixe.Sens.assign(NombreDeContraintes, 0);

    ProblemeLineairePartieFixe.IndicesDebutDeLigne.assign(NombreDeContraintes, 0);
    ProblemeLineairePartieFixe.NombreDeTermesDesLignes.assign(NombreDeContraintes, 0);

    int NombreDeTermesAlloues = 0;
    NombreDeTermesAlloues += 3 * NbPdt;
    NombreDeTermesAlloues += 2;
    NombreDeTermesAlloues += 2 * NbPdt;
    NombreDeTermesAlloues += 2 * NbPdt;
    NombreDeTermesAlloues += 2 * NbPdt;
    NombreDeTermesAlloues += 3 * NbPdt;
    NombreDeTermesAlloues += 3 * NbPdt;

    ProblemeLineairePartieFixe.NombreDeTermesAlloues = NombreDeTermesAlloues;

    ProblemeLineairePartieFixe.CoefficientsDeLaMatriceDesContraintes
        .assign(NombreDeTermesAlloues, 0.);

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
