/*
** Copyright 2007-2018 RTE
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

#include <limits>

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_structure_probleme_economique.h"
#include "../simulation/sim_structure_probleme_adequation.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"

extern "C"
{
#include "pi_define.h"
#include "pi_definition_arguments.h"
#include "pi_fonctions.h"
}

#include <antares/logs.h>

using namespace Antares;

bool OPT_AppelDuSolveurQuadratique(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
                                   const int PdtHebdo)
{
    int Var;
    double* pt;
    double ToleranceSurLAdmissibilite;
    int ChoixToleranceParDefautSurLAdmissibilite;
    double ToleranceSurLaStationnarite;
    int ChoixToleranceParDefautSurLaStationnarite;
    double ToleranceSurLaComplementarite;
    int ChoixToleranceParDefautSurLaComplementarite;

    PROBLEME_POINT_INTERIEUR Probleme;

    ToleranceSurLAdmissibilite = 1.e-5;
    ChoixToleranceParDefautSurLAdmissibilite = OUI_PI;

    ToleranceSurLaStationnarite = 1.e-5;
    ChoixToleranceParDefautSurLaStationnarite = OUI_PI;

    ToleranceSurLaComplementarite = 1.e-5;
    ChoixToleranceParDefautSurLaComplementarite = OUI_PI;

    Probleme.NombreMaxDIterations = -1;
    Probleme.CoutQuadratique = ProblemeAResoudre->CoutQuadratique;
    Probleme.CoutLineaire = ProblemeAResoudre->CoutLineaire;
    Probleme.X = ProblemeAResoudre->X;
    Probleme.Xmin = ProblemeAResoudre->Xmin;
    Probleme.Xmax = ProblemeAResoudre->Xmax;
    Probleme.NombreDeVariables = ProblemeAResoudre->NombreDeVariables;
    Probleme.TypeDeVariable = ProblemeAResoudre->TypeDeVariable;

    Probleme.VariableBinaire = (char*)ProblemeAResoudre->CoutsReduits;

    Probleme.NombreDeContraintes = ProblemeAResoudre->NombreDeContraintes;
    Probleme.IndicesDebutDeLigne = ProblemeAResoudre->IndicesDebutDeLigne;
    Probleme.NombreDeTermesDesLignes = ProblemeAResoudre->NombreDeTermesDesLignes;
    Probleme.IndicesColonnes = ProblemeAResoudre->IndicesColonnes;
    Probleme.CoefficientsDeLaMatriceDesContraintes
      = ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes;
    Probleme.Sens = ProblemeAResoudre->Sens;
    Probleme.SecondMembre = ProblemeAResoudre->SecondMembre;

    Probleme.AffichageDesTraces = NON_PI;

    Probleme.UtiliserLaToleranceDAdmissibiliteParDefaut = ChoixToleranceParDefautSurLAdmissibilite;
    Probleme.ToleranceDAdmissibilite = ToleranceSurLAdmissibilite;

    Probleme.UtiliserLaToleranceDeStationnariteParDefaut
      = ChoixToleranceParDefautSurLaStationnarite;
    Probleme.ToleranceDeStationnarite = ToleranceSurLaStationnarite;

    Probleme.UtiliserLaToleranceDeComplementariteParDefaut
      = ChoixToleranceParDefautSurLaComplementarite;
    Probleme.ToleranceDeComplementarite = ToleranceSurLaComplementarite;

    Probleme.CoutsMarginauxDesContraintes = ProblemeAResoudre->CoutsMarginauxDesContraintes;

    Probleme.CoutsMarginauxDesContraintesDeBorneInf = ProblemeAResoudre->CoutsReduits;
    Probleme.CoutsMarginauxDesContraintesDeBorneSup = ProblemeAResoudre->CoutsReduits;

    PI_Quamin(&Probleme);

    ProblemeAResoudre->ExistenceDUneSolution = Probleme.ExistenceDUneSolution;
    if (ProblemeAResoudre->ExistenceDUneSolution == OUI_PI)
    {
        for (Var = 0; Var < ProblemeAResoudre->NombreDeVariables; Var++)
        {
            pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var];
            if (pt)
                *pt = ProblemeAResoudre->X[Var];
        }

        return true;
    }
    else
    {
        logs.warning() << "Quadratic Optimisation: No solution, hour " << PdtHebdo;

        for (Var = 0; Var < ProblemeAResoudre->NombreDeVariables; Var++)
        {
            pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var];
            if (pt)
                *pt = std::numeric_limits<double>::quiet_NaN();
        }

#ifndef NDEBUG

        {
            logs.info();

            logs.info() << LOG_UI_DISPLAY_MESSAGES_OFF;

            logs.info() << "Here is the trace:";
            for (Var = 0; Var < ProblemeAResoudre->NombreDeVariables; Var++)
            {
                logs.info().appendFormat("Variable %ld cout lineaire %e  cout quadratique %e",
                                         Var,
                                         ProblemeAResoudre->CoutLineaire[Var],
                                         ProblemeAResoudre->CoutQuadratique[Var]);
            }
            for (int Cnt = 0; Cnt < ProblemeAResoudre->NombreDeContraintes; Cnt++)
            {
                logs.info().appendFormat("Constraint %ld sens %c B %e",
                                         Cnt,
                                         ProblemeAResoudre->Sens[Cnt],
                                         ProblemeAResoudre->SecondMembre[Cnt]);

                int il = ProblemeAResoudre->IndicesDebutDeLigne[Cnt];
                int ilMax = il + ProblemeAResoudre->NombreDeTermesDesLignes[Cnt];
                for (; il < ilMax; ++il)
                {
                    Var = ProblemeAResoudre->IndicesColonnes[il];
                    logs.info().appendFormat(
                      "      coeff %e var %ld xmin %e xmax %e type %ld",
                      ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes[il],
                      Var,
                      ProblemeAResoudre->Xmin[Var],
                      ProblemeAResoudre->Xmax[Var],
                      ProblemeAResoudre->TypeDeVariable[Var]);
                }
            }
        }
#endif

        return false;
    }
}
