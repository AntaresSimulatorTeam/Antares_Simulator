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

#include <limits>

#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"

/*
 pi_define.h doesn't include this header, yet it uses struct jmp_buf.
 It would be nice to remove this include, but would require to change pi_define.h,
 which isn't part of Antares
*/
#include <setjmp.h>

extern "C"
{
#include "pi_define.h"
#include "pi_definition_arguments.h"
#include "pi_fonctions.h"
}

#include <antares/logs/logs.h>

using namespace Antares;

bool OPT_AppelDuSolveurQuadratique(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
                                   const int PdtHebdo)
{
    PROBLEME_POINT_INTERIEUR Probleme;

    double ToleranceSurLAdmissibilite = 1.e-5;
    int ChoixToleranceParDefautSurLAdmissibilite = OUI_PI;

    double ToleranceSurLaStationnarite = 1.e-5;
    int ChoixToleranceParDefautSurLaStationnarite = OUI_PI;

    double ToleranceSurLaComplementarite = 1.e-5;
    int ChoixToleranceParDefautSurLaComplementarite = OUI_PI;

    Probleme.NombreMaxDIterations = -1;
    Probleme.CoutQuadratique = ProblemeAResoudre->CoutQuadratique.data();
    Probleme.CoutLineaire = ProblemeAResoudre->CoutLineaire.data();
    Probleme.X = ProblemeAResoudre->X.data();
    Probleme.Xmin = ProblemeAResoudre->Xmin.data();
    Probleme.Xmax = ProblemeAResoudre->Xmax.data();
    Probleme.NombreDeVariables = ProblemeAResoudre->NombreDeVariables;
    Probleme.TypeDeVariable = ProblemeAResoudre->TypeDeVariable.data();

    Probleme.VariableBinaire = (char*)ProblemeAResoudre->CoutsReduits.data();

    Probleme.NombreDeContraintes = ProblemeAResoudre->NombreDeContraintes;
    Probleme.IndicesDebutDeLigne = ProblemeAResoudre->IndicesDebutDeLigne.data();
    Probleme.NombreDeTermesDesLignes = ProblemeAResoudre->NombreDeTermesDesLignes.data();
    Probleme.IndicesColonnes = ProblemeAResoudre->IndicesColonnes.data();
    Probleme.CoefficientsDeLaMatriceDesContraintes = ProblemeAResoudre
                                                       ->CoefficientsDeLaMatriceDesContraintes
                                                       .data();
    Probleme.Sens = ProblemeAResoudre->Sens.data();
    Probleme.SecondMembre = ProblemeAResoudre->SecondMembre.data();

    Probleme.AffichageDesTraces = NON_PI;

    Probleme.UtiliserLaToleranceDAdmissibiliteParDefaut = ChoixToleranceParDefautSurLAdmissibilite;
    Probleme.ToleranceDAdmissibilite = ToleranceSurLAdmissibilite;

    Probleme.UtiliserLaToleranceDeStationnariteParDefaut
      = ChoixToleranceParDefautSurLaStationnarite;
    Probleme.ToleranceDeStationnarite = ToleranceSurLaStationnarite;

    Probleme.UtiliserLaToleranceDeComplementariteParDefaut
      = ChoixToleranceParDefautSurLaComplementarite;
    Probleme.ToleranceDeComplementarite = ToleranceSurLaComplementarite;

    Probleme.CoutsMarginauxDesContraintes = ProblemeAResoudre->CoutsMarginauxDesContraintes.data();

    Probleme.CoutsMarginauxDesContraintesDeBorneInf = ProblemeAResoudre->CoutsReduits.data();
    Probleme.CoutsMarginauxDesContraintesDeBorneSup = ProblemeAResoudre->CoutsReduits.data();

    PI_Quamin(&Probleme);

    ProblemeAResoudre->ExistenceDUneSolution = Probleme.ExistenceDUneSolution;
    if (ProblemeAResoudre->ExistenceDUneSolution == OUI_PI)
    {
        for (int i = 0; i < ProblemeAResoudre->NombreDeVariables; i++)
        {
            double* pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[i];
            if (pt)
            {
                *pt = ProblemeAResoudre->X[i];
            }
        }

        return true;
    }
    else
    {
        logs.warning() << "Quadratic Optimisation: No solution, hour " << PdtHebdo;

        for (int i = 0; i < ProblemeAResoudre->NombreDeVariables; i++)
        {
            double* pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[i];
            if (pt)
            {
                *pt = std::numeric_limits<double>::quiet_NaN();
            }
        }

#ifndef NDEBUG

        {
            logs.info();

            logs.info() << LOG_UI_DISPLAY_MESSAGES_OFF;

            logs.info() << "Here is the trace:";
            for (int i = 0; i < ProblemeAResoudre->NombreDeVariables; i++)
            {
                logs.info().appendFormat("Variable %ld cout lineaire %e  cout quadratique %e",
                                         i,
                                         ProblemeAResoudre->CoutLineaire[i],
                                         ProblemeAResoudre->CoutQuadratique[i]);
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
                    int var = ProblemeAResoudre->IndicesColonnes[il];
                    logs.info().appendFormat(
                      "      coeff %e var %ld xmin %e xmax %e type %ld",
                      ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes[il],
                      var,
                      ProblemeAResoudre->Xmin[var],
                      ProblemeAResoudre->Xmax[var],
                      ProblemeAResoudre->TypeDeVariable[var]);
                }
            }
        }
#endif

        return false;
    }
}
