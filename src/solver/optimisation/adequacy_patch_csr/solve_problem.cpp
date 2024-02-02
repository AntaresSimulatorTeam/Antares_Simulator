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

#include "../solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "../solver/simulation/simulation.h"
#include "../solver/simulation/sim_structure_probleme_economique.h"

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

#include <antares/logs/logs.h>

using namespace Antares;

std::unique_ptr<PROBLEME_POINT_INTERIEUR> buildInteriorPointProblem(
  PROBLEME_ANTARES_A_RESOUDRE& ProblemeAResoudre)
{
    auto Probleme = std::make_unique<PROBLEME_POINT_INTERIEUR>();

    Probleme->NombreMaxDIterations = -1;
    Probleme->CoutQuadratique = ProblemeAResoudre.CoutQuadratique.data();
    Probleme->CoutLineaire = ProblemeAResoudre.CoutLineaire.data();
    Probleme->X = ProblemeAResoudre.X.data();
    Probleme->Xmin = ProblemeAResoudre.Xmin.data();
    Probleme->Xmax = ProblemeAResoudre.Xmax.data();
    Probleme->NombreDeVariables = ProblemeAResoudre.NombreDeVariables;
    Probleme->TypeDeVariable = ProblemeAResoudre.TypeDeVariable.data();

    Probleme->VariableBinaire = (char*)ProblemeAResoudre.CoutsReduits.data();

    Probleme->NombreDeContraintes = ProblemeAResoudre.NombreDeContraintes;
    Probleme->IndicesDebutDeLigne = ProblemeAResoudre.IndicesDebutDeLigne.data();
    Probleme->NombreDeTermesDesLignes = ProblemeAResoudre.NombreDeTermesDesLignes.data();
    Probleme->IndicesColonnes = ProblemeAResoudre.IndicesColonnes.data();
    Probleme->CoefficientsDeLaMatriceDesContraintes
      = ProblemeAResoudre.CoefficientsDeLaMatriceDesContraintes.data();
    Probleme->Sens = ProblemeAResoudre.Sens.data();
    Probleme->SecondMembre = ProblemeAResoudre.SecondMembre.data();

    Probleme->AffichageDesTraces = NON_PI;

    Probleme->UtiliserLaToleranceDAdmissibiliteParDefaut = OUI_PI;
    Probleme->UtiliserLaToleranceDeStationnariteParDefaut = OUI_PI;
    Probleme->UtiliserLaToleranceDeComplementariteParDefaut = OUI_PI;

    Probleme->CoutsMarginauxDesContraintes = ProblemeAResoudre.CoutsMarginauxDesContraintes.data();

    Probleme->CoutsMarginauxDesContraintesDeBorneInf = ProblemeAResoudre.CoutsReduits.data();
    Probleme->CoutsMarginauxDesContraintesDeBorneSup = ProblemeAResoudre.CoutsReduits.data();

    return Probleme;
}

void setToZeroIfBelowThreshold(PROBLEME_ANTARES_A_RESOUDRE& ProblemeAResoudre,
                               HourlyCSRProblem& hourlyCsrProblem)
{
    for (int var = 0; var < ProblemeAResoudre.NombreDeVariables; var++)
    {
        bool inSet = hourlyCsrProblem.varToBeSetToZeroIfBelowThreshold.find(var)
                     != hourlyCsrProblem.varToBeSetToZeroIfBelowThreshold.end();
        bool belowLimit = ProblemeAResoudre.X[var] < hourlyCsrProblem.belowThisThresholdSetToZero;
        if (inSet && belowLimit)
            ProblemeAResoudre.X[var] = 0.0;
    }
}

void storeInteriorPointResults(const PROBLEME_ANTARES_A_RESOUDRE& ProblemeAResoudre)
{
    for (int var = 0; var < ProblemeAResoudre.NombreDeVariables; var++)
    {
        if (double* pt = ProblemeAResoudre.AdresseOuPlacerLaValeurDesVariablesOptimisees[var]; pt)
            *pt = ProblemeAResoudre.X[var];

        logs.debug() << "[CSR] X[" << var << "] = " << ProblemeAResoudre.X[var];
    }
}

void storeOrDisregardInteriorPointResults(const PROBLEME_ANTARES_A_RESOUDRE& ProblemeAResoudre,
                                          const HourlyCSRProblem& hourlyCsrProblem,
                                          const AdqPatchParams& adqPatchParams,
                                          uint weekNb,
                                          int yearNb,
                                          double costPriorToCsr,
                                          double costAfterCsr)
{
    const int hoursInWeek = 168;
    const bool checkCost = adqPatchParams.curtailmentSharing.checkCsrCostFunction;
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
          << yearNb + 1 << ". hour: " << weekNb * hoursInWeek + hourlyCsrProblem.triggeredHour + 1;
}

