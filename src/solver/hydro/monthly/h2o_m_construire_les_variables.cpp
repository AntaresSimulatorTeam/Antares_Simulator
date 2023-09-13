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

#ifdef __cplusplus
extern "C"
{
#endif

#include "spx_constantes_externes.h"

#ifdef __cplusplus
}
#endif

#include "h2o_m_donnees_annuelles.h"
#include "h2o_m_fonctions.h"

void H2O_M_ConstruireLesVariables(DONNEES_ANNUELLES& DonneesAnnuelles)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesAnnuelles.ProblemeHydraulique;
    PROBLEME_LINEAIRE_PARTIE_VARIABLE& ProblemeLineairePartieVariable
        = ProblemeHydraulique.ProblemeLineairePartieVariable;
    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe
        = ProblemeHydraulique.ProblemeLineairePartieFixe;
    CORRESPONDANCE_DES_VARIABLES& CorrespondanceDesVariables
        = ProblemeHydraulique.CorrespondanceDesVariables;

    int NbPdt = DonneesAnnuelles.NombreDePasDeTemps;
    int Var = 0;

    CorrespondanceDesVariables.NumeroDeVariableVolume[0] = Var;
    ProblemeLineairePartieVariable.Xmin[Var] = 0.0;
    ProblemeLineairePartieVariable.Xmax[Var] = 1.0;
    ProblemeLineairePartieFixe.TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
    ProblemeLineairePartieVariable.AdresseOuPlacerLaValeurDesVariablesOptimisees[Var]
      = &(DonneesAnnuelles.Volume[0]);
    Var++;

    for (int Pdt = 1; Pdt < NbPdt; Pdt++)
    {
        CorrespondanceDesVariables.NumeroDeVariableVolume[Pdt] = Var;
        ProblemeLineairePartieVariable.Xmin[Var] = 0.0;
        ProblemeLineairePartieVariable.Xmax[Var] = 1.0;
        ProblemeLineairePartieFixe.TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
        ProblemeLineairePartieVariable.AdresseOuPlacerLaValeurDesVariablesOptimisees[Var]
          = &(DonneesAnnuelles.Volume[Pdt]);
        Var++;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CorrespondanceDesVariables.NumeroDeVariableTurbine[Pdt] = Var;
        ProblemeLineairePartieVariable.Xmin[Var] = 0.0;
        ProblemeLineairePartieVariable.Xmax[Var] = 0.0;
        ProblemeLineairePartieFixe.TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
        ProblemeLineairePartieVariable.AdresseOuPlacerLaValeurDesVariablesOptimisees[Var]
          = &(DonneesAnnuelles.Turbine[Pdt]);
        Var++;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CorrespondanceDesVariables.NumeroDeVariableDepassementVolumeMax[Pdt] = Var;
        ProblemeLineairePartieVariable.Xmin[Var] = 0.0;
        ProblemeLineairePartieVariable.Xmax[Var] = LINFINI;
        ProblemeLineairePartieFixe.TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
        Var++;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
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

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        CorrespondanceDesVariables.NumeroDeVariableDEcartPositifAuTurbineCible[Pdt] = Var;
        ProblemeLineairePartieVariable.Xmin[Var] = 0.0;
        ProblemeLineairePartieVariable.Xmax[Var] = LINFINI;
        ProblemeLineairePartieFixe.TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
        Var++;
    }

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
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
