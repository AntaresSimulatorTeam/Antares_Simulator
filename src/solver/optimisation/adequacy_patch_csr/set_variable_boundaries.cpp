// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <pi_constantes_externes.h>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/solver/modeler/ModelerData.h>
#include <antares/study/system-model/component.h>
#include <antares/study/system-model/portType.h>
#include <antares/study/system-model/system.h>
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace Yuni;

double computeGemsContributionForArea(const PROBLEME_HEBDO* problemeHebdo,
                                      uint32_t area,
                                      int triggeredHour,
                                      const std::string& portFieldName)
{
    auto* modelerData = problemeHebdo->modelerData;
    if (!modelerData || !modelerData->system)
    {
        return 0.0;
    }

    double contribution = 0.0;
    const std::string areaName = problemeHebdo->NomsDesPays[area];

    for (const auto& component: modelerData->system->Components())
    {
        for (const auto& [portId, connectedArea]: component.portToAreaConnections())
        {
            if (connectedArea != areaName)
            {
                continue;
            }

            const auto& port = component.findPort(portId, "");
            const auto& areaConnection = port.Type().areaConnection();
            if (!areaConnection)
            {
                continue;
            }

            std::string fieldId;
            if (portFieldName == "unsupplied_energy_bound")
            {
                fieldId = areaConnection->unsupplied_energy_bound;
            }
            else if (portFieldName == "spillage_bound")
            {
                fieldId = areaConnection->spillage_bound;
            }

            if (fieldId.empty())
            {
                continue;
            }

            auto* expressionNode = component.nodeAtPortField(portId, fieldId);

            auto* optimEntityContainer = problemeHebdo->optimEntityContainer;
            if (!optimEntityContainer)
            {
                continue;
            }

            const auto& scenario = modelerData->scenarioGroupRepository.scenario(
              component.getScenarioGroupId());
            Antares::LinearProblem::Api::FillContext fillContext(
              0,                                              // localFirst
              0,                                              // localLast
              triggeredHour + problemeHebdo->HeureDansLAnnee, // globalFirst
              triggeredHour + problemeHebdo->HeureDansLAnnee, // globalLast
              problemeHebdo->year);
            Expressions::Visitors::EvalVisitor evalVisitor(*optimEntityContainer,
                                                           fillContext,
                                                           component,
                                                           modelerData->dataSeries.get(),
                                                           scenario);
            contribution += evalVisitor.dispatch(expressionNode).valuesAsVector()[0];

            (void)expressionNode;
        }
    }
    return contribution;
}

void HourlyCSRProblem::setBoundsOnENS()
{
    double* AdresseDuResultat;

    // variables: ENS for each area inside adq patch
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            int var = variableManager_.UnsuppliedEnergy(area, triggeredHour);

            problemeAResoudre_.Xmin[var] = -belowThisThresholdSetToZero;
            double ensLegacy = problemeHebdo_->ResultatsHoraires[area]
                                 .ValeursHorairesDENS[triggeredHour];
            double gemsContribution = computeGemsContributionForArea(problemeHebdo_,
                                                                     area,
                                                                     triggeredHour,
                                                                     "unsupplied_energy_bound");
            problemeAResoudre_.Xmax[var] = ensLegacy + gemsContribution
                                           + belowThisThresholdSetToZero;

            problemeAResoudre_.X[var] = problemeHebdo_->ResultatsHoraires[area]
                                          .ValeursHorairesDeDefaillancePositive[triggeredHour];

            AdresseDuResultat = &(problemeHebdo_->ResultatsHoraires[area]
                                    .ValeursHorairesDeDefaillancePositive[triggeredHour]);

            problemeAResoudre_.AdresseOuPlacerLaValeurDesVariablesOptimisees[var]
              = AdresseDuResultat;

            logs.debug() << var << ": " << problemeAResoudre_.Xmin[var] << ", "
                         << problemeAResoudre_.Xmax[var];
        }
    }
}