double calculateCSRcost(const PROBLEME_POINT_INTERIEUR& Probleme,
                        const HourlyCSRProblem& hourlyCsrProblem,
                        const AdqPatchParams& adqPatchParams)
{
    logs.debug() << "calculate CSR cost : ";
    double cost = 0.0;
    if (!adqPatchParams.curtailmentSharing.checkCsrCostFunction)
    {
        logs.debug() << "CSR Cost is FALSE";
        return cost;
    }

    for (int i = 0; i < Probleme.NombreDeVariables; i++)
    {
        logs.debug() << "i: " << i;
        if (hourlyCsrProblem.ensVariablesInsideAdqPatch.find(i)
                != hourlyCsrProblem.ensVariablesInsideAdqPatch.end())
        {
            cost += Probleme.X[i] * Probleme.X[i] * Probleme.CoutQuadratique[i];
            logs.debug() << "X-Q: " << Probleme.X[i] * 1e3;
            logs.debug() << "CoutQ: " << Probleme.CoutQuadratique[i] * 1e3;
            logs.debug() << "TotalCost: " << cost * 1e3;
        }

        auto itLink = hourlyCsrProblem.linkInsideAdqPatch.find(i);
        if ((itLink == hourlyCsrProblem.linkInsideAdqPatch.end())
                || adqPatchParams.curtailmentSharing.includeHurdleCost
                || !itLink->second.check())
            continue;

        if (Probleme.X[i] >= 0)
        {
            const int varDirect = itLink->second.directVar;
            if (varDirect < 0)
            {
                logs.warning() << "VarDirect < 0 detected, this should not happen";
                continue;
            }
            cost += Probleme.X[i] * Probleme.CoutLineaire[varDirect];
            logs.debug() << "X+: " << Probleme.X[i] * 1e3;
            logs.debug() << "CoutL: " << Probleme.CoutLineaire[varDirect] * 1e3;
            logs.debug() << "TotalCost: " << cost * 1e3;
        }
        else
        {
            const int varIndirect = itLink->second.indirectVar;
            if (varIndirect < 0)
            {
                logs.warning() << "VarIndirect < 0 detected, this should not happen";
                continue;

            }
            cost -= Probleme.X[i] * Probleme.CoutLineaire[varIndirect];
            logs.debug() << "X-: " << Probleme.X[i] * 1e3;
            logs.debug() << "CoutL: " << Probleme.CoutLineaire[varIndirect] * 1e3;
            logs.debug() << "TotalCost: " << cost * 1e3;
        }
    }
    return cost;
}

void CSR_DEBUG_HANDLE(const PROBLEME_POINT_INTERIEUR& Probleme)
{
    logs.info();
    logs.info() << LOG_UI_DISPLAY_MESSAGES_OFF;
    logs.info() << "Here is the trace:";

    for (int i = 0; i < Probleme.NombreDeVariables; i++)
    {
        logs.info().appendFormat("Variable %ld cout lineaire %e  cout quadratique %e",
                                 i,
                                 Probleme.CoutLineaire[i],
                                 Probleme.CoutQuadratique[i]);
    }
    for (int Cnt = 0; Cnt < Probleme.NombreDeContraintes; Cnt++)
    {
        logs.info().appendFormat(
          "Constraint %ld sens %c B %e", Cnt, Probleme.Sens[Cnt], Probleme.SecondMembre[Cnt]);

        int il = Probleme.IndicesDebutDeLigne[Cnt];
        int ilMax = il + Probleme.NombreDeTermesDesLignes[Cnt];
        for (; il < ilMax; ++il)
        {
            int var = Probleme.IndicesColonnes[il];
            logs.info().appendFormat("      coeff %e var %ld xmin %e xmax %e type %ld",
                                     Probleme.CoefficientsDeLaMatriceDesContraintes[il],
                                     var,
                                     Probleme.Xmin[var],
                                     Probleme.Xmax[var],
                                     Probleme.TypeDeVariable[var]);
        }
    }
}

void handleInteriorPointError([[maybe_unused]] const PROBLEME_POINT_INTERIEUR& Probleme,
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

bool ADQ_PATCH_CSR(PROBLEME_ANTARES_A_RESOUDRE& ProblemeAResoudre,
                   HourlyCSRProblem& hourlyCsrProblem,
                   const AdqPatchParams& adqPatchParams,
                   uint weekNb,
                   int yearNb)
{
    auto interiorPointProblem = buildInteriorPointProblem(ProblemeAResoudre);
    double costPriorToCsr = calculateCSRcost(*interiorPointProblem, hourlyCsrProblem, adqPatchParams);
    PI_Quamin(interiorPointProblem.get()); // resolution
    if (interiorPointProblem->ExistenceDUneSolution == OUI_PI)
    {
        setToZeroIfBelowThreshold(ProblemeAResoudre, hourlyCsrProblem);
        double costAfterCsr = calculateCSRcost(*interiorPointProblem, hourlyCsrProblem, adqPatchParams);
        storeOrDisregardInteriorPointResults(ProblemeAResoudre, 
                                             hourlyCsrProblem,
                                             adqPatchParams,
                                             weekNb,
                                             yearNb,
                                             costPriorToCsr,
                                             costAfterCsr);
        return true;
    }
    else
    {
        handleInteriorPointError(*interiorPointProblem, hourlyCsrProblem.triggeredHour, weekNb, yearNb);
        return false;
    }
}
