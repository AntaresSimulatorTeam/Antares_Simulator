

#include "antares/solver/optimisation/adequacy_patch_csr/gems-part.h"

#include <ranges>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/solver/modeler/ModelerData.h>
#include <antares/study/system-model/component.h>
#include <antares/study/system-model/system.h>
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/optimisation/variables/VariableManagerUtils.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace Antares::Data::AdequacyPatch;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Expressions;
using namespace Antares::LinearProblem::Api;

ActiveGemsPart::ActiveGemsPart(PROBLEME_HEBDO* problemeHebdo):
    problemeHebdo_(problemeHebdo)
{
    if (!problemeHebdo_->optimEntityContainer)
    {
        throw std::runtime_error("optimEntityContainer is null but GEMS data is present");
    }
}

void ActiveGemsPart::setHour(int triggeredHour)
{
    triggeredHour_ = triggeredHour;
    fillContext_ = FillContext(0,
                               0,
                               triggeredHour_ + problemeHebdo_->HeureDansLAnnee,
                               triggeredHour_ + problemeHebdo_->HeureDansLAnnee,
                               problemeHebdo_->year);
}

double ActiveGemsPart::gemsContributionForArea(
  uint32_t area,
  const std::function<std::string(const AreaConnection&)>& getFieldId) const
{
    auto* modelerData = problemeHebdo_->modelerData;
    double contribution = 0.0;
    const std::string areaName = problemeHebdo_->NomsDesPays[area];
    const auto isConnectedToArea = [&](const auto& p) { return p.second == areaName; };
    const auto filterPort = std::views::filter(isConnectedToArea) | std::views::keys;

    for (const auto& component: modelerData->system->Components())
    {
        for (const auto& portId: component.portToAreaConnections() | filterPort)
        {
            const auto& port = component.findPort(portId, "");
            const auto& areaConnection = port.Type().areaConnection();
            if (!areaConnection)
            {
                continue;
            }

            std::string fieldId = getFieldId(*areaConnection);
            if (fieldId.empty())
            {
                continue;
            }

            auto* expressionNode = component.nodeAtPortField(portId, fieldId);
            auto* optimEntityContainer = problemeHebdo_->optimEntityContainer.get();

            const auto& scenario = modelerData->scenarioGroupRepository.scenario(
              component.getScenarioGroupId());
            Visitors::EvalVisitor evalVisitor(*optimEntityContainer,
                                              fillContext_,
                                              component,
                                              modelerData->dataSeries.get(),
                                              scenario);
            contribution += evalVisitor.dispatch(expressionNode).value(0);
        }
    }
    return contribution;
}

double ActiveGemsPart::gemsUnsupEnergyForArea(uint32_t area) const
{
    return gemsContributionForArea(area, getUnsupEnergyBound);
}

void ActiveGemsPart::setBoundsOnENS(PROBLEME_ANTARES_A_RESOUDRE& problem,
                                    VariableManagement::VariableManager& varManager)
{
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area] == physicalAreaInsideAdqPatch)
        {
            int var = varManager.UnsuppliedEnergy(area, triggeredHour_);
            problem.Xmax[var] += gemsUnsupEnergyForArea(area);
        }
    }
}

void ActiveGemsPart::setRHSfictitiousLoadValue(PROBLEME_ANTARES_A_RESOUDRE& problem,
                                               std::map<int, int>& constraintMap)
{
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area] == physicalAreaInsideAdqPatch)
        {
            auto it = constraintMap.find(area);
            if (it != constraintMap.end())
            {
                int Cnt = it->second;
                problem.SecondMembre[Cnt] += gemsSpilledForArea(area);
            }
        }
    }
}

void ActiveGemsPart::setRHSMaxEnsLoadValue(PROBLEME_ANTARES_A_RESOUDRE& problem,
                                           std::map<int, int>& constraintMap)
{
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area] == physicalAreaInsideAdqPatch)
        {
            auto it = constraintMap.find(area);
            if (it != constraintMap.end())
            {
                int Cnt = it->second;
                problem.SecondMembre[Cnt] += gemsUnsupEnergyForArea(area);
            }
        }
    }
}

double ActiveGemsPart::gemsSpilledForArea(uint32_t area) const
{
    return gemsContributionForArea(area, Antares::ModelerStudy::SystemModel::getSpilledBound);
}

// Factory
std::unique_ptr<IGemsPart> makeGemsPart(PROBLEME_HEBDO* problemeHebdo)
{
    if (problemeHebdo->modelerData && problemeHebdo->modelerData->system)
    {
        return std::make_unique<ActiveGemsPart>(problemeHebdo);
    }
    return std::make_unique<NullGemsPart>();
}
