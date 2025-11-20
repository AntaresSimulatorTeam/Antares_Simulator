/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include <mutex>

#include <antares/antares/fatal-error.h>
#include <antares/logs/logs.h>
#include <antares/solver/utils/ortools_utils.h>
#include "antares/expressions/visitors/TimeIndexVisitor.h"
#include "antares/io/outputs/ISimulationTable.h"
#include "antares/io/outputs/SimulationTableCsv.h"
#include "antares/io/outputs/SimulationTableGenerator.h"
#include "antares/optimisation/linear-problem-api/linearProblemBuilder.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/convertOrtoolsBasisStatus.h"
#include "antares/optimization-options/options.h"
#include "antares/solver/infeasible-problem-analysis/unfeasible-pb-analyzer.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"
#include "antares/solver/optimisation/ComponentToAreaConnectionFiller.h"
#include "antares/solver/optimisation/LegacyFiller.h"
#include "antares/solver/optimisation/LegacyOrtoolsLinearProblem.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/utils/filename.h"
#include "antares/solver/utils/mps_utils.h"
#include "antares/study/system-model/system.h"

#include "spx_constantes_externes.h"

using namespace operations_research;
using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemApi;
using namespace Antares::Optimisation::LinearProblemMpsolverImpl;
using namespace Antares::IO;

using Solver::IResultWriter;
using Solver::Optimization::SingleOptimOptions;

struct SimplexResult
{
    TIME_MEASURE timeMeasure;
    mpsWriterFactory mps_writer_factory;
    double objectiveValue;
};

static std::once_flag logProblemSizeFlag;

static void logProblemSize(const MPSolver* mpSolver)
{
    logs.info();
    logs.info();
    logs.info() << " Total Problem size : " << mpSolver->NumVariables() << " variables, "
                << mpSolver->NumConstraints() << " constraints";
    logs.info();
    logs.info();
}

static void fillModelerComponents(
  std::vector<std::unique_ptr<LinearProblemFiller>>& fillersCollection,
  Modeler::Data* modelerData,
  OptimEntityContainer& optimEntityContainer)
{
    const auto& components = modelerData->system->Components();
    optimEntityContainer.addFromSystemComponents(components);
    for (const auto& component: components)
    {
        fillersCollection.push_back(
          std::make_unique<ComponentFiller>(component,
                                            optimEntityContainer,
                                            modelerData->scenarioGroupRepository,
                                            Modeler::Config::Location::SUBPROBLEMS,
                                            nullptr));
    }
}

FillContext buildFillContext(const PROBLEME_HEBDO* problemeHebdo, int NumIntervalle)
{
    unsigned globalFirst, globalLast;
    unsigned localFirst = 0, localLast;
    auto nTsInDay = static_cast<unsigned>(problemeHebdo->NombreDePasDeTempsDUneJournee);
    if (problemeHebdo->OptimisationAuPasHebdomadaire)
    {
        globalFirst = problemeHebdo->weekInTheYear * nTsInDay * problemeHebdo->NombreDeJours;
        globalLast = globalFirst + nTsInDay * problemeHebdo->NombreDeJours - 1;
        localLast = nTsInDay * problemeHebdo->NombreDeJours - 1;
    }
    else
    {
        globalFirst = (problemeHebdo->weekInTheYear * problemeHebdo->NombreDeJours
                       + static_cast<unsigned>(NumIntervalle))
                      * nTsInDay;
        globalLast = globalFirst + nTsInDay - 1;
        localLast = nTsInDay - 1;
    }
    return {localFirst,
            localLast,
            globalFirst,
            globalLast,
            problemeHebdo->year}; // TODO: handle scenarios/year
}

// Returns a non-owning pointer
MPSolver* fillAndGetMpSolver(LegacyOrtoolsLinearProblem& ortoolsProblem,
                             FillContext& fillCtx,
                             const PROBLEME_HEBDO* problemeHebdo,
                             OptimEntityContainer& optimEntityContainer,
                             bool namedProblems)
{
    std::vector<std::unique_ptr<LinearProblemFiller>> fillersCollection;
    fillersCollection.push_back(
      std::make_unique<LegacyFiller>(optimEntityContainer.Problem(), problemeHebdo, namedProblems));
    Utils::TimeMeasurement measure;
    if (problemeHebdo->modelerData)
    {
        // All LP variables coordinates (component id, variable id, scenario, time step)
        fillModelerComponents(fillersCollection, problemeHebdo->modelerData, optimEntityContainer);

        // Add compatibility filler that connects components to areas
        // Must be the last one, because it uses constraints defined by the other fillers !!
        fillersCollection.push_back(std::make_unique<ComponentToAreaConnectionFiller>(
          problemeHebdo,
          optimEntityContainer,
          *problemeHebdo->modelerData->dataSeries,
          problemeHebdo->modelerData->scenarioGroupRepository));
    }

    LinearProblemBuilder linearProblemBuilder(fillersCollection);

    // Note that the modeler is only called for the 1st simulation week,
    // this limitation must be lifted later,
    // when appropriate solvers (e.g with warm start) is integrated.
    // TODO try to make this cleaner
    linearProblemBuilder.build(fillCtx);

    measure.tick();

    logs.debug() << "Modeler build took " << measure.toStringInSeconds();

    return ortoolsProblem.getMpSolver();
}

