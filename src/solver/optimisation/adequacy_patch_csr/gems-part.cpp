

#include "antares/solver/optimisation/adequacy_patch_csr/gems-part.h"

#include <cstdint>
#include <ranges>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/solver/modeler/ModelerData.h>
#include <antares/study/system-model/component.h>
#include <antares/study/system-model/system.h>
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/optimisation/simplex/LpFiller.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace Antares::Data::AdequacyPatch;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Expressions;
using namespace Antares::LinearProblem::Api;

namespace
{
std::vector<double> evaluateContributions(
  PROBLEME_HEBDO& problem,
  uint32_t area,
  const FillContext& context,
  const std::function<std::string(const AreaConnection&)>& getFieldId)
{
    auto* modelerData = problem.modelerData;
    std::vector<double> contributions(context.getLocalNumberOfTimeSteps(), 0.);
    const std::string areaName = problem.NomsDesPays[area];
    const auto isConnectedToArea = [areaName](const auto& p) { return p.second == areaName; };
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
            const std::string fieldId = getFieldId(*areaConnection);
            if (fieldId.empty())
            {
                continue;
            }
            const auto& scenario = modelerData->scenarioGroupRepository.scenario(
              component.getScenarioGroupId());
            Visitors::EvalVisitor visitor(*problem.optimEntityContainer,
                                          context,
                                          component,
                                          modelerData->dataSeries.get(),
                                          scenario);
            const auto result = visitor.dispatch(component.nodeAtPortField(portId, fieldId));
            for (unsigned hour = 0; hour < contributions.size(); ++hour)
            {
                contributions[hour] += result.value(hour);
            }
        }
    }
    return contributions;
}
} // namespace

void captureDailyGemsContributions(PROBLEME_HEBDO& problem, unsigned interval)
{
    const auto context = Antares::Solver::Optimization::Simplex::LpFiller::buildFillContext(
      problem,
      interval);
    const auto firstHour = context.getGlobalFirstTimeStep()
                           - static_cast<unsigned>(problem.HeureDansLAnnee);
    auto& runtime = *problem.adequacyPatchRuntimeData;
    for (uint32_t area = 0; area < problem.NombreDePays; ++area)
    {
        if (runtime.areaMode[area] != physicalAreaInsideAdqPatch)
        {
            continue;
        }
        const auto spillage = evaluateContributions(problem, area, context, getSpilledBound);
        const auto unsupplied = evaluateContributions(problem, area, context, getUnsupEnergyBound);
        for (unsigned hour = 0; hour < spillage.size(); ++hour)
        {
            runtime.setDailyGemsContribution(area,
                                             firstHour + hour,
                                             {spillage[hour], unsupplied[hour]});
        }
    }
}

ActiveGemsPart::ActiveGemsPart(PROBLEME_HEBDO* problemeHebdo,
                               PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre,
                               std::map<int, int>& constraintCsrFictitiousLoad,
                               std::map<int, int>& constraintCsrMaxEnsLoad):
    problemeHebdo_(problemeHebdo),
    problemeAResoudre_(problemeAResoudre),
    constraintCsrFictitiousLoad_(constraintCsrFictitiousLoad),
    constraintCsrMaxEnsLoad_(constraintCsrMaxEnsLoad)
{
    if (!problemeHebdo_->optimEntityContainer)
    {
        throw MissingOptimEntityContainerError(
          "optimEntityContainer is null but GEMS data is present");
    }
}

void ActiveGemsPart::setHour(int triggeredHour)
{
    triggeredHour_ = static_cast<unsigned>(triggeredHour);
    if (problemeHebdo_->OptimisationAuPasHebdomadaire)
    {
        // Variables are stored over the entire solved interval; time shifts also use that interval.
        fillContext_ = Antares::Solver::Optimization::Simplex::LpFiller::buildFillContext(
          *problemeHebdo_,
          0);
        evaluationHour_ = static_cast<unsigned>(triggeredHour);
    }
}

double ActiveGemsPart::gemsContributionForArea(
  uint32_t area,
  const std::function<std::string(const AreaConnection&)>& getFieldId) const
{
    return evaluateContributions(*problemeHebdo_, area, fillContext_, getFieldId)
      .at(evaluationHour_);
}

double ActiveGemsPart::gemsUnsupEnergyForArea(uint32_t area) const
{
    if (!problemeHebdo_->OptimisationAuPasHebdomadaire)
    {
        return problemeHebdo_->adequacyPatchRuntimeData->dailyGemsContribution(area, triggeredHour_)
          .unsuppliedEnergy;
    }
    return gemsContributionForArea(area, getUnsupEnergyBound);
}

void ActiveGemsPart::setRHSfictitiousLoadValue()
{
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area] == physicalAreaInsideAdqPatch)
        {
            if (auto it = constraintCsrFictitiousLoad_.find(area);
                it != constraintCsrFictitiousLoad_.end())
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
            if (auto it = constraintCsrMaxEnsLoad_.find(area); it != constraintCsrMaxEnsLoad_.end())
            {
                int Cnt = it->second;
                problemeAResoudre_.SecondMembre[Cnt] += gemsUnsupEnergyForArea(area);
            }
        }
    }
}

double ActiveGemsPart::gemsSpilledForArea(uint32_t area) const
{
    if (!problemeHebdo_->OptimisationAuPasHebdomadaire)
    {
        return problemeHebdo_->adequacyPatchRuntimeData->dailyGemsContribution(area, triggeredHour_)
          .spillage;
    }
    return gemsContributionForArea(area, getSpilledBound);
}

// Factory
std::unique_ptr<IGemsPart> makeGemsPart(PROBLEME_HEBDO* problemeHebdo,
                                        PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre,
                                        std::map<int, int>& constraintCsrFictitiousLoad,
                                        std::map<int, int>& constraintCsrMaxEnsLoad)
{
    if (problemeHebdo->modelerData && problemeHebdo->modelerData->system)
    {
        return std::make_unique<ActiveGemsPart>(problemeHebdo,
                                                problemeAResoudre,
                                                constraintCsrFictitiousLoad,
                                                constraintCsrMaxEnsLoad);
    }
    return std::make_unique<NullGemsPart>();
}
