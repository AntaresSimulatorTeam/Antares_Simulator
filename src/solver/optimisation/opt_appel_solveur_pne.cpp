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

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"

#include "pne_definition_arguments.h"
#include "pne_fonctions.h"

#include "opt_fonctions.h"

#include <antares/emergency.h>

#include "../utils/ortools_utils.h"

bool OPT_AppelDuSolveurPne(PROBLEME_HEBDO* ProblemeHebdo, uint numSpace, int NumIntervalle)
{
    int Var;
    int Cnt;
    double* pt;
    int il;
    int ilMax;
    double CoutOpt;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    int* TypeEntierOuReel;
    PROBLEME_A_RESOUDRE ProblemePourPne;
    double u;

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    TypeEntierOuReel = (int*)ProblemeAResoudre->CoutsReduits;

    for (Var = 0; Var < ProblemeAResoudre->NombreDeVariables; Var++)
        TypeEntierOuReel[Var] = REEL;

    ProblemePourPne.NombreDeVariables = ProblemeAResoudre->NombreDeVariables;
    ProblemePourPne.TypeDeVariable = TypeEntierOuReel;
    ProblemePourPne.TypeDeBorneDeLaVariable = ProblemeAResoudre->TypeDeVariable;
    ProblemePourPne.X = ProblemeAResoudre->X;
    ProblemePourPne.Xmax = ProblemeAResoudre->Xmax;
    ProblemePourPne.Xmin = ProblemeAResoudre->Xmin;
    ProblemePourPne.CoutLineaire = ProblemeAResoudre->CoutLineaire;
    ProblemePourPne.NombreDeContraintes = ProblemeAResoudre->NombreDeContraintes;
    ProblemePourPne.SecondMembre = ProblemeAResoudre->SecondMembre;
    ProblemePourPne.Sens = ProblemeAResoudre->Sens;
    ProblemePourPne.IndicesDebutDeLigne = ProblemeAResoudre->IndicesDebutDeLigne;
    ProblemePourPne.NombreDeTermesDesLignes = ProblemeAResoudre->NombreDeTermesDesLignes;
    ProblemePourPne.CoefficientsDeLaMatriceDesContraintes
      = ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes;
    ProblemePourPne.IndicesColonnes = ProblemeAResoudre->IndicesColonnes;
    ProblemePourPne.VariablesDualesDesContraintes = ProblemeAResoudre->CoutsMarginauxDesContraintes;
    ProblemePourPne.SortirLesDonneesDuProbleme = NON_PNE;
    ProblemePourPne.AlgorithmeDeResolution = SIMPLEXE;
    ProblemePourPne.CoupesLiftAndProject = NON_PNE;
    ProblemePourPne.AffichageDesTraces = NON_PNE;
    ProblemePourPne.FaireDuPresolve = OUI_PNE;
    if (ProblemeAResoudre->NumeroDOptimisation == DEUXIEME_OPTIMISATION)
    {
        ProblemePourPne.FaireDuPresolve = NON_PNE;
    }

    ProblemePourPne.TempsDExecutionMaximum = 0;
    ProblemePourPne.NombreMaxDeSolutionsEntieres = -1;
    ProblemePourPne.ToleranceDOptimalite = 1.e-4;

    auto& study = *Data::Study::Current::Get();
    bool ortoolsUsed = study.parameters.ortoolsUsed;

    MPSolver* solver = NULL;
    if (ortoolsUsed)
    {
        solver = ORTOOLS_Simplexe_PNE(&ProblemePourPne, NULL);

        if (ProblemeHebdo->ExportMPS == OUI_ANTARES)
        {
            int const n = ProblemeHebdo->numeroOptimisation[NumIntervalle];
            ORTOOLS_EcrireJeuDeDonneesLineaireAuFormatMPS(solver, numSpace, n);
        }
    }
    else
    {
        PNE_Solveur(&ProblemePourPne);

        if (ProblemeHebdo->ExportMPS == OUI_ANTARES)
            OPT_EcrireJeuDeDonneesLineaireAuFormatMPS(
              (void*)&ProblemePourPne, numSpace, ANTARES_PNE);
    }
    ProblemeAResoudre->ExistenceDUneSolution = ProblemePourPne.ExistenceDUneSolution;

    if (ProblemeAResoudre->ExistenceDUneSolution == ARRET_CAR_ERREUR_INTERNE)
    {
        logs.info();
        logs.error() << "Internal error: insufficient memory";
        logs.info();
        AntaresSolverEmergencyShutdown();
        return false;
    }

    bool sol_opt_trouvee = (ProblemeAResoudre->ExistenceDUneSolution == SOLUTION_OPTIMALE_TROUVEE);
    sol_opt_trouvee = (sol_opt_trouvee
                       || ProblemeAResoudre->ExistenceDUneSolution
                            == SOLUTION_OPTIMALE_TROUVEE_MAIS_QUELQUES_CONTRAINTES_SONT_VIOLEES);
    if (sol_opt_trouvee)
    {
        CoutOpt = 0.0;
        for (Var = 0; Var < ProblemeAResoudre->NombreDeVariables; Var++)
            ProblemeAResoudre->CoutsReduits[Var] = ProblemeAResoudre->CoutLineaire[Var];

        for (Cnt = 0; Cnt < ProblemeAResoudre->NombreDeContraintes; Cnt++)
        {
            il = ProblemeAResoudre->IndicesDebutDeLigne[Cnt];
            ilMax = il + ProblemeAResoudre->NombreDeTermesDesLignes[Cnt];
            u = ProblemeAResoudre->CoutsMarginauxDesContraintes[Cnt];
            while (il < ilMax)
            {
                Var = ProblemeAResoudre->IndicesColonnes[il];
                ProblemeAResoudre->CoutsReduits[Var]
                  -= u * ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes[il];
                il++;
            }
        }

        for (Var = 0; Var < ProblemeAResoudre->NombreDeVariables; Var++)
        {
            CoutOpt += ProblemeAResoudre->CoutLineaire[Var] * ProblemeAResoudre->X[Var];

            pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var];
            if (pt != NULL)
                *pt = ProblemeAResoudre->X[Var];
            pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits[Var];
            if (pt != NULL)
                *pt = ProblemeAResoudre->CoutsReduits[Var];
        }

        if (ProblemeHebdo->numeroOptimisation[NumIntervalle] == PREMIERE_OPTIMISATION)
            ProblemeHebdo->coutOptimalSolution1[NumIntervalle] = CoutOpt;
        else
            ProblemeHebdo->coutOptimalSolution2[NumIntervalle] = CoutOpt;

        for (Cnt = 0; Cnt < ProblemeAResoudre->NombreDeContraintes; Cnt++)
        {
            pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt];
            if (pt != NULL)
                *pt = ProblemeAResoudre->CoutsMarginauxDesContraintes[Cnt];
        }
    }
    else
    {
        logs.error() << "Infeasible linear problem encountered. Possible causes:";
        logs.error() << "* binding constraints,";
        logs.error() << "* last resort shedding status,";
        logs.error() << "* negative hurdle costs on lines with infinite capacity,";
        logs.error() << "* Hydro reservoir impossible to manage with cumulative options \"hard "
                        "bounds without heuristic\"";

        // Write MPS only if exportMPSOnError is activated and MPS weren't exported before with
        // ExportMPS option
        if (ProblemeHebdo->ExportMPS == NON_ANTARES && ProblemeHebdo->exportMPSOnError)
        {
            if (ortoolsUsed)
            {
                int const n = ProblemeHebdo->numeroOptimisation[NumIntervalle];
                ORTOOLS_EcrireJeuDeDonneesLineaireAuFormatMPS(solver, numSpace, n);
            }
            else
            {
                OPT_EcrireJeuDeDonneesLineaireAuFormatMPS(
                  (void*)&ProblemePourPne, numSpace, ANTARES_PNE);
            }
        }

        if (solver != NULL)
        {
            ORTOOLS_LibererProbleme(solver);
        }

        return false;
    }

    if (solver != NULL)
    {
        ORTOOLS_LibererProbleme(solver);
    }

    return true;
}
