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

#include <chrono>

#include <antares/antares/fatal-error.h>
#include <antares/logs/logs.h>
#include <antares/solver/utils/ortools_utils.h>
#include "antares/expressions/nodes/ExpressionsNodes.h"
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
using namespace Antares::Optimisation::LinearProblemApi;
using namespace Antares::Optimisation::LinearProblemMpsolverImpl;

using Antares::Solver::IResultWriter;
using Antares::Solver::Optimization::SingleOptimOptions;

class TimeMeasurement
{
    using clock = std::chrono::steady_clock;

public:
    TimeMeasurement()
    {
        start_ = clock::now();
        end_ = start_;
    }

    void tick()
    {
        end_ = clock::now();
    }

    long duration_ms() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_ - start_).count();
    }

    std::string toString() const
    {
        return std::to_string(duration_ms()) + " ms";
    }

private:
    clock::time_point start_;
    clock::time_point end_;
};

struct SimplexResult
{
    bool success = false;
    TIME_MEASURE timeMeasure;
    mpsWriterFactory mps_writer_factory;
    double objectiveValue;
};

static void fillModelerComponents(std::vector<std::unique_ptr<ComponentFiller>>& componentFillers,
                                  std::vector<LinearProblemFiller*>& fillersCollection,
                                  const ModelerStudy::SystemModel::System* modelerSystem,
                                  VariableDictionary& variableDictionary)
{
    if (!modelerSystem)
    {
        logs.info() << "No modeler system found, optimization will only be done on legacy study";
        return;
    }

    for (const auto& [_, component]: modelerSystem->Components())
    {
        componentFillers.push_back(
          std::make_unique<ComponentFiller>(component, variableDictionary));
    }
    for (auto& component_filler: componentFillers)
    {
        fillersCollection.push_back(component_filler.get());
    }
}

static void writeModelerSolutions(const MPSolver* solver,
                                  unsigned nLegacyVariables,
                                  const int optimizationNumber,
                                  const OptPeriodStringGenerator& optPeriodStringGenerator,
                                  IResultWriter& writer)
{
    std::stringstream contentStream;
    const auto& variables = solver->variables();

    // we want to only get modeler variables, they're added after legacy vars
    // TODO make this cleaner (what happens if order changes? use modeler var dictionary instead?)
    auto start = variables.begin() + nLegacyVariables;
    if (start == variables.end())
    {
        logs.debug() << "No modeler solutions, skip writing files";
        return;
    }
    for (auto v = start; v < variables.end(); ++v)
    {
        contentStream << (*v)->name() << "\t" << (*v)->solution_value() << std::endl;
    }

    auto modelerSolutionFilename = createModelerSolutionsFilename(optPeriodStringGenerator,
                                                                  optimizationNumber);
    std::string content = contentStream.str();
    writer.addEntryFromBuffer(modelerSolutionFilename, content);
}

FillContext buildFillContext(const PROBLEME_HEBDO* problemeHebdo, int NumIntervalle)
{
    unsigned firstTimestep, lastTimestep;
    auto nTsInDay = static_cast<unsigned>(problemeHebdo->NombreDePasDeTempsDUneJournee);
    if (problemeHebdo->OptimisationAuPasHebdomadaire)
    {
        firstTimestep = problemeHebdo->weekInTheYear * nTsInDay * problemeHebdo->NombreDeJours;
        lastTimestep = firstTimestep + nTsInDay * problemeHebdo->NombreDeJours - 1;
    }
    else
    {
        firstTimestep = (problemeHebdo->weekInTheYear * problemeHebdo->NombreDeJours
                         + static_cast<unsigned>(NumIntervalle))
                        * nTsInDay;
        lastTimestep = firstTimestep + nTsInDay - 1;
    }
    return {firstTimestep, lastTimestep};
}