static SimplexResult OPT_TryToCallSimplex(const SingleOptimOptions& options,
                                          PROBLEME_HEBDO* problemeHebdo,
                                          const int NumIntervalle,
                                          const int optimizationNumber,
                                          const OptPeriodStringGenerator& optPeriodStringGenerator,
                                          IResultWriter& writer,
                                          ISimulationTable* simulationTable)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    const int opt = optimizationNumber - 1;
    assert(opt >= 0 && opt < 2);
    OptimizationStatistics& optimizationStatistics = problemeHebdo->optimizationStatistics[opt];
    TIME_MEASURE timeMeasure;

    LegacyOrtoolsLinearProblem ortoolsProblem(problemeHebdo->ProblemeAResoudre->isMIP(),
                                              options.solverName);
    FillContext fillCtx = buildFillContext(problemeHebdo, NumIntervalle);
    const auto& modelerData = problemeHebdo->modelerData;
    bool hasModelerData = modelerData != nullptr;
    const Optimisation::LinearProblemApi::ILinearProblemData* modelerDataSeries = hasModelerData
                                                                                    ? modelerData
                                                                                        ->dataSeries
                                                                                        .get()
                                                                                    : nullptr;
    const Optimisation::ScenarioGroupRepository* modelerScenarioGroupRepository
      = hasModelerData ? &modelerData->scenarioGroupRepository : nullptr;

    OptimEntityContainer optimEntityContainer(ortoolsProblem,
                                              modelerDataSeries,
                                              modelerScenarioGroupRepository);

    auto* solver = fillAndGetMpSolver(ortoolsProblem,
                                      fillCtx,
                                      problemeHebdo,
                                      optimEntityContainer,
                                      problemeHebdo->NamedProblems);

    ProblemeAResoudre->ProblemesSpx[NumIntervalle].reset(solver);

    std::call_once(logProblemSizeFlag, logProblemSize, solver);

    const std::string filename = createMPSfilename(optPeriodStringGenerator, optimizationNumber);

    mpsWriterFactory mps_writer_factory(problemeHebdo->ExportMPS,
                                        problemeHebdo->exportMPSOnError,
                                        optimizationNumber,
                                        solver);

    auto mps_writer = mps_writer_factory.create();
    mps_writer->runIfNeeded(writer, filename);

    Utils::TimeMeasurement measure;
    ORTOOLS_Simplexe(ProblemeAResoudre.get(), solver, options);

    measure.tick();
    logs.info() << fmt::format("Problem {}-{} solved in {}",
                               problemeHebdo->weekInTheYear,
                               problemeHebdo->year,
                               measure.toStringInSeconds());
    timeMeasure.solveTime = measure.duration_ms();
    optimizationStatistics.addSolveTime(timeMeasure.solveTime);

    if (ProblemeAResoudre->ExistenceDUneSolution != OUI_SPX)
    {
        if (ProblemeAResoudre->ExistenceDUneSolution != SPX_ERREUR_INTERNE)
        {
            if (solver)
            {
                ProblemeAResoudre->ProblemesSpx[NumIntervalle].reset();
            }

            logs.info() << " Solver: resolution failed";
            logs.debug() << " solver: resetting";

            return {.timeMeasure = timeMeasure,
                    .mps_writer_factory = mps_writer_factory,
                    .objectiveValue = 0};
        }
        throw FatalError("Internal error: insufficient memory");
    }

    if (simulationTable && modelerData)
    {
        unsigned currentBlock = problemeHebdo->OptimisationAuPasHebdomadaire
                                  ? problemeHebdo->weekInTheYear
                                  : problemeHebdo->weekInTheYear * 7 + NumIntervalle;
        TimeConversionMode timeConversionMode = problemeHebdo->OptimisationAuPasHebdomadaire
                                                  ? TimeConversionMode::WeeklyBlocks
                                                  : TimeConversionMode::DailyBlocks;
        FillSimulationTable(*simulationTable,
                            ortoolsProblem,
                            ::getObjectiveValue(solver),
                            *modelerData,
                            optimEntityContainer,
                            fillCtx,
                            currentBlock,
                            timeConversionMode,
                            true);
    }

    return {.timeMeasure = timeMeasure,
            .mps_writer_factory = mps_writer_factory,
            .objectiveValue = getObjectiveValue(solver)};
}

