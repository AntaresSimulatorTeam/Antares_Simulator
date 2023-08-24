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

#include "h2o_j_donnees_mensuelles.h"
#include "h2o_j_fonctions.h"
#include <algorithm>

void H2O_J_InitialiserLesBornesdesVariables(DONNEES_MENSUELLES* DonneesMensuelles,
                                            int NumeroDeProbleme)
{
    int NbPdt;
    int Var;
    const double* TurbineMax;
    const double* TurbineMin;
    double* Turbine;
    const double* TurbineCible;

    PROBLEME_HYDRAULIQUE* ProblemeHydraulique;

    TurbineMax = DonneesMensuelles->TurbineMax;
    TurbineMin = DonneesMensuelles->TurbineMin;
    Turbine = DonneesMensuelles->Turbine;
    TurbineCible = DonneesMensuelles->TurbineCible;

    ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;

    NbPdt = ProblemeHydraulique->NbJoursDUnProbleme[NumeroDeProbleme];

    const CORRESPONDANCE_DES_VARIABLES& CorrespondanceDesVariables
        = ProblemeHydraulique->CorrespondanceDesVariables[NumeroDeProbleme];

    PROBLEME_LINEAIRE_PARTIE_VARIABLE& ProblemeLineairePartieVariable
        = ProblemeHydraulique->ProblemeLineairePartieVariable[NumeroDeProbleme];

    std::vector<double>& Xmin = ProblemeLineairePartieVariable.Xmin;
    std::vector<double>& Xmax = ProblemeLineairePartieVariable.Xmax;
    std::vector<double*>& AdresseOuPlacerLaValeurDesVariablesOptimisees
        = ProblemeLineairePartieVariable.AdresseOuPlacerLaValeurDesVariablesOptimisees;

    for (int Pdt = 0; Pdt < NbPdt; Pdt++)
    {
        Var = CorrespondanceDesVariables.NumeroDeVariableTurbine[Pdt];
        Xmax[Var] = TurbineMax[Pdt];

        Xmin[Var] = std::min(TurbineMax[Pdt], std::max(TurbineCible[Pdt], TurbineMin[Pdt]));

        AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(Turbine[Pdt]);
    }

    return;
}
