// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/simplex/LpFiller.h"

#include <antares/logs/logs.h>
#include "antares/optimisation/linear-problem-api/linearProblemBuilder.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"
#include "antares/solver/optimisation/ComponentToAreaConnectionFiller.h"
#include "antares/solver/optimisation/LegacyFiller.h"
#include "antares/solver/optimisation/ThermalCapacityFiller.h"
#include "antares/study/system-model/system.h"

using Antares::Optimisation::BendersDecomposition;
using Antares::Optimisation::ComponentFiller;
using Antares::Optimisation::OptimEntityContainer;
using Antares::Optimisation::LinearProblemApi::FillContext;
using Antares::Optimisation::LinearProblemApi::LinearProblemBuilder;
using Antares::Optimization::LegacyFiller;
using Antares::Optimization::ThermalCapacityFiller;

using Antares::Utils::TimeMeasurement;

namespace Antares::Solver::Optimization::Simplex
{

void LpFiller::fillModelerComponents(
  std::vector<std::unique_ptr<Optimisation::LinearProblemApi::LinearProblemFiller>>&
    fillersCollection,
  Antares::Solver::ModelerData* modelerData,
  OptimEntityContainer& optimEntityContainer,
  BendersDecomposition* bendersDecomposition)
{
    const auto& components = modelerData->system->Components();
    optimEntityContainer.addFromSystemComponents(components);
    for (const auto& component: components)
    {
        fillersCollection.push_back(
          std::make_unique<ComponentFiller>(component,
                                            modelerData->dataSeries.get(),
                                            optimEntityContainer,
                                            modelerData->scenarioGroupRepository,
                                            Antares::Solver::Config::Location::SUBPROBLEMS,
                                            bendersDecomposition));
    }
}

Optimisation::LinearProblemApi::FillContext LpFiller::buildFillContext(
  const PROBLEME_HEBDO* problemeHebdo,
  int NumIntervalle)
{
    unsigned globalFirst, globalLast;
    unsigned localFirst = 0, localLast;
    auto nTsInDay = HOURS_PER_DAY;
    if (problemeHebdo->OptimisationAuPasHebdomadaire)
    {
        globalFirst = static_cast<unsigned>(problemeHebdo->HeureDansLAnnee);
        globalLast = globalFirst + nTsInDay * problemeHebdo->NombreDeJours - 1;
        localLast = nTsInDay * problemeHebdo->NombreDeJours - 1;
    }
    else
    {
        globalFirst = static_cast<unsigned>(problemeHebdo->HeureDansLAnnee)
                      + static_cast<unsigned>(NumIntervalle) * nTsInDay;
        globalLast = globalFirst + nTsInDay - 1;
        localLast = nTsInDay - 1;
    }
    return {localFirst, localLast, globalFirst, globalLast, problemeHebdo->year};
}

void LpFiller::fillLinearProblem(const Optimisation::LinearProblemApi::FillContext& fillCtx,
                                 PROBLEME_HEBDO* problemeHebdo,
                                 Optimisation::OptimEntityContainer& optimEntityContainer,
                                 Optimisation::BendersDecomposition* bendersDecomposition)
{
    std::vector<std::unique_ptr<Optimisation::LinearProblemApi::LinearProblemFiller>>
      fillersCollection;
    fillersCollection.push_back(
      std::make_unique<Antares::Optimization::LegacyFiller>(optimEntityContainer.Problem(),
                                                            problemeHebdo));
    TimeMeasurement measure;
    if (problemeHebdo->modelerData)
    {
        // All LP variables coordinates (component id, variable id, scenario, time step)
        fillModelerComponents(fillersCollection,
                              problemeHebdo->modelerData,
                              optimEntityContainer,
                              bendersDecomposition);

        // Add compatibility filler that connects components to areas
        // Must be the last one, because it uses constraints defined by the other fillers !!
        fillersCollection.push_back(
          std::make_unique<Antares::Optimization::ComponentToAreaConnectionFiller>(
            problemeHebdo,
            optimEntityContainer,
            problemeHebdo->modelerData->dataSeries.get(),
            problemeHebdo->modelerData->scenarioGroupRepository));

        fillersCollection.push_back(std::make_unique<Antares::Optimization::ThermalCapacityFiller>(
          problemeHebdo,
          optimEntityContainer,
          problemeHebdo->modelerData->dataSeries.get(),
          problemeHebdo->modelerData->scenarioGroupRepository));
    }

    LinearProblemBuilder linearProblemBuilder(fillersCollection);

    // Note that the modeler is only called for the 1st simulation week,
    // this limitation must be lifted later,
    // when appropriate solvers (e.g with warm start) is integrated.
    linearProblemBuilder.build(fillCtx);

    measure.tick();

    Antares::logs.debug() << "Modeler build took " << measure.toStringInSeconds();
}

} // namespace Antares::Solver::Optimization::Simplex

// ──────────────────────────────────────────────────────────────
// Free-function wrappers for backward compatibility.
// ──────────────────────────────────────────────────────────────
Antares::Optimisation::LinearProblemApi::FillContext buildFillContext(
  const PROBLEME_HEBDO* problemeHebdo,
  int NumIntervalle)
{
    return Antares::Solver::Optimization::Simplex::LpFiller::buildFillContext(problemeHebdo,
                                                                              NumIntervalle);
}

void fillLinearProblem(const Antares::Optimisation::LinearProblemApi::FillContext& fillCtx,
                       PROBLEME_HEBDO* problemeHebdo,
                       Antares::Optimisation::OptimEntityContainer& optimEntityContainer,
                       Antares::Optimisation::BendersDecomposition* bendersDecomposition)
{
    Antares::Solver::Optimization::Simplex::LpFiller::fillLinearProblem(fillCtx,
                                                                        problemeHebdo,
                                                                        optimEntityContainer,
                                                                        bendersDecomposition);
}
