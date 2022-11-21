/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
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

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_structure_probleme_economique.h"
#include "../simulation/sim_structure_probleme_adequation.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"

extern "C"
{
#include "spx_definition_arguments.h"
#include "spx_fonctions.h"

#include "srs_api.h"
}

#include <antares/logs.h>
#include <antares/study.h>
#include <antares/emergency.h>

#include "../utils/mps_utils.h"
#include "../utils/ortools_utils.h"
#include "../utils/filename.h"

#include "../infeasible-problem-analysis/problem.h"
#include "../infeasible-problem-analysis/exceptions.h"

#include <chrono>

using namespace operations_research;

using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;

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

    long long duration_ms() const
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

bool OPT_AppelDuSimplexe(PROBLEME_HEBDO* ProblemeHebdo, uint numSpace, int NumIntervalle)
{
    int Var;
    int Cnt;
    double* pt;
    char PremierPassage;
    double CoutOpt;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;

    PROBLEME_SPX* ProbSpx;
    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    Optimization::PROBLEME_SIMPLEXE_NOMME Probleme(ProblemeAResoudre->NomDesVariables,
                                                   ProblemeAResoudre->NomDesContraintes);
    PremierPassage = OUI_ANTARES;
    MPSolver* solver;

    ProbSpx = (PROBLEME_SPX*)(ProblemeAResoudre->ProblemesSpx->ProblemeSpx[(int)NumIntervalle]);
    solver = (MPSolver*)(ProblemeAResoudre->ProblemesSpx->ProblemeSpx[(int)NumIntervalle]);

    auto study = Data::Study::Current::Get();
    bool ortoolsUsed = study->parameters.ortoolsUsed;

RESOLUTION:

    if (ProbSpx == nullptr && solver == nullptr)
    {
        Probleme.Contexte = SIMPLEXE_SEUL;
        Probleme.BaseDeDepartFournie = NON_SPX;
    }
    else
    {
        if (ProblemeHebdo->ReinitOptimisation == OUI_ANTARES)
        {
            if (ortoolsUsed && solver != nullptr)
            {
                ORTOOLS_LibererProbleme(solver);
            }
            else if (ProbSpx != nullptr)
            {
                SPX_LibererProbleme(ProbSpx);
            }
            ProblemeAResoudre->ProblemesSpx->ProblemeSpx[NumIntervalle] = nullptr;

            ProbSpx = nullptr;
            solver = nullptr;
            Probleme.Contexte = SIMPLEXE_SEUL;
            Probleme.BaseDeDepartFournie = NON_SPX;
        }
        else
        {
            Probleme.Contexte = BRANCH_AND_BOUND_OU_CUT_NOEUD;
            Probleme.BaseDeDepartFournie = UTILISER_LA_BASE_DU_PROBLEME_SPX;

            TimeMeasurement measure;
            if (ortoolsUsed)
            {
                ORTOOLS_ModifierLeVecteurCouts(
                  solver, ProblemeAResoudre->CoutLineaire, ProblemeAResoudre->NombreDeVariables);
                ORTOOLS_ModifierLeVecteurSecondMembre(solver,
                                                      ProblemeAResoudre->SecondMembre,
                                                      ProblemeAResoudre->Sens,
                                                      ProblemeAResoudre->NombreDeContraintes);
                ORTOOLS_CorrigerLesBornes(solver,
                                          ProblemeAResoudre->Xmin,
                                          ProblemeAResoudre->Xmax,
                                          ProblemeAResoudre->TypeDeVariable,
                                          ProblemeAResoudre->NombreDeVariables);
            }
            else
            {
                SPX_ModifierLeVecteurCouts(
                  ProbSpx, ProblemeAResoudre->CoutLineaire, ProblemeAResoudre->NombreDeVariables);
                SPX_ModifierLeVecteurSecondMembre(ProbSpx,
                                                  ProblemeAResoudre->SecondMembre,
                                                  ProblemeAResoudre->Sens,
                                                  ProblemeAResoudre->NombreDeContraintes);
            }
            measure.tick();
            ProblemeHebdo->optimizationStatistics_object.addUpdateTime(measure.duration_ms());
        }
    }

    Probleme.NombreMaxDIterations = -1;
    Probleme.DureeMaxDuCalcul = -1.;

    Probleme.CoutLineaire = ProblemeAResoudre->CoutLineaire;
    Probleme.X = ProblemeAResoudre->X;
    Probleme.Xmin = ProblemeAResoudre->Xmin;
    Probleme.Xmax = ProblemeAResoudre->Xmax;
    Probleme.NombreDeVariables = ProblemeAResoudre->NombreDeVariables;
    Probleme.TypeDeVariable = ProblemeAResoudre->TypeDeVariable;

    Probleme.NombreDeContraintes = ProblemeAResoudre->NombreDeContraintes;
    Probleme.IndicesDebutDeLigne = ProblemeAResoudre->IndicesDebutDeLigne;
    Probleme.NombreDeTermesDesLignes = ProblemeAResoudre->NombreDeTermesDesLignes;
    Probleme.IndicesColonnes = ProblemeAResoudre->IndicesColonnes;
    Probleme.CoefficientsDeLaMatriceDesContraintes
      = ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes;
    Probleme.Sens = ProblemeAResoudre->Sens;
    Probleme.SecondMembre = ProblemeAResoudre->SecondMembre;

    Probleme.ChoixDeLAlgorithme = SPX_DUAL;

    Probleme.TypeDePricing = PRICING_STEEPEST_EDGE;

    if (PremierPassage == NON_ANTARES)
        Probleme.FaireDuScaling = NON_SPX;
    if (PremierPassage == OUI_ANTARES)
        Probleme.FaireDuScaling = OUI_SPX;

    Probleme.StrategieAntiDegenerescence = AGRESSIF;

    Probleme.PositionDeLaVariable = ProblemeAResoudre->PositionDeLaVariable;
    Probleme.NbVarDeBaseComplementaires = 0;
    Probleme.ComplementDeLaBase = ProblemeAResoudre->ComplementDeLaBase;

    Probleme.LibererMemoireALaFin = NON_SPX;

    Probleme.UtiliserCoutMax = NON_SPX;
    Probleme.CoutMax = 0.0;

    Probleme.CoutsMarginauxDesContraintes = ProblemeAResoudre->CoutsMarginauxDesContraintes;
    Probleme.CoutsReduits = ProblemeAResoudre->CoutsReduits;

#ifndef NDEBUG
    if (PremierPassage == OUI_ANTARES)
        Probleme.AffichageDesTraces = NON_SPX;
    else
        Probleme.AffichageDesTraces = OUI_SPX;
#else
    Probleme.AffichageDesTraces = NON_SPX;
#endif

    Probleme.NombreDeContraintesCoupes = 0;

    if (ortoolsUsed)
    {
        solver = ORTOOLS_ConvertIfNeeded(&Probleme, solver);
    }

    mpsWriterFactory mps_writer_factory(
      ProblemeHebdo, NumIntervalle, &Probleme, ortoolsUsed, solver, numSpace);
    auto mps_writer = mps_writer_factory.create();
    mps_writer->runIfNeeded(study->resultWriter);

    TimeMeasurement measure;
    if (ortoolsUsed)
    {
        solver = ORTOOLS_Simplexe(&Probleme, solver);
        if (solver != nullptr)
        {
            ProblemeAResoudre->ProblemesSpx->ProblemeSpx[NumIntervalle] = (void*)solver;
        }
    }
    else
    {
        ProbSpx = SPX_Simplexe(&Probleme, ProbSpx);
        if (ProbSpx != nullptr)
        {
            ProblemeAResoudre->ProblemesSpx->ProblemeSpx[NumIntervalle] = (void*)ProbSpx;
        }
    }
    measure.tick();
    ProblemeHebdo->optimizationStatistics_object.addSolveTime(measure.duration_ms());

    ProblemeAResoudre->ExistenceDUneSolution = Probleme.ExistenceDUneSolution;
    if (ProblemeAResoudre->ExistenceDUneSolution != OUI_SPX && PremierPassage == OUI_ANTARES)
    {
        if (ProblemeAResoudre->ExistenceDUneSolution != SPX_ERREUR_INTERNE)
        {
            if (ortoolsUsed && solver != nullptr)
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
            ProbSpx = nullptr;
            solver = nullptr;
            PremierPassage = NON_ANTARES;
            goto RESOLUTION;
        }

        else
        {
            logs.info();
            logs.error() << "Internal error: insufficient memory";
            logs.info();
            AntaresSolverEmergencyShutdown();
            return false;
        }
    }

    if (ProblemeAResoudre->ExistenceDUneSolution == OUI_SPX)
    {
        if (PremierPassage == NON_ANTARES)
        {
            logs.info() << " Solver: Safe resolution succeeded";
        }
        CoutOpt = 0.0;

        for (Var = 0; Var < ProblemeAResoudre->NombreDeVariables; Var++)
        {
            CoutOpt += ProblemeAResoudre->CoutLineaire[Var] * ProblemeAResoudre->X[Var];

            pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var];
            if (pt != nullptr)
                *pt = ProblemeAResoudre->X[Var];
            pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits[Var];
            if (pt != nullptr)
                *pt = ProblemeAResoudre->CoutsReduits[Var];
        }

        if (ProblemeHebdo->numeroOptimisation[NumIntervalle] == PREMIERE_OPTIMISATION)
            ProblemeHebdo->coutOptimalSolution1[NumIntervalle] = CoutOpt;
        else
            ProblemeHebdo->coutOptimalSolution2[NumIntervalle] = CoutOpt;

        for (Cnt = 0; Cnt < ProblemeAResoudre->NombreDeContraintes; Cnt++)
        {
            pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux[Cnt];
            if (pt != nullptr)
                *pt = ProblemeAResoudre->CoutsMarginauxDesContraintes[Cnt];
        }
    }

    else
    {
        if (PremierPassage == NON_ANTARES)
        {
            logs.info() << " Solver: Safe resolution failed";
        }

        Optimization::InfeasibleProblemAnalysis analysis(&Probleme);
        logs.notice() << " Solver: Starting infeasibility analysis...";
        try
        {
            Optimization::InfeasibleProblemReport report = analysis.produceReport();
            report.prettyPrint();
        }
        catch (const Optimization::SlackVariablesEmpty& ex)
        {
            logs.error() << ex.what();
        }
        catch (const Optimization::ProblemResolutionFailed& ex)
        {
            logs.error() << ex.what();
        }

        auto mps_writer_on_error = mps_writer_factory.createOnOptimizationError();
        mps_writer_on_error->runIfNeeded(study->resultWriter);

        return false;
    }

    return true;
}

void OPT_EcrireResultatFonctionObjectiveAuFormatTXT(void* Prob,
                                                    uint numSpace,
                                                    int NumeroDeLIntervalle)
{
    Yuni::Clob buffer;
    double CoutOptimalDeLaSolution;
    PROBLEME_HEBDO* Probleme;

    Probleme = (PROBLEME_HEBDO*)Prob;

    CoutOptimalDeLaSolution = 0.;
    if (Probleme->numeroOptimisation[NumeroDeLIntervalle] == PREMIERE_OPTIMISATION)
        CoutOptimalDeLaSolution = Probleme->coutOptimalSolution1[NumeroDeLIntervalle];
    else
        CoutOptimalDeLaSolution = Probleme->coutOptimalSolution2[NumeroDeLIntervalle];

    buffer.appendFormat("* Optimal criterion value :   %11.10e\n", CoutOptimalDeLaSolution);

    auto study = Data::Study::Current::Get();
    auto optNumber = Probleme->numeroOptimisation[NumeroDeLIntervalle];
    auto filename = getFilenameWithExtension("criterion", "txt", numSpace, optNumber);
    auto writer = study->resultWriter;
    writer->addEntryFromBuffer(filename, buffer);
}