// Returns a non-owning pointer
MPSolver* convertToMPSolver(const PROBLEME_HEBDO* problemeHebdo,
                            const int NumIntervalle,
                            const SingleOptimOptions& options,
                            bool namedProblems)
{
    LegacyOrtoolsLinearProblem ortoolsProblem(problemeHebdo->ProblemeAResoudre->isMIP(),
                                              options.solverName);
    LegacyFiller legacyOrtoolsFiller(problemeHebdo, namedProblems);
    std::vector<LinearProblemFiller*> fillersCollection = {&legacyOrtoolsFiller};

    std::vector<std::unique_ptr<ComponentFiller>> componentFillers;
    VariableDictionary variableDictionary;
    ComponentToAreaConnectionFiller componentToAreaConnectionFiller(problemeHebdo,
                                                                    variableDictionary);
    if (problemeHebdo->modelerSystem)
    {
        // All LP variables coordinates (component id, variable id, scenario, time step)
        fillModelerComponents(componentFillers,
                              fillersCollection,
                              problemeHebdo->modelerSystem,
                              variableDictionary);

        // Add compatibility filler that connects components to areas
        // Must be the last one, because it uses constraints defined by the other fillers !!
        fillersCollection.push_back(&componentToAreaConnectionFiller);
    }

    FillContext fillCtx = buildFillContext(problemeHebdo, NumIntervalle);
    LinearProblemBuilder linearProblemBuilder(fillersCollection);

    // Note that the modeler is only called for the 1st simulation week,
    // this limitation must be lifted later,
    // when appropriate solvers (e.g with warm start) is integrated.
    linearProblemBuilder.build(ortoolsProblem, *problemeHebdo->linear_problem_data_, fillCtx);

    return ortoolsProblem.getMpSolver();
}

struct LegacySolverTraits
{
    static double getValue(const MPVariable* var)
    {
        return var->solution_value();
    }

    static MipBasisStatus getStatus(const MPVariable* var)
    {
        return var->integer()
                 ? MipBasisStatus::FREE
                 : convertOrtoolsBasisStatus(var->basis_status()); // TODO or return not available
    }

    static MipBasisStatus getStatus(const MPConstraint* cst)
    {
        try
        {
            return convertOrtoolsBasisStatus(cst->basis_status());
        }
        catch (const std::exception&)
        {
            return MipBasisStatus::FREE; // TODO
        }
    }

    static std::optional<double> getValue(const MPConstraint*)
    {
        return std::nullopt;
    }
};

static void FillSimulationTable(ISimulationTable& simulationTable,
                                const MPSolver* solver,
                                const PROBLEME_HEBDO* problemeHebdo,
                                const FillContext& fillContext,
                                int NumIntervalle)
{
    const auto& components = problemeHebdo->modelerSystem->Components();
    const auto& variables = solver->variables();

    std::unordered_map<std::string, const MPVariable*> variablesByName;
    // TODO maybe not necessary
    for (const auto* v: variables)
    {
        variablesByName.emplace(v->name(), v);
    }

    auto variableLookup = [&](const std::string& cid,
                              const std::string& name,
                              std::optional<unsigned> scen,
                              std::optional<unsigned> ts) -> const MPVariable*
    {
        // TODO or call solver->LookupVariableOrNull()
        return variablesByName.at(
          VariableDictionary::buildVariableName({cid, name},
                                                /* TODO should be = scen*/ std::nullopt,
                                                ts));
    };

    auto constraintLookup = [&](const std::string& cid,
                                const std::string& cname,
                                std::optional<unsigned> scen,
                                std::optional<unsigned> ts) -> const MPConstraint*
    {
        return solver->LookupConstraintOrNull(
          BuildModelerConstraintName(cid, cname, /* TODO should be = scen*/ std::nullopt, ts));
    };

    unsigned currentBlock = problemeHebdo->OptimisationAuPasHebdomadaire
                              ? problemeHebdo->weekInTheYear
                              : NumIntervalle;

    TimeConversionMode timeConversionMode = problemeHebdo->OptimisationAuPasHebdomadaire
                                              ? TimeConversionMode::WeeklyBlocks
                                              : TimeConversionMode::DailyBlocks;
    for (const auto& comp: components | std::views::values)
    {
        addVariableEntries<LegacySolverTraits>(simulationTable,
                                               fillContext,
                                               comp,
                                               variableLookup,
                                               currentBlock,
                                               timeConversionMode,
                                               problemeHebdo->year,
                                               !solver->IsMIP()); /*assume we never do discrete pb*/

        addConstraintEntries<LegacySolverTraits>(simulationTable,
                                                 fillContext,
                                                 comp,
                                                 constraintLookup,
                                                 currentBlock,
                                                 timeConversionMode,
          problemeHebdo->year,
          !solver->IsMIP()); /*assume we never do discrete pb*/
    }
}

static SimplexResult OPT_TryToCallSimplex(const SingleOptimOptions& options,
                                          PROBLEME_HEBDO* problemeHebdo,
                                          const int NumIntervalle,
                                          const int optimizationNumber,
                                          const OptPeriodStringGenerator& optPeriodStringGenerator,
                                          IResultWriter& writer,
                                          ISimulationTable& simulationTable)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    auto* solver = ProblemeAResoudre->ProblemesSpx[NumIntervalle];

    const int opt = optimizationNumber - 1;
    assert(opt >= 0 && opt < 2);
    OptimizationStatistics& optimizationStatistics = problemeHebdo->optimizationStatistics[opt];
    TIME_MEASURE timeMeasure;

    ORTOOLS_LibererProbleme(solver);

    ProblemeAResoudre->ProblemesSpx[NumIntervalle] = nullptr;

    solver = convertToMPSolver(problemeHebdo, NumIntervalle, options, problemeHebdo->NamedProblems);

    const std::string filename = createMPSfilename(optPeriodStringGenerator, optimizationNumber);

    mpsWriterFactory mps_writer_factory(problemeHebdo->ExportMPS,
                                        problemeHebdo->exportMPSOnError,
                                        optimizationNumber,
                                        solver);

    auto mps_writer = mps_writer_factory.create();
    mps_writer->runIfNeeded(writer, filename);

    TimeMeasurement measure;
    solver = ORTOOLS_Simplexe(ProblemeAResoudre.get(), solver, options);
    if (solver != nullptr)
    {
        ProblemeAResoudre->ProblemesSpx[NumIntervalle] = solver;
    }

    measure.tick();
    timeMeasure.solveTime = measure.duration_ms();
    optimizationStatistics.addSolveTime(timeMeasure.solveTime);

    if (ProblemeAResoudre->ExistenceDUneSolution != OUI_SPX)
    {
        if (ProblemeAResoudre->ExistenceDUneSolution != SPX_ERREUR_INTERNE)
        {
            if (solver)
            {
                ORTOOLS_LibererProbleme(solver);

                ProblemeAResoudre->ProblemesSpx[NumIntervalle] = nullptr;

                solver = nullptr;
            }

            logs.info() << " Solver: resolution failed";
            logs.debug() << " solver: resetting";

            return {.success = false,
                    .timeMeasure = timeMeasure,
                    .mps_writer_factory = mps_writer_factory,
                    .objectiveValue = 0};
        }
        throw FatalError("Internal error: insufficient memory");
    }

    writeModelerSolutions(solver,
                          ProblemeAResoudre->NombreDeVariables,
                          optimizationNumber,
                          optPeriodStringGenerator,
                          writer);
    if (problemeHebdo->modelerSystem)
    {
        FillSimulationTable(simulationTable,
                            solver,
                            problemeHebdo,
                            buildFillContext(problemeHebdo, NumIntervalle),
                            NumIntervalle);
        // simulationTable.write();
    }

    return {.success = true,
            .timeMeasure = timeMeasure,
            .mps_writer_factory = mps_writer_factory,
            .objectiveValue = solver != nullptr ? getObjectiveValue(solver) : 0};
}

bool OPT_AppelDuSimplexe(const SingleOptimOptions& options,
                         PROBLEME_HEBDO* problemeHebdo,
                         int NumIntervalle,
                         const int optimizationNumber,
                         const OptPeriodStringGenerator& optPeriodStringGenerator,
                         IResultWriter& writer,
                         ISimulationTable& simulationTable)
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
        std::unique_ptr<MPSolver> MPproblem(
          convertToMPSolver(problemeHebdo, NumIntervalle, options, true));

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
