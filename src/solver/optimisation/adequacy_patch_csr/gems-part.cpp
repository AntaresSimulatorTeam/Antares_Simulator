

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

ActiveGemsPart::ActiveGemsPart(PROBLEME_HEBDO* problemeHebdo,
                               PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre,
                               VariableManagement::VariableManager& variableManager,
                               std::map<int, int>& constraintCsrFictitiousLoad,
                               std::map<int, int>& constraintCsrMaxEnsLoad):
    problemeHebdo_(problemeHebdo),
    problemeAResoudre_(problemeAResoudre),
    variableManager_(variableManager),
    constraintCsrFictitiousLoad_(constraintCsrFictitiousLoad),
    constraintCsrMaxEnsLoad_(constraintCsrMaxEnsLoad)
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

            auto* expression = component.nodeAtPortField(portId, fieldId);

            const auto& scenario = modelerData->scenarioGroupRepository.scenario(
              component.getScenarioGroupId());
            Visitors::EvalVisitor evalVisitor(*problemeHebdo_->optimEntityContainer,
                                              fillContext_,
                                              component,
                                              modelerData->dataSeries.get(),
                                              scenario);
            contribution += evalVisitor.dispatch(expression).value(0);
        }
    }
    return contribution;
}

double ActiveGemsPart::gemsUnsupEnergyForArea(uint32_t area) const
{
    return gemsContributionForArea(area, getUnsupEnergyBound);
}

void ActiveGemsPart::setBoundsOnENS()
{
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area] == physicalAreaInsideAdqPatch)
        {
            int var = variableManager_.UnsuppliedEnergy(area, triggeredHour_);
            problemeAResoudre_.Xmax[var] += gemsUnsupEnergyForArea(area);
        }
    }
}

void ActiveGemsPart::setRHSfictitiousLoadValue()
{
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area] == physicalAreaInsideAdqPatch)
        {
            auto it = constraintCsrFictitiousLoad_.find(area);
            if (it != constraintCsrFictitiousLoad_.end())
            {
                int Cnt = it->second;
                problemeAResoudre_.SecondMembre[Cnt] += gemsSpilledForArea(area);
            }
        }
    }
}

void ActiveGemsPart::setRHSMaxEnsLoadValue()
{
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area] == physicalAreaInsideAdqPatch)
        {
            auto it = constraintCsrMaxEnsLoad_.find(area);
            if (it != constraintCsrMaxEnsLoad_.end())
            {
                int Cnt = it->second;
                problemeAResoudre_.SecondMembre[Cnt] += gemsUnsupEnergyForArea(area);
            }
        }
    }
}

double ActiveGemsPart::gemsSpilledForArea(uint32_t area) const
{
    return gemsContributionForArea(area, Antares::ModelerStudy::SystemModel::getSpilledBound);
}

// Factory
std::unique_ptr<IGemsPart> makeGemsPart(PROBLEME_HEBDO* problemeHebdo,
                                        PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre,
                                        VariableManagement::VariableManager& variableManager,
                                        std::map<int, int>& constraintCsrFictitiousLoad,
                                        std::map<int, int>& constraintCsrMaxEnsLoad)
{
    if (problemeHebdo->modelerData && problemeHebdo->modelerData->system)
    {
        return std::make_unique<ActiveGemsPart>(problemeHebdo,
                                                problemeAResoudre,
                                                variableManager,
                                                constraintCsrFictitiousLoad,
                                                constraintCsrMaxEnsLoad);
    }
    return std::make_unique<NullGemsPart>();
}
