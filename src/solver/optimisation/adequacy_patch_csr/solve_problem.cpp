/*
** Copyright 2007-2022 RTE
** Authors: RTE-international / Redstork / Antares_Simulator Team
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

#include "../solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "../solver/simulation/simulation.h"
#include "../solver/simulation/sim_structure_donnees.h"
#include "../solver/simulation/sim_structure_probleme_economique.h"
#include "../solver/simulation/sim_structure_probleme_adequation.h"
#include "../solver/simulation/sim_extern_variables_globales.h"

#include "../solver/optimisation/opt_fonctions.h"

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

#include <antares/logs.h>

using namespace Antares;

std::unique_ptr<PROBLEME_POINT_INTERIEUR> buildInteriorPointProblem(
  PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre)
{
    auto Probleme = std::make_unique<PROBLEME_POINT_INTERIEUR>();

    Probleme->NombreMaxDIterations = -1;
    Probleme->CoutQuadratique = ProblemeAResoudre->CoutQuadratique;
    Probleme->CoutLineaire = ProblemeAResoudre->CoutLineaire;
    Probleme->X = ProblemeAResoudre->X;
    Probleme->Xmin = ProblemeAResoudre->Xmin;
    Probleme->Xmax = ProblemeAResoudre->Xmax;
    Probleme->NombreDeVariables = ProblemeAResoudre->NombreDeVariables;
    Probleme->TypeDeVariable = ProblemeAResoudre->TypeDeVariable;

    Probleme->VariableBinaire = (char*)ProblemeAResoudre->CoutsReduits;

    Probleme->NombreDeContraintes = ProblemeAResoudre->NombreDeContraintes;
    Probleme->IndicesDebutDeLigne = ProblemeAResoudre->IndicesDebutDeLigne;
    Probleme->NombreDeTermesDesLignes = ProblemeAResoudre->NombreDeTermesDesLignes;
    Probleme->IndicesColonnes = ProblemeAResoudre->IndicesColonnes;
    Probleme->CoefficientsDeLaMatriceDesContraintes
      = ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes;
    Probleme->Sens = ProblemeAResoudre->Sens;
    Probleme->SecondMembre = ProblemeAResoudre->SecondMembre;

    Probleme->AffichageDesTraces = NON_PI;

    Probleme->UtiliserLaToleranceDAdmissibiliteParDefaut = OUI_PI;
    Probleme->UtiliserLaToleranceDeStationnariteParDefaut = OUI_PI;
    Probleme->UtiliserLaToleranceDeComplementariteParDefaut = OUI_PI;

    Probleme->CoutsMarginauxDesContraintes = ProblemeAResoudre->CoutsMarginauxDesContraintes;

    Probleme->CoutsMarginauxDesContraintesDeBorneInf = ProblemeAResoudre->CoutsReduits;
    Probleme->CoutsMarginauxDesContraintesDeBorneSup = ProblemeAResoudre->CoutsReduits;

    return Probleme;
}

void setToZeroIfBelowThreshold(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
                               HourlyCSRProblem& hourlyCsrProblem)
{
    for (int Var = 0; Var < ProblemeAResoudre->NombreDeVariables; Var++)
    {
        bool inSet = hourlyCsrProblem.varToBeSetToZeroIfBelowThreshold.find(Var)
                     != hourlyCsrProblem.varToBeSetToZeroIfBelowThreshold.end();
        bool belowLimit = ProblemeAResoudre->X[Var] < hourlyCsrProblem.belowThisThresholdSetToZero;
        if (inSet && belowLimit)
            ProblemeAResoudre->X[Var] = 0.0;
    }
}

void storeInteriorPointResults(const PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre)
{
    double* pt;
    for (int Var = 0; Var < ProblemeAResoudre->NombreDeVariables; Var++)
    {
        pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var];
        if (pt)
        {
            *pt = ProblemeAResoudre->X[Var];
        }
        logs.debug() << "[CSR]" << Var << " = " << ProblemeAResoudre->X[Var];
    }
}

void storeOrDisregardInteriorPointResults(const PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
                                          HourlyCSRProblem& hourlyCsrProblem,
                                          uint weekNb,
                                          int yearNb,
                                          double costPriorToCsr,
                                          double costAfterCsr)
{
    const int hoursInWeek = 168;
    const bool checkCost
      = hourlyCsrProblem.problemeHebdo->adqPatchParams->CheckCsrCostFunctionValue;
    double deltaCost = costAfterCsr - costPriorToCsr;

    if (checkCost)
    {
        logs.info() << "[adq-patch] costPriorToCsr: " << costPriorToCsr
                    << ", costAfterCsr: " << costAfterCsr
                    << ", deltaCost: " << costAfterCsr - costPriorToCsr;
    }

    if (!checkCost || (checkCost && deltaCost < 0.0))
        storeInteriorPointResults(ProblemeAResoudre);
    else if (checkCost && deltaCost >= 0.0)
        logs.warning()
          << "[adq-patch] CSR optimization is providing solution with greater costs, optimum "
             "solution is set as LMR . year: "
          << yearNb + 1
          << ". hour: " << weekNb * hoursInWeek + hourlyCsrProblem.hourInWeekTriggeredCsr + 1;
}

double calculateCsrCostFunctionValue(const PROBLEME_POINT_INTERIEUR& Probleme,
                                     const HourlyCSRProblem& hourlyCsrProblem)
{
    logs.debug() << "calculateCsrCostFunctionValue! ";
    double cost = 0.0;
    if (!hourlyCsrProblem.problemeHebdo->adqPatchParams->CheckCsrCostFunctionValue)
    {
        logs.debug() << "CheckCsrCostFunctionValue = FALSE";
        return cost;
    }

    for (int Var = 0; Var < Probleme.NombreDeVariables; Var++)
    {
        logs.debug() << "Var: " << Var;
        bool inEnsSet = hourlyCsrProblem.ensSet.find(Var) != hourlyCsrProblem.ensSet.end();
        if (inEnsSet)
        {
            cost += Probleme.X[Var] * Probleme.X[Var] * Probleme.CoutQuadratique[Var];
            logs.debug() << "X-Q: " << Probleme.X[Var] * 1e3;
            logs.debug() << "CoutQ: " << Probleme.CoutQuadratique[Var] * 1e3;
            logs.debug() << "TotalCost: " << cost * 1e3;
        }
        bool inLinkSet = hourlyCsrProblem.linkSet.find(Var) != hourlyCsrProblem.linkSet.end();
        if (inLinkSet && hourlyCsrProblem.problemeHebdo->adqPatchParams->IncludeHurdleCostCsr)
        {
            if (Probleme.X[Var] >= 0)
            {
                cost += Probleme.X[Var] * Probleme.CoutLineaire[Var + 1];
                logs.debug() << "X+: " << Probleme.X[Var] * 1e3;
                logs.debug() << "CoutL: " << Probleme.CoutLineaire[Var + 1] * 1e3;
                logs.debug() << "TotalCost: " << cost * 1e3;
            }
            else
            {
                cost -= Probleme.X[Var] * Probleme.CoutLineaire[Var + 2];
                logs.debug() << "X-: " << Probleme.X[Var] * 1e3;
                logs.debug() << "CoutL: " << Probleme.CoutLineaire[Var + 2] * 1e3;
                logs.debug() << "TotalCost: " << cost * 1e3;
            }
        }
    }
    return cost;
}

void CSR_DEBUG_HANDLE(const PROBLEME_POINT_INTERIEUR& Probleme)
{
    int Var;

    logs.info();
    logs.info() << LOG_UI_DISPLAY_MESSAGES_OFF;
    logs.info() << "Here is the trace:";

    for (Var = 0; Var < Probleme.NombreDeVariables; Var++)
    {
        logs.info().appendFormat("Variable %ld cout lineaire %e  cout quadratique %e",
                                 Var,
                                 Probleme.CoutLineaire[Var],
                                 Probleme.CoutQuadratique[Var]);
    }
    for (int Cnt = 0; Cnt < Probleme.NombreDeContraintes; Cnt++)
    {
        logs.info().appendFormat(
          "Constraint %ld sens %c B %e", Cnt, Probleme.Sens[Cnt], Probleme.SecondMembre[Cnt]);

        int il = Probleme.IndicesDebutDeLigne[Cnt];
        int ilMax = il + Probleme.NombreDeTermesDesLignes[Cnt];
        for (; il < ilMax; ++il)
        {
            Var = Probleme.IndicesColonnes[il];
            logs.info().appendFormat("      coeff %e var %ld xmin %e xmax %e type %ld",
                                     Probleme.CoefficientsDeLaMatriceDesContraintes[il],
                                     Var,
                                     Probleme.Xmin[Var],
                                     Probleme.Xmax[Var],
                                     Probleme.TypeDeVariable[Var]);
        }
    }
}

void handleInteriorPointError(const PROBLEME_POINT_INTERIEUR& Probleme,
                              int hour,
                              uint weekNb,
                              int yearNb)
{
    const int hoursInWeek = 168;
    logs.warning()
      << "No further optimization for CSR is possible, optimum solution is set as LMR . year: "
      << yearNb + 1 << ". hour: " << weekNb * hoursInWeek + hour + 1;

#ifndef NDEBUG
    CSR_DEBUG_HANDLE(Probleme);
#endif
}

bool ADQ_PATCH_CSR(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
                   HourlyCSRProblem& hourlyCsrProblem,
                   uint weekNb,
                   int yearNb)
{
    auto Probleme = buildInteriorPointProblem(ProblemeAResoudre);
    double costPriorToCsr = calculateCsrCostFunctionValue(*Probleme, hourlyCsrProblem);
    PI_Quamin(Probleme.get()); // resolution
    if (Probleme->ExistenceDUneSolution == OUI_PI)
    {
        setToZeroIfBelowThreshold(ProblemeAResoudre, hourlyCsrProblem);
        double costAfterCsr = calculateCsrCostFunctionValue(*Probleme, hourlyCsrProblem);
        storeOrDisregardInteriorPointResults(
          ProblemeAResoudre, hourlyCsrProblem, weekNb, yearNb, costPriorToCsr, costAfterCsr);
        return true;
    }
    else
    {
        handleInteriorPointError(
          *Probleme, hourlyCsrProblem.hourInWeekTriggeredCsr, weekNb, yearNb);
        return false;
    }
}
