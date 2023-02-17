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

#include <vector>
#include "../solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "../solver/simulation/simulation.h"
#include "../solver/simulation/sim_structure_donnees.h"
#include "../simulation/adequacy_patch_runtime_data.h"
#include "../solver/optimisation/opt_fonctions.h"
#include "csr_quadratic_problem.h"
#include "hourly_csr_problem.h"

using namespace Antares::Data;

namespace Antares::Solver::Optimization
{
void CsrQuadraticProblem::setConstraintsOnFlows(double* Pi, int* Colonne)
{
    int hour = hourlyCsrProblem_.triggeredHour;
    const CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim
      = problemeHebdo_->CorrespondanceVarNativesVarOptim[hour];

    // constraint: Flow = Flow_direct - Flow_indirect (+ loop flow) for links between nodes of
    // type 2.
    for (int Interco = 0; Interco < problemeHebdo_->NombreDInterconnexions; Interco++)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->originAreaMode[Interco]
              == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch
            && problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode[Interco]
                 == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            int NombreDeTermes = 0;
            int var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
            if (var >= 0)
            {
                Pi[NombreDeTermes] = 1.0;
                Colonne[NombreDeTermes] = var;
                NombreDeTermes++;
            }
            var = CorrespondanceVarNativesVarOptim
                    ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco];
            if (var >= 0)
            {
                Pi[NombreDeTermes] = -1.0;
                Colonne[NombreDeTermes] = var;
                NombreDeTermes++;
            }
            var = CorrespondanceVarNativesVarOptim
                    ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco];
            if (var >= 0)
            {
                Pi[NombreDeTermes] = 1.0;
                Colonne[NombreDeTermes] = var;
                NombreDeTermes++;
            }

            hourlyCsrProblem_.numberOfConstraintCsrFlowDissociation[Interco]
              = problemeAResoudre_.NombreDeContraintes;

            std::string NomDeLaContrainte = "flow=d-i, Interco:" + std::to_string(Interco);
            logs.debug() << "C Interco: " << problemeAResoudre_.NombreDeContraintes << ": "
                         << NomDeLaContrainte;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              &problemeAResoudre_, Pi, Colonne, NombreDeTermes, '=');
        }
    }
}

