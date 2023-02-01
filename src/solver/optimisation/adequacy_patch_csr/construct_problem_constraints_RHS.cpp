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

#include "../solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "../simulation/adequacy_patch_runtime_data.h"
#include "../solver/simulation/simulation.h"
#include "../hourly_csr_problem.h"

void HourlyCSRProblem::setRHSvalueOnFlows()
{
    // constraint: Flow = Flow_direct - Flow_indirect (+ loop flow) for links between nodes of
    // type 2.
    for (int Interco = 0; Interco < problemeHebdo_->NombreDInterconnexions; Interco++)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->originAreaMode[Interco]
              == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch
            && problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode[Interco]
                 == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            std::map<int, int>::iterator it = numberOfConstraintCsrFlowDissociation.find(Interco);
            if (it != numberOfConstraintCsrFlowDissociation.end())
            {
                int Cnt = it->second;
                problemeAResoudre_.SecondMembre[Cnt] = 0.;
                logs.debug() << Cnt << "Flow=D-I: RHS[" << Cnt
                             << "] = " << problemeAResoudre_.SecondMembre[Cnt];
            }
        }
    }
}

void HourlyCSRProblem::setRHSnodeBalanceValue()
{
    // constraint:
    // ENS(node A) +
    // [ Sum flow_direct(node 2 upstream -> node A) + Sum flow_indirect(node A <- node 2 downstream)
    // – Sum flow_indirect(node 2 upstream <- node A) – Sum flow_direct(node A -> node 2 downstream)
    // ] – spillage(node A) = ENS_init(node A) + net_position_init(node A) – spillage_init(node A)
    // for all areas inside adequacy patch

    for (int Area = 0; Area < problemeHebdo_->NombreDePays; Area++)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[Area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            std::map<int, int>::iterator it = numberOfConstraintCsrAreaBalance.find(Area);
            if (it != numberOfConstraintCsrAreaBalance.end())
            {
                int Cnt = it->second;
                problemeAResoudre_.SecondMembre[Cnt] = rhsAreaBalanceValues[Area];
                logs.debug() << Cnt << ": Area Balance: RHS[" << Cnt
                             << "] = " << problemeAResoudre_.SecondMembre[Cnt]
                             << " (Area = " << Area << ")";
            }
        }
    }
}

void HourlyCSRProblem::setRHSbindingConstraintsValue()
{
    double csrSolverRelaxationRHS = problemeHebdo_->adqPatchParams->ThresholdCSRVarBoundsRelaxation;
    double* SecondMembre = problemeAResoudre_.SecondMembre;

    // constraint:
    // user defined Binding constraints between transmission flows
    // and/or power generated from generating units.
    for (int CntCouplante = 0; CntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
         CntCouplante++)
    {
        if (numberOfConstraintCsrHourlyBinding.find(CntCouplante)
            == numberOfConstraintCsrHourlyBinding.end())
            continue;

        const CONTRAINTES_COUPLANTES* MatriceDesContraintesCouplantes
          = problemeHebdo_->MatriceDesContraintesCouplantes[CntCouplante];

        int Cnt = numberOfConstraintCsrHourlyBinding[CntCouplante];

        // 1. The original RHS of bingding constraint
        SecondMembre[Cnt]
          = MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplante[triggeredHour];

        // 2. RHS part 2: flow other than 2<->2
        int NbInterco
          = MatriceDesContraintesCouplantes->NombreDInterconnexionsDansLaContrainteCouplante;
        for (int Index = 0; Index < NbInterco; Index++)
        {
            int Interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[Index];
            double Poids = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[Index];

            if (problemeHebdo_->adequacyPatchRuntimeData->originAreaMode[Interco]
                  != Data::AdequacyPatch::physicalAreaInsideAdqPatch
                || problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode[Interco]
                     != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            {
                double ValueOfFlow
                  = problemeHebdo_->ValeursDeNTC[triggeredHour]->ValeurDuFlux[Interco];
                SecondMembre[Cnt] -= ValueOfFlow * Poids;
            }
        }

        // 3. RHS part 3: - cluster
        int NbClusters
          = MatriceDesContraintesCouplantes->NombreDePaliersDispatchDansLaContrainteCouplante;

        for (int Index = 0; Index < NbClusters; Index++)
        {
            int Area = MatriceDesContraintesCouplantes->PaysDuPalierDispatch[Index];

            int IndexNumeroDuPalierDispatch
              = MatriceDesContraintesCouplantes->NumeroDuPalierDispatch[Index];

            double Poids = MatriceDesContraintesCouplantes->PoidsDuPalierDispatch[Index];

            double ValueOfVar = problemeHebdo_->ResultatsHoraires[Area]
                                  ->ProductionThermique[triggeredHour]
                                  ->ProductionThermiqueDuPalier[IndexNumeroDuPalierDispatch];

            SecondMembre[Cnt] -= ValueOfVar * Poids;
        }
        if (MatriceDesContraintesCouplantes->SensDeLaContrainteCouplante == '<')
        {
            SecondMembre[Cnt] += csrSolverRelaxationRHS;
        }
        else if (MatriceDesContraintesCouplantes->SensDeLaContrainteCouplante == '>')
        {
            SecondMembre[Cnt] -= csrSolverRelaxationRHS;
        }
        logs.debug() << Cnt << ": Hourly bc: -RHS[" << Cnt << "] = " << SecondMembre[Cnt];
    }
}
