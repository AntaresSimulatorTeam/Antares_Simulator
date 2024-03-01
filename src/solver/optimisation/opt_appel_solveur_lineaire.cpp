/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <yuni/yuni.h>
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "antares/solver/simulation/simulation.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/optimisation/opt_fonctions.h"

extern "C"
{
#include "spx_definition_arguments.h"
#include "spx_fonctions.h"

#include "srs_api.h"
}

#include <antares/logs/logs.h>
#include <antares/antares/fatal-error.h>

#include "antares/solver/utils/mps_utils.h"
#include "antares/solver/utils/filename.h"

#include "../infeasible-problem-analysis/unfeasible-pb-analyzer.h"
#include "../infeasible-problem-analysis/variables-bounds-consistency.h"
#include "../infeasible-problem-analysis/constraint-slack-analysis.h"

#include <chrono>

using namespace operations_research;

using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;
using Antares::Solver::IResultWriter;

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
};

void dumpSPXData(const PROBLEME_SIMPLEXE& Probleme,
		 const OptPeriodStringGenerator& optPeriodStringGenerator,
		 IResultWriter& writer,
		 unsigned int optNumber,
		 std::size_t nbTerms)
{
  auto dump = [&optNumber, &optPeriodStringGenerator, &writer](std::string name, auto* tab, std::size_t sz)
  {
    const auto filename = createOptimizationFilename(name,
						     optPeriodStringGenerator,
						     optNumber,
						     "txt");
    std::ostringstream os;
    for (size_t ii = 0; ii < sz; ii++)
    {
	os << tab[ii] << "\n";
    }
    auto buffer = os.str();
    writer.addEntryFromBuffer(filename, buffer);
  };

    {
      const auto nbVar = Probleme.NombreDeVariables;
      dump("CoutLineaire", Probleme.CoutLineaire, nbVar);
      dump("Xmin", Probleme.Xmin, nbVar);
      dump("Xmax", Probleme.Xmax, nbVar);
      dump("TypeDeVariable", Probleme.TypeDeVariable, nbVar);
    }

    {
      const auto nbConstraints = Probleme.NombreDeContraintes;
      dump("IndicesDebutDeLigne", Probleme.IndicesDebutDeLigne, nbConstraints);
      dump("NombreDeTermesDesLignes", Probleme.NombreDeTermesDesLignes, nbConstraints);
      dump("Sens", Probleme.Sens, nbConstraints);
      dump("SecondMembre", Probleme.SecondMembre, nbConstraints);
    }

    {
      dump("IndicesColonnes", Probleme.IndicesColonnes, nbTerms);
      dump("CoefficientsDeLaMatriceDesContraintes", Probleme.CoefficientsDeLaMatriceDesContraintes, nbTerms);
    }
}

static SimplexResult OPT_TryToCallSimplex(
        const OptimizationOptions& options,
        PROBLEME_HEBDO* problemeHebdo,
        Optimization::PROBLEME_SIMPLEXE_NOMME& Probleme,
        const int NumIntervalle,
        const int optimizationNumber,
        const OptPeriodStringGenerator& optPeriodStringGenerator,
        bool PremierPassage,
        IResultWriter& writer)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    const auto nbTerms = ProblemeAResoudre->NombreDeTermesAllouesDansLaMatriceDesContraintes;
    auto ProbSpx
      = (PROBLEME_SPX*)(ProblemeAResoudre->ProblemesSpx[(int)NumIntervalle]);
    auto solver = (MPSolver*)(ProblemeAResoudre->ProblemesSpx[(int)NumIntervalle]);

    const int opt = optimizationNumber - 1;
    assert(opt >= 0 && opt < 2);
    OptimizationStatistics& optimizationStatistics = problemeHebdo->optimizationStatistics[opt];
    TIME_MEASURE timeMeasure;
    if (!PremierPassage)
    {
        ProbSpx = nullptr;
        solver = nullptr;
    }

    if (ProbSpx == nullptr && solver == nullptr)
    {
        Probleme.Contexte = SIMPLEXE_SEUL;
        Probleme.BaseDeDepartFournie = NON_SPX;
    }
    else
    {
        if (problemeHebdo->ReinitOptimisation)
        {
            if (options.useOrtools && solver != nullptr)
            {
                ORTOOLS_LibererProbleme(solver);
            }
            else if (ProbSpx != nullptr)
            {
                SPX_LibererProbleme(ProbSpx);
            }
            ProblemeAResoudre->ProblemesSpx[NumIntervalle] = nullptr;

            ProbSpx = nullptr;
            solver = nullptr;
            Probleme.Contexte = SIMPLEXE_SEUL;
            Probleme.BaseDeDepartFournie = NON_SPX;
        }
        else
        {
            Probleme.Contexte = BRANCH_AND_BOUND_OU_CUT_NOEUD;
            Probleme.BaseDeDepartFournie = UTILISER_LA_BASE_DU_PROBLEME_SPX;

            TimeMeasurement updateMeasure;
            if (options.useOrtools)
            {
                ORTOOLS_ModifierLeVecteurCouts(
                  solver, ProblemeAResoudre->CoutLineaire.data(), ProblemeAResoudre->NombreDeVariables);
                ORTOOLS_ModifierLeVecteurSecondMembre(solver,
                                                      ProblemeAResoudre->SecondMembre.data(),
                                                      ProblemeAResoudre->Sens.data(),
                                                      ProblemeAResoudre->NombreDeContraintes);
                ORTOOLS_CorrigerLesBornes(solver,
                                          ProblemeAResoudre->Xmin.data(),
                                          ProblemeAResoudre->Xmax.data(),
                                          ProblemeAResoudre->TypeDeVariable.data(),
                                          ProblemeAResoudre->NombreDeVariables);
            }
            else
            {
                SPX_ModifierLeVecteurCouts(
                  ProbSpx, ProblemeAResoudre->CoutLineaire.data(), ProblemeAResoudre->NombreDeVariables);
                SPX_ModifierLeVecteurSecondMembre(ProbSpx,
                                                  ProblemeAResoudre->SecondMembre.data(),
                                                  ProblemeAResoudre->Sens.data(),
                                                  ProblemeAResoudre->NombreDeContraintes);
            }
            updateMeasure.tick();
            timeMeasure.updateTime = updateMeasure.duration_ms();
            optimizationStatistics.addUpdateTime(timeMeasure.updateTime);
        }
    }

    Probleme.NombreMaxDIterations = -1;
    Probleme.DureeMaxDuCalcul = -1.;

    Probleme.CoutLineaire = ProblemeAResoudre->CoutLineaire.data();
    Probleme.X = ProblemeAResoudre->X.data();
    Probleme.Xmin = ProblemeAResoudre->Xmin.data();
    Probleme.Xmax = ProblemeAResoudre->Xmax.data();
    Probleme.NombreDeVariables = ProblemeAResoudre->NombreDeVariables;
    Probleme.TypeDeVariable = ProblemeAResoudre->TypeDeVariable.data();

    Probleme.NombreDeContraintes = ProblemeAResoudre->NombreDeContraintes;
    Probleme.IndicesDebutDeLigne = ProblemeAResoudre->IndicesDebutDeLigne.data();
    Probleme.NombreDeTermesDesLignes = ProblemeAResoudre->NombreDeTermesDesLignes.data();
    Probleme.IndicesColonnes = ProblemeAResoudre->IndicesColonnes.data();
    Probleme.CoefficientsDeLaMatriceDesContraintes
      = ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes.data();
    Probleme.Sens = ProblemeAResoudre->Sens.data();
    Probleme.SecondMembre = ProblemeAResoudre->SecondMembre.data();

    Probleme.ChoixDeLAlgorithme = SPX_DUAL;

    Probleme.TypeDePricing = PRICING_STEEPEST_EDGE;

    Probleme.FaireDuScaling = ( PremierPassage ? OUI_SPX : NON_SPX );

    Probleme.StrategieAntiDegenerescence = AGRESSIF;

    Probleme.PositionDeLaVariable = ProblemeAResoudre->PositionDeLaVariable.data();
    Probleme.NbVarDeBaseComplementaires = 0;
    Probleme.ComplementDeLaBase = ProblemeAResoudre->ComplementDeLaBase.data();

    Probleme.LibererMemoireALaFin = NON_SPX;

    Probleme.UtiliserCoutMax = NON_SPX;
    Probleme.CoutMax = 0.0;

    Probleme.CoutsMarginauxDesContraintes = ProblemeAResoudre->CoutsMarginauxDesContraintes.data();
    Probleme.CoutsReduits = ProblemeAResoudre->CoutsReduits.data();

    Probleme.NombreDeContraintesCoupes = 0;

    if (options.useOrtools)
    {
        solver = ORTOOLS_ConvertIfNeeded(options.solverName, &Probleme, solver);
    }
    const std::string filename = createMPSfilename(optPeriodStringGenerator, optimizationNumber);

    mpsWriterFactory mps_writer_factory(problemeHebdo->ExportMPS,
                                        problemeHebdo->exportMPSOnError,
                                        optimizationNumber,
                                        &Probleme,
                                        options.useOrtools,
                                        solver);

    auto mps_writer = mps_writer_factory.create();
    mps_writer->runIfNeeded(writer, filename);

    TimeMeasurement measure;
    if (options.useOrtools)
    {
        const bool keepBasis = (optimizationNumber == PREMIERE_OPTIMISATION);
	{
	  auto* srs = static_cast<SRS_PROBLEM*>(solver->underlying_solver());
	  PROBLEME_SIMPLEXE* spx = srs->problem_simplexe;
	  dumpSPXData(*spx,
		      optPeriodStringGenerator,
		      writer,
		      optimizationNumber,
		      nbTerms);
	}

        solver = ORTOOLS_Simplexe(&Probleme, solver, keepBasis);
        if (solver != nullptr)
        {
            ProblemeAResoudre->ProblemesSpx[NumIntervalle] = (void*)solver;
        }
    }
    else
    {
        dumpSPXData(Probleme,
		    optPeriodStringGenerator,
		    writer,
		    optimizationNumber,
		    nbTerms);
        ProbSpx = SPX_Simplexe(&Probleme, ProbSpx);
        if (ProbSpx != nullptr)
        {
            ProblemeAResoudre->ProblemesSpx[NumIntervalle] = (void*)ProbSpx;
        }
    }
    measure.tick();
    timeMeasure.solveTime = measure.duration_ms();
    optimizationStatistics.addSolveTime(timeMeasure.solveTime);

    ProblemeAResoudre->ExistenceDUneSolution = Probleme.ExistenceDUneSolution;
    if (ProblemeAResoudre->ExistenceDUneSolution != OUI_SPX && PremierPassage)
    {
        if (ProblemeAResoudre->ExistenceDUneSolution != SPX_ERREUR_INTERNE)
        {
            if (options.useOrtools && solver != nullptr)
            {
                ORTOOLS_LibererProbleme(solver);
            }
            else if (ProbSpx != nullptr)
            {
                SPX_LibererProbleme(ProbSpx);
            }

            logs.info() << " Solver: Standard resolution failed";
            logs.info() << " Solver: Retry in safe mode"; // second trial w/o scaling

            if (Logs::Verbosity::Debug::enabled)
            {
                logs.info() << " solver: resetting";
            }
            return {.success=false, .timeMeasure=timeMeasure,
                    .mps_writer_factory=mps_writer_factory};
        }

        else
        {
            throw FatalError("Internal error: insufficient memory");
        }
    }
    return {.success=true, .timeMeasure=timeMeasure,
            .mps_writer_factory=mps_writer_factory};
}

bool OPT_AppelDuSimplexe(const OptimizationOptions& options,
                         PROBLEME_HEBDO* problemeHebdo,
                         int NumIntervalle,
                         const int optimizationNumber,
                         const OptPeriodStringGenerator& optPeriodStringGenerator,
                         IResultWriter& writer)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    Optimization::PROBLEME_SIMPLEXE_NOMME Probleme(ProblemeAResoudre->NomDesVariables,
                                                   ProblemeAResoudre->NomDesContraintes,
                                                   ProblemeAResoudre->VariablesEntieres,
                                                   ProblemeAResoudre->StatutDesVariables,
                                                   ProblemeAResoudre->StatutDesContraintes,
                                                   problemeHebdo->NamedProblems,
                                                   problemeHebdo->solverLogs);

    bool PremierPassage = true;

    SimplexResult simplexResult =
        OPT_TryToCallSimplex(options, problemeHebdo, Probleme, NumIntervalle, optimizationNumber,
                optPeriodStringGenerator, PremierPassage, writer);

    if (!simplexResult.success)
    {
        PremierPassage = false;
        simplexResult = OPT_TryToCallSimplex(options, problemeHebdo, Probleme,  NumIntervalle, optimizationNumber,
                optPeriodStringGenerator, PremierPassage, writer);
    }

    if (ProblemeAResoudre->ExistenceDUneSolution == OUI_SPX)
    {
        if (!PremierPassage)
        {
            logs.info() << " Solver: Safe resolution succeeded";
        }

        double* pt;
        double CoutOpt = 0.0;

        for (int i = 0; i < ProblemeAResoudre->NombreDeVariables; i++)
        {
            CoutOpt += ProblemeAResoudre->CoutLineaire[i] * ProblemeAResoudre->X[i];

            pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[i];
            if (pt != nullptr)
                *pt = ProblemeAResoudre->X[i];

            pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits[i];
            if (pt != nullptr)
                *pt = ProblemeAResoudre->CoutsReduits[i];
        }

        {
            const int opt = optimizationNumber - 1;
            assert(opt >= 0 && opt < 2);
            problemeHebdo->timeMeasure[opt] = simplexResult.timeMeasure;
        }

        // TODO remove this if..else
        if (optimizationNumber == PREMIERE_OPTIMISATION)
        {
            problemeHebdo->coutOptimalSolution1[NumIntervalle] = CoutOpt;
        }
        else
        {
            problemeHebdo->coutOptimalSolution2[NumIntervalle] = CoutOpt;
        }
        for (int Cnt = 0; Cnt < ProblemeAResoudre->NombreDeContraintes; Cnt++)
        {
            pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt];
            if (pt != nullptr)
                *pt = ProblemeAResoudre->CoutsMarginauxDesContraintes[Cnt];
        }
    }

    else
    {
        if (!PremierPassage)
        {
            logs.info() << " Solver: Safe resolution failed";
        }

        Probleme.SetUseNamedProblems(true);

        auto MPproblem = std::shared_ptr<MPSolver>(ProblemSimplexeNommeConverter(options.solverName, &Probleme).Convert());

        auto analyzer = makeUnfeasiblePbAnalyzer();
        analyzer->run(MPproblem.get());
        analyzer->printReport();

        auto mps_writer_on_error = simplexResult.mps_writer_factory.createOnOptimizationError();
        const std::string filename = createMPSfilename(optPeriodStringGenerator, optimizationNumber);
        mps_writer_on_error->runIfNeeded(writer, filename);

        return false;
    }

    return true;
}