void HourlyCSRProblem::setBoundsOnSpilledEnergy()
{
    // variables: Spilled Energy for each area inside adq patch
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            int var = variableManager_.Spillage(area, triggeredHour);

            problemeAResoudre_.Xmin[var] = -belowThisThresholdSetToZero;
            problemeAResoudre_.Xmax[var] = LINFINI_ANTARES;

            problemeAResoudre_.X[var] = problemeHebdo_->ResultatsHoraires[area]
                                          .ValeursHorairesDeDefaillanceNegative[triggeredHour];

            double* AdresseDuResultat = &(problemeHebdo_->ResultatsHoraires[area]
                                            .ValeursHorairesDeDefaillanceNegative[triggeredHour]);

            problemeAResoudre_.AdresseOuPlacerLaValeurDesVariablesOptimisees[var]
              = AdresseDuResultat;

            logs.debug() << var << ": " << problemeAResoudre_.Xmin[var] << ", "
                         << problemeAResoudre_.Xmax[var];
        }
    }
}

void HourlyCSRProblem::setBoundsOnFlows()
{
    std::vector<double>& Xmin = problemeAResoudre_.Xmin;
    std::vector<double>& Xmax = problemeAResoudre_.Xmax;
    VALEURS_DE_NTC_ET_RESISTANCES& ValeursDeNTC = problemeHebdo_->ValeursDeNTC[triggeredHour];

    // variables bounds: transmissin flows (flow, direct_direct and flow_indirect). For links
    // between nodes of type 2. Set hourly bounds for links between nodes of type 2, depending on
    // the user input (max direct and indirect flow).
    for (uint32_t Interco = 0; Interco < problemeHebdo_->NombreDInterconnexions; ++Interco)
    {
        // only consider link between 2 and 2
        if (problemeHebdo_->adequacyPatchRuntimeData->originAreaMode[Interco]
              != Data::AdequacyPatch::physicalAreaInsideAdqPatch
            || problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode[Interco]
                 != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            continue;
        }

        // flow
        int var = variableManager_.DirectFlow(Interco, triggeredHour);
        Xmax[var] = ValeursDeNTC.ValeurDeNTCOrigineVersExtremite[Interco]
                    + belowThisThresholdSetToZero;
        Xmin[var] = -(ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine[Interco])
                    - belowThisThresholdSetToZero;
        problemeAResoudre_.X[var] = ValeursDeNTC.ValeurDuFlux[Interco];

        if (std::isinf(Xmax[var]))
        {
            if (std::isinf(Xmin[var]))
            {
                problemeAResoudre_.TypeDeVariable[var] = VARIABLE_NON_BORNEE;
            }
            else
            {
                problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
            }
        }
        else
        {
            if (std::isinf(Xmin[var]))
            {
                problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_SUPERIEUREMENT;
            }
            else
            {
                problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
            }
        }

        double* AdresseDuResultat = &(ValeursDeNTC.ValeurDuFlux[Interco]);
        problemeAResoudre_.AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = AdresseDuResultat;

        logs.debug() << var << ": " << problemeAResoudre_.Xmin[var] << ", "
                     << problemeAResoudre_.Xmax[var];

        // direct / indirect flow
        var = variableManager_.PositiveDirectFlow(Interco, triggeredHour);

        Xmin[var] = -belowThisThresholdSetToZero;
        Xmax[var] = ValeursDeNTC.ValeurDeNTCOrigineVersExtremite[Interco]
                    + belowThisThresholdSetToZero;
        problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
        if (std::isinf(Xmax[var]))
        {
            problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
        }

        logs.debug() << var << ": " << problemeAResoudre_.Xmin[var] << ", "
                     << problemeAResoudre_.Xmax[var];

        var = variableManager_.PositiveIndirectFlow(Interco, triggeredHour);

        Xmin[var] = -belowThisThresholdSetToZero;
        Xmax[var] = ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine[Interco]
                    + belowThisThresholdSetToZero;
        problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
        if (std::isinf(Xmax[var]))
        {
            problemeAResoudre_.TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
        }

        logs.debug() << var << ": " << problemeAResoudre_.Xmin[var] << ", "
                     << problemeAResoudre_.Xmax[var];
    }
}