void CsrQuadraticProblem::setNodeBalanceConstraints(double* Pi, int* Colonne)
{
    int hour = hourlyCsrProblem_.triggeredHour;
    const CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim
      = problemeHebdo_->CorrespondanceVarNativesVarOptim[hour];

    // constraint:
    // ENS(node A) +
    // - flow (A -> 2) or (+ flow (2 -> A)) there should be only one of them, otherwise double-count
    // - spillage(node A) =
    // ENS_init(node A) + net_position_init(node A) – spillage_init(node A)
    // for all areas inside adequacy patch

    for (int Area = 0; Area < problemeHebdo_->NombreDePays; ++Area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[Area]
            != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            continue;

        // + ENS
        int NombreDeTermes = 0;
        int var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[Area];
        if (var >= 0)
        {
            Pi[NombreDeTermes] = 1.0;
            Colonne[NombreDeTermes] = var;
            NombreDeTermes++;
        }

        // - export flows
        int Interco = problemeHebdo_->IndexDebutIntercoOrigine[Area];
        while (Interco >= 0)
        {
            if (problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode[Interco]
                != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            {
                Interco = problemeHebdo_->IndexSuivantIntercoOrigine[Interco];
                continue;
            }

            var = CorrespondanceVarNativesVarOptim
                    ->NumeroDeVariableDeLInterconnexion[Interco]; // flow (A->2)
            if (var >= 0)
            {
                Pi[NombreDeTermes] = -1.0;
                Colonne[NombreDeTermes] = var;
                NombreDeTermes++;
                logs.debug()
                  << "S-Interco number: [" << std::to_string(Interco) << "] between: ["
                  << problemeHebdo_->NomsDesPays[Area] << "]-["
                  << problemeHebdo_
                       ->NomsDesPays[problemeHebdo_->PaysExtremiteDeLInterconnexion[Interco]]
                  << "]";
            }
            Interco = problemeHebdo_->IndexSuivantIntercoOrigine[Interco];
        }

        // or + import flows
        Interco = problemeHebdo_->IndexDebutIntercoExtremite[Area];
        while (Interco >= 0)
        {
            if (problemeHebdo_->adequacyPatchRuntimeData->originAreaMode[Interco]
                != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            {
                Interco = problemeHebdo_->IndexSuivantIntercoExtremite[Interco];
                continue;
            }
            var = CorrespondanceVarNativesVarOptim
                    ->NumeroDeVariableDeLInterconnexion[Interco]; // flow (2 -> A)
            if (var >= 0)
            {
                Pi[NombreDeTermes] = 1.0;
                Colonne[NombreDeTermes] = var;
                NombreDeTermes++;
                logs.debug()
                  << "E-Interco number: [" << std::to_string(Interco) << "] between: ["
                  << problemeHebdo_->NomsDesPays[Area] << "]-["
                  << problemeHebdo_
                       ->NomsDesPays[problemeHebdo_->PaysOrigineDeLInterconnexion[Interco]]
                  << "]";
            }
            Interco = problemeHebdo_->IndexSuivantIntercoExtremite[Interco];
        }

        // - Spilled Energy
        var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[Area];
        if (var >= 0)
        {
            Pi[NombreDeTermes] = -1.0;
            Colonne[NombreDeTermes] = var;
            NombreDeTermes++;
        }

        hourlyCsrProblem_.numberOfConstraintCsrAreaBalance[Area]
          = problemeAResoudre_.NombreDeContraintes;

        std::string NomDeLaContrainte
          = "Area Balance, Area:" + std::to_string(Area) + "; " + problemeHebdo_->NomsDesPays[Area];

        logs.debug() << "C: " << problemeAResoudre_.NombreDeContraintes << ": "
                     << NomDeLaContrainte;

        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
          &problemeAResoudre_, Pi, Colonne, NombreDeTermes, '=');
    }
}

void CsrQuadraticProblem::setBindingConstraints(double* Pi, int* Colonne)
{
    int hour = hourlyCsrProblem_.triggeredHour;

    // Special case of the binding constraints
    for (int CntCouplante = 0; CntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
         CntCouplante++)
    {
        const CONTRAINTES_COUPLANTES* MatriceDesContraintesCouplantes
          = problemeHebdo_->MatriceDesContraintesCouplantes[CntCouplante];

        if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante != CONTRAINTE_HORAIRE)
            continue;

        int NbInterco
          = MatriceDesContraintesCouplantes->NombreDInterconnexionsDansLaContrainteCouplante;
        int NombreDeTermes = 0;
        for (int Index = 0; Index < NbInterco; Index++)
        {
            int Interco = MatriceDesContraintesCouplantes->NumeroDeLInterconnexion[Index];
            double Poids = MatriceDesContraintesCouplantes->PoidsDeLInterconnexion[Index];

            if (problemeHebdo_->adequacyPatchRuntimeData->originAreaMode[Interco]
                  == Data::AdequacyPatch::physicalAreaInsideAdqPatch
                && problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode[Interco]
                     == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            {
                int var = problemeHebdo_->CorrespondanceVarNativesVarOptim[hour]
                            ->NumeroDeVariableDeLInterconnexion[Interco];

                if (var >= 0)
                {
                    Pi[NombreDeTermes] = Poids;
                    Colonne[NombreDeTermes] = var;
                    NombreDeTermes++;
                }
            }
        }

        if (NombreDeTermes > 0) // current binding constraint contains an interco type 2<->2
        {
            hourlyCsrProblem_.numberOfConstraintCsrHourlyBinding[CntCouplante]
              = problemeAResoudre_.NombreDeContraintes;

            std::string NomDeLaContrainte
              = "bc::hourly::" + std::to_string(hour)
                + "::" + MatriceDesContraintesCouplantes->NomDeLaContrainteCouplante;

            logs.debug() << "C (bc): " << problemeAResoudre_.NombreDeContraintes << ": "
                         << NomDeLaContrainte;

            OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
              &problemeAResoudre_,
              Pi,
              Colonne,
              NombreDeTermes,
              MatriceDesContraintesCouplantes->SensDeLaContrainteCouplante);
        }
    }
}

void CsrQuadraticProblem::buildConstraintMatrix()
{
    logs.debug() << "[CSR] constraint list:";

    std::vector<double> Pi(problemeAResoudre_.NombreDeVariables);
    std::vector<int> Colonne(problemeAResoudre_.NombreDeVariables);

    problemeAResoudre_.NombreDeContraintes = 0;
    problemeAResoudre_.NombreDeTermesDansLaMatriceDesContraintes = 0;

    setConstraintsOnFlows(Pi.data(), Colonne.data());
    setNodeBalanceConstraints(Pi.data(), Colonne.data());
    setBindingConstraints(Pi.data(), Colonne.data());
}

} // namespace Antares::Solver::Optimization
