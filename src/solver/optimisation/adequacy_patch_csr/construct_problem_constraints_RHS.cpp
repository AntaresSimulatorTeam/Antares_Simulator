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
#include "../solver/simulation/simulation.h"
#include "../solver/simulation/sim_extern_variables_globales.h"
#include "../solver/optimisation/opt_fonctions.h"

const double csrSolverRelaxationRHS = 1e-3;

void setRHSvalueOnFlows(PROBLEME_HEBDO* ProblemeHebdo, HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    int Cnt;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

    // constraint: Flow = Flow_direct - Flow_indirect (+ loop flow) for links between nodes of
    // type 2.
    for (int Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaMode[Interco]
              == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch
            && ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaMode[Interco]
                 == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            std::map<int, int>::iterator it
              = hourlyCsrProblem.numberOfConstraintCsrFlowDissociation.find(Interco);
            if (it != hourlyCsrProblem.numberOfConstraintCsrFlowDissociation.end())
            {
                Cnt = it->second;
                ProblemeAResoudre->SecondMembre[Cnt] = 0.;
                logs.debug() << Cnt << "Flow=D-I: RHS[" << Cnt
                             << "] = " << ProblemeAResoudre->SecondMembre[Cnt];
            }
        }
    }
}

void setRHSnodeBalanceValue(PROBLEME_HEBDO* ProblemeHebdo, HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    int Cnt;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

    // constraint:
    // ENS(node A) +
    // [ Sum flow_direct(node 2 upstream -> node A) + Sum flow_indirect(node A <- node 2 downstream)
    // – Sum flow_indirect(node 2 upstream <- node A) – Sum flow_direct(node A -> node 2 downstream)
    // ] – spillage(node A) = ENS_init(node A) + net_position_init(node A) – spillage_init(node A)
    // for all areas inside adequacy patch

    for (int Area = 0; Area < ProblemeHebdo->NombreDePays; Area++)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[Area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            std::map<int, int>::iterator it
              = hourlyCsrProblem.numberOfConstraintCsrAreaBalance.find(Area);
            if (it != hourlyCsrProblem.numberOfConstraintCsrAreaBalance.end())
            {
                Cnt = it->second;
                ProblemeAResoudre->SecondMembre[Cnt] = hourlyCsrProblem.rhsAreaBalanceValues[Area];
                logs.debug() << Cnt << ": Area Balance: RHS[" << Cnt
                             << "] = " << ProblemeAResoudre->SecondMembre[Cnt]
                             << " (Area = " << Area << ")";
            }
        }
    }
}

void setRHSbindingConstraintsValue(PROBLEME_HEBDO* ProblemeHebdo,
                                   const HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    int hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    int Cnt;
    int Interco;
    int NbInterco;
    double Poids;
    double ValueOfFlow;
    int Index;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    double* SecondMembre = ProblemeAResoudre->SecondMembre;
    const CONTRAINTES_COUPLANTES* MatriceDesContraintesCouplantes;
    std::map<int, int> bingdingConstraintNumber
      = hourlyCsrProblem.numberOfConstraintCsrHourlyBinding;

    // constraint:
    // user defined Binding constraints between transmission flows
    // and/or power generated from generating units.
    for (int CntCouplante = 0; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes;
         CntCouplante++)
    {
        if (bingdingConstraintNumber.find(CntCouplante) != bingdingConstraintNumber.end())
        {
            MatriceDesContraintesCouplantes
              = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];

            Cnt = bingdingConstraintNumber[CntCouplante];

            // 1. The original RHS of bingding constraint
            SecondMembre[Cnt]
              = MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplante[hour];

            // 2. RHS part 2: flow other than 2<->2
            NbInterco
              = MatriceDesContraintesCouplantes->NombreDInterconnexionsDansLaContrainteCouplante;
            for (Index = 0; Index < NbInterco; Index++)
            {
                Interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[Index];
                Poids = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[Index];

                if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaMode[Interco]
                      != Data::AdequacyPatch::physicalAreaInsideAdqPatch
                    || ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaMode[Interco]
                         != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
                {
                    ValueOfFlow = ProblemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco];
                    SecondMembre[Cnt] -= ValueOfFlow * Poids;
                }
            }

            // 3. RHS part 3: - cluster
            int NbClusters
              = MatriceDesContraintesCouplantes->NombreDePaliersDispatchDansLaContrainteCouplante;
            int Area;
            int Palier;
            int IndexNumeroDuPalierDispatch;
            double ValueOfVar;
            const PALIERS_THERMIQUES* PaliersThermiquesDuPays;

            for (Index = 0; Index < NbClusters; Index++)
            {
                Area = MatriceDesContraintesCouplantes->PaysDuPalierDispatch[Index];
                PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Area];

                IndexNumeroDuPalierDispatch
                  = MatriceDesContraintesCouplantes->NumeroDuPalierDispatch[Index];

                Poids = MatriceDesContraintesCouplantes->PoidsDuPalierDispatch[Index];

                ValueOfVar = ProblemeHebdo->ResultatsHoraires[Area]
                               ->ProductionThermique[hour]
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
}

void OPT_InitialiserLeSecondMembreDuProblemeQuadratique_CSR(PROBLEME_HEBDO* ProblemeHebdo,
                                                            HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    logs.debug() << "[CSR] RHS: ";

    setRHSvalueOnFlows(ProblemeHebdo, hourlyCsrProblem);
    setRHSnodeBalanceValue(ProblemeHebdo, hourlyCsrProblem);
    setRHSbindingConstraintsValue(ProblemeHebdo, hourlyCsrProblem);
}