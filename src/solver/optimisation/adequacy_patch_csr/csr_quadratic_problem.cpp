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

#include "../simulation/adequacy_patch_runtime_data.h"
#include "../solver/optimisation/opt_fonctions.h"
#include "csr_quadratic_problem.h"
#include "hourly_csr_problem.h"
#include "sim_structure_probleme_economique.h"
#include "../constraint_builder.h"
#include "../constraint_builder_utils.h"
#include "../opt_rename_problem.h"
#include "adequacy_patch_runtime_data.h"

using namespace Antares::Data;

namespace Antares::Solver::Optimization
{
struct CsrFlowDissociationData
{
    std::map<int, int>& numberOfConstraintCsrFlowDissociation;
    const uint32_t NombreDInterconnexions;

    const std::vector<adqPatchParamsMode>& originAreaMode;
    const std::vector<adqPatchParamsMode>& extremityAreaMode;

    const std::vector<int>& PaysOrigineDeLInterconnexion;
    const std::vector<int>& PaysExtremiteDeLInterconnexion;
};
class CsrFlowDissociation : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int hour, CsrFlowDissociationData& data)
    {
        builder.updateHourWithinWeek(hour);

        ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);
        namer.UpdateTimeStep(hour);
        // constraint: Flow = Flow_direct - Flow_indirect (+ loop flow) for links between nodes
        // of type 2.
        for (uint32_t interco = 0; interco < data.NombreDInterconnexions; interco++)
        {
            if (data.originAreaMode[interco]
                  == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch
                && data.extremityAreaMode[interco]
                     == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            {
                builder.include(Variable::NTCDirect(interco), 1.0)
                  .include(Variable::IntercoDirectCost(interco), -1.0)
                  .include(Variable::IntercoIndirectCost(interco), 1.0);

                data.numberOfConstraintCsrFlowDissociation[interco]
                  = builder.data.nombreDeContraintes;

                const auto& origin
                  = builder.data.NomsDesPays[data.PaysOrigineDeLInterconnexion[interco]];
                const auto& destination
                  = builder.data.NomsDesPays[data.PaysExtremiteDeLInterconnexion[interco]];
                namer.CsrFlowDissociation(builder.data.nombreDeContraintes, origin, destination);
                builder.equalTo();
                builder.build();
            }
        }
    }
};

void CsrQuadraticProblem::setConstraintsOnFlows(std::vector<double>& Pi, std::vector<int>& Colonne)
{
    int hour = hourlyCsrProblem_.triggeredHour;
    //!\ TODO not associated problemHebdo && probleamAressoudre
    auto builder = GetConstraintBuilderFromProblemHebdoAndProblemAResoudre(problemeHebdo_,
                                                                           &problemeAResoudre_);
    CsrFlowDissociation csrFlowDissociation(builder);
    CsrFlowDissociationData csrFlowDissociationData
      = {hourlyCsrProblem_.numberOfConstraintCsrFlowDissociation,
         problemeHebdo_->NombreDInterconnexions,
         problemeHebdo_->adequacyPatchRuntimeData->originAreaMode,
         problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode,
         problemeHebdo_->PaysOrigineDeLInterconnexion,
         problemeHebdo_->PaysExtremiteDeLInterconnexion};

    csrFlowDissociation.add(hour, csrFlowDissociationData);
}
struct CsrAreaBalanceData
{
    const adqPatchParamsMode& areaMode;
    const int hour;
    const std::vector<int>& IndexDebutIntercoOrigine;
    const std::vector<int>& IndexSuivantIntercoOrigine;
    const std::vector<int>& IndexDebutIntercoExtremite;
    const std::vector<int>& IndexSuivantIntercoExtremite;

    const std::vector<adqPatchParamsMode>& originAreaMode;
    const std::vector<adqPatchParamsMode>& extremityAreaMode;
    const std::vector<int>& PaysOrigineDeLInterconnexion;
    const std::vector<int>& PaysExtremiteDeLInterconnexion;
    std::map<int, int> numberOfConstraintCsrAreaBalance;
};

class CsrAreaBalance : private Constraint
{
public:
    using Constraint::Constraint;
    void add(uint32_t Area, CsrAreaBalanceData& data)
    {
        if (data.areaMode != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            return;

        // + ENS
        int NombreDeTermes = 0;
        builder.updateHourWithinWeek(data.hour).include(Variable::NegativeUnsuppliedEnergy(Area),
                                                        1.0);

        // - export flows
        int Interco = data.IndexDebutIntercoOrigine[Area];
        while (Interco >= 0)
        {
            if (data.extremityAreaMode[Interco] != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            {
                Interco = data.IndexSuivantIntercoOrigine[Interco];
                continue;
            }

            // flow (A->2)
            builder.include(Variable::NTCDirect(Interco), -1.0);

            logs.debug() << "S-Interco number: [" << std::to_string(Interco) << "] between: ["
                         << builder.data.NomsDesPays[Area] << "]-["
                         << builder.data.NomsDesPays[data.PaysExtremiteDeLInterconnexion[Interco]]
                         << "]";

            Interco = data.IndexSuivantIntercoOrigine[Interco];
        }

        // or + import flows
        Interco = data.IndexDebutIntercoExtremite[Area];
        while (Interco >= 0)
        {
            if (data.originAreaMode[Interco] != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            {
                Interco = data.IndexSuivantIntercoExtremite[Interco];
                continue;
            }
            // flow (2 -> A)
            builder.include(Variable::NTCDirect(Interco), 1.0);

            logs.debug() << "E-Interco number: [" << std::to_string(Interco) << "] between: ["
                         << builder.data.NomsDesPays[Area] << "]-["
                         << builder.data.NomsDesPays[data.PaysOrigineDeLInterconnexion[Interco]]
                         << "]";

            Interco = data.IndexSuivantIntercoExtremite[Interco];
        }

        // - Spilled Energy
        builder.include(Variable::NegativeUnsuppliedEnergy(Area), -1.0);

        data.numberOfConstraintCsrAreaBalance[Area] = builder.data.nombreDeContraintes;

        std::string NomDeLaContrainte
          = "Area Balance, Area:" + std::to_string(Area) + "; " + builder.data.NomsDesPays[Area];

        logs.debug() << "C: " << builder.data.nombreDeContraintes << ": " << NomDeLaContrainte;
        builder.equalTo();
        builder.build();
    }
};
void CsrQuadraticProblem::setNodeBalanceConstraints(std::vector<double>& Pi,
                                                    std::vector<int>& Colonne)
{
    int hour = hourlyCsrProblem_.triggeredHour;
    const CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
      = problemeHebdo_->CorrespondanceVarNativesVarOptim[hour];

    // constraint:
    // ENS(node A) +
    // - flow (A -> 2) or (+ flow (2 -> A)) there should be only one of them, otherwise double-count
    // - spillage(node A) =
    // ENS_init(node A) + net_position_init(node A) – spillage_init(node A)
    // for all areas inside adequacy patch

    ConstraintBuilder builder(
      GetConstraintBuilderFromProblemHebdoAndProblemAResoudre(problemeHebdo_, &problemeAResoudre_));
    CsrAreaBalance csrAreaBalance(builder);

    for (uint32_t Area = 0; Area < problemeHebdo_->NombreDePays; ++Area)
    {
        CsrAreaBalanceData csrAreaBalanceData{
          problemeHebdo_->adequacyPatchRuntimeData->areaMode[Area],
          hour,
          problemeHebdo_->IndexDebutIntercoOrigine,
          problemeHebdo_->IndexSuivantIntercoOrigine,
          problemeHebdo_->IndexDebutIntercoExtremite,
          problemeHebdo_->IndexSuivantIntercoExtremite,
          problemeHebdo_->adequacyPatchRuntimeData->originAreaMode,
          problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode,
          problemeHebdo_->PaysOrigineDeLInterconnexion,
          problemeHebdo_->PaysExtremiteDeLInterconnexion,
          hourlyCsrProblem_.numberOfConstraintCsrAreaBalance};
        csrAreaBalance.add(Area, csrAreaBalanceData);
    }
}

struct CsrBindingConstraintHourData
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes;

    const std::vector<adqPatchParamsMode>& originAreaMode;
    const std::vector<adqPatchParamsMode>& extremityAreaMode;
    const int hour;

    std::map<int, int>& numberOfConstraintCsrHourlyBinding;
};

class CsrBindingConstraintHour : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int CntCouplante, CsrBindingConstraintHourData& data)
    {
        if (data.MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_HORAIRE)
            return;

        int NbInterco
          = data.MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante;
        builder.updateHourWithinWeek(data.hour);

        for (int Index = 0; Index < NbInterco; Index++)
        {
            int Interco = data.MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[Index];
            double Poids = data.MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[Index];

            if (data.originAreaMode[Interco] == Data::AdequacyPatch::physicalAreaInsideAdqPatch
                && data.extremityAreaMode[Interco]
                     == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            {
                builder.include(Variable::NTCDirect(Interco), Poids);
            }
        }

        if (builder.NumberOfVariables()
            > 0) // current binding constraint contains an interco type 2<->2
        {
            data.numberOfConstraintCsrHourlyBinding[CntCouplante]
              = builder.data.nombreDeContraintes;

            std::string NomDeLaContrainte
              = "bc::hourly::" + std::to_string(data.hour)
                + "::" + data.MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante;

            logs.debug() << "C (bc): " << builder.data.nombreDeContraintes << ": "
                         << NomDeLaContrainte;

            char op = data.MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante;
            builder.operatorRHS(op);
            builder.build();
        }
    }
};
void CsrQuadraticProblem::setBindingConstraints(std::vector<double>& Pi, std::vector<int>& Colonne)
{
    int hour = hourlyCsrProblem_.triggeredHour;
    ConstraintBuilder builder(
      GetConstraintBuilderFromProblemHebdoAndProblemAResoudre(problemeHebdo_, &problemeAResoudre_));
    // Special case of the binding constraints
    for (uint32_t CntCouplante = 0; CntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
         CntCouplante++)
    {
        CsrBindingConstraintHourData csrBindingConstraintHourData
          = {problemeHebdo_->MatriceDesContraintesCouplantes[CntCouplante],
             problemeHebdo_->adequacyPatchRuntimeData->originAreaMode,
             problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode,
             hour,
             hourlyCsrProblem_.numberOfConstraintCsrHourlyBinding};
        CsrBindingConstraintHour csrBindingConstraintHour(builder);
        csrBindingConstraintHour.add(CntCouplante, csrBindingConstraintHourData);
    }
}

void CsrQuadraticProblem::buildConstraintMatrix()
{
    logs.debug() << "[CSR] constraint list:";

    std::vector<double> Pi(problemeAResoudre_.NombreDeVariables);
    std::vector<int> Colonne(problemeAResoudre_.NombreDeVariables);
    // TODO reset problemeAResoudre_.Pi and problemeAResoudre_.Colonne?
    problemeAResoudre_.NombreDeContraintes = 0;
    problemeAResoudre_.NombreDeTermesDansLaMatriceDesContraintes = 0;

    setConstraintsOnFlows(Pi, Colonne);
    setNodeBalanceConstraints(Pi, Colonne);
    setBindingConstraints(Pi, Colonne);
}

} // namespace Antares::Solver::Optimization