bool OPT_AppelDuSimplexe(const SingleOptimOptions& options,
                         PROBLEME_HEBDO* problemeHebdo,
                         int NumIntervalle,
                         const int optimizationNumber,
                         const OptPeriodStringGenerator& optPeriodStringGenerator,
                         IResultWriter& writer,
                         ISimulationTable* simulationTable)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    SimplexResult simplexResult = OPT_TryToCallSimplex(options,
                                                       problemeHebdo,
                                                       NumIntervalle,
                                                       optimizationNumber,
                                                       optPeriodStringGenerator,
                                                       writer,
                                                       simulationTable);

    if (ProblemeAResoudre->ExistenceDUneSolution == OUI_SPX)
    {
        double* pt;
        double optimizationCost = simplexResult.objectiveValue;

        for (int i = 0; i < ProblemeAResoudre->NombreDeVariables; i++)
        {
            pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[i];
            if (pt != nullptr)
            {
                *pt = ProblemeAResoudre->X[i];
            }

            pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits[i];
            if (pt != nullptr)
            {
                *pt = ProblemeAResoudre->CoutsReduits[i];
            }
        }
        {
            const int opt = optimizationNumber - 1;
            assert(opt >= 0 && opt < 2);
            problemeHebdo->timeMeasure[opt] = simplexResult.timeMeasure;
        }

        // TODO remove this if..else
        if (optimizationNumber == PREMIERE_OPTIMISATION)
        {
            problemeHebdo->coutOptimalSolution1[static_cast<unsigned int>(NumIntervalle)]
              = optimizationCost;
        }
        else
        {
            problemeHebdo->coutOptimalSolution2[static_cast<unsigned int>(NumIntervalle)]
              = optimizationCost;
        }
        for (int Cnt = 0; Cnt < ProblemeAResoudre->NombreDeContraintes; Cnt++)
        {
            pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt];
            if (pt != nullptr)
            {
                *pt = ProblemeAResoudre->CoutsMarginauxDesContraintes[Cnt];
            }
        }
    }
    else
    {
        LegacyOrtoolsLinearProblem infeasibleProblem(problemeHebdo->ProblemeAResoudre->isMIP(),
                                                     options.solverName);
        FillContext fillCtx = buildFillContext(problemeHebdo, NumIntervalle);
        const auto& modelerData = problemeHebdo->modelerData;
        bool hasModelerData = modelerData != nullptr;
        const Optimisation::LinearProblemApi::ILinearProblemData* modelerDataSeries
          = hasModelerData ? modelerData->dataSeries.get() : nullptr;
        const Optimisation::ScenarioGroupRepository* modelerScenarioGroupRepository
          = hasModelerData ? &modelerData->scenarioGroupRepository : nullptr;

        OptimEntityContainer optimEntityContainer(infeasibleProblem,
                                                  modelerDataSeries,
                                                  modelerScenarioGroupRepository);
        std::unique_ptr<MPSolver> MPproblem(fillAndGetMpSolver(infeasibleProblem,
                                                               fillCtx,
                                                               problemeHebdo,
                                                               optimEntityContainer,
                                                               true));

        auto analyzer = makeUnfeasiblePbAnalyzer();
        analyzer->run(MPproblem.get());
        analyzer->printReport();
        mpsWriterFactory mps_writer_factory(problemeHebdo->ExportMPS,
                                            problemeHebdo->exportMPSOnError,
                                            optimizationNumber,
                                            MPproblem.get());
        // Since MpProblem must have named vars and constraints in case of infeasibility, we must
        // use the updated MPSolver
        auto mps_writer_on_error = mps_writer_factory.createOnOptimizationError();
        const std::string filename = createMPSfilename(optPeriodStringGenerator,
                                                       optimizationNumber);
        mps_writer_on_error->runIfNeeded(writer, filename);

        return false;
    }

    return true;
}
