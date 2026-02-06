// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_OPTIMISATION_FUNCTIONS_H__
#define __SOLVER_OPTIMISATION_FUNCTIONS_H__

#include <antares/optimization-options/options.h>
#include <antares/solver/utils/opt_period_string_generator.h>
#include <antares/writer/i_writer.h>
#include "antares/config/config.h"
#include "antares/io/outputs/SimulationTableCsv.h"
#include "antares/solver/optim-model-filler/BendersDecomposition.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/ISimulationObserver.h"
#include "antares/study/parameters/adq-patch-params.h"

#include "adequacy_patch_csr/hourly_csr_problem.h"

namespace Antares::Optimisation
{
class OptimEntityContainer;
}

using AdqPatchParams = Antares::Data::AdequacyPatch::AdqPatchParams;
using OptimizationOptions = Antares::Solver::Optimization::OptimizationOptions;
using SingleOptimOptions = Antares::Solver::Optimization::SingleOptimOptions;

namespace Antares::IO::Outputs
{
class ISimulationTable;
}
class OptimisationsSimulationTable;
void OPT_OptimisationHebdomadaireLineaire(
  const OptimizationOptions& options,
  PROBLEME_HEBDO* pProblemeHebdo,
  Solver::IResultWriter& writer,
  Solver::Simulation::ISimulationObserver& simulationObserver,
  OptimisationsSimulationTable* simulationTables);
void OPT_OptimisationHebdomadaireQuadratique(const OptimizationOptions& options,
                                             PROBLEME_HEBDO* pProblemeHebdo);

void OPT_NumeroDeJourDuPasDeTemps(PROBLEME_HEBDO*);
void OPT_NumeroDIntervalleOptimiseDuPasDeTemps(PROBLEME_HEBDO*);
void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaire(PROBLEME_HEBDO*);
void OPT_InitialiserLesPminHebdo(PROBLEME_HEBDO*);
void OPT_InitialiserLesContrainteDEnergieHydrauliqueParIntervalleOptimise(PROBLEME_HEBDO*);
void OPT_MaxDesPmaxHydrauliques(PROBLEME_HEBDO*);
void OPT_InitialiserLesBornesDesVariablesDuProblemeLineaire(PROBLEME_HEBDO*,
                                                            const int,
                                                            const int,
                                                            const int);
void OPT_InitialiserLeSecondMembreDuProblemeLineaire(PROBLEME_HEBDO*, int, int, int, const int);
void OPT_InitialiserLesCoutsLineaire(PROBLEME_HEBDO*, const int, const int);

bool OPT_PilotageOptimisationLineaire(const OptimizationOptions& options,
                                      PROBLEME_HEBDO* problemeHebdo,
                                      Solver::IResultWriter& writer,
                                      Solver::Simulation::ISimulationObserver& simulationObserver,
                                      OptimisationsSimulationTable* simulationTables);
void OPT_VerifierPresenceReserveJmoins1(PROBLEME_HEBDO*);

/*!
** \brief Appel du solver
**
** \return True si l'operation s'est bien deroulee, false si le probleme n'a pas de solution
*/
bool OPT_AppelDuSimplexe(const SingleOptimOptions& options,
                         PROBLEME_HEBDO*,
                         int,
                         const int,
                         const OptPeriodStringGenerator&,
                         Antares::Solver::IResultWriter& writer,
                         Antares::IO::Outputs::ISimulationTable* simulationTable);

bool OPT_OptimisationLineaire(const OptimizationOptions& options,
                              PROBLEME_HEBDO* problemeHebdo,
                              Solver::IResultWriter& writer,
                              Solver::Simulation::ISimulationObserver& simulationObserver,
                              OptimisationsSimulationTable* simulationTables);
void OPT_RestaurerLesDonnees(PROBLEME_HEBDO*);
/*------------------------------*/

void OPT_CalculerLesPminThermiquesEnFonctionDeMUTetMDT(PROBLEME_HEBDO*);
double OPT_CalculerAireMaxPminJour(int, int, int, int, std::vector<int>&, std::vector<int>&);

void OPT_ChainagesDesIntercoPartantDUnNoeud(PROBLEME_HEBDO*);

void OPT_AllocateFromNumberOfVariableConstraints(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre);
void OPT_AllocDuProblemeAOptimiser(PROBLEME_HEBDO*);
int OPT_DecompteDesVariablesEtDesContraintesDuProblemeAOptimiser(PROBLEME_HEBDO*);

/*------------------------------*/

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireCoutsDeDemarrage(PROBLEME_HEBDO*,
                                                                                   bool);
void OPT_InitialiserLesBornesDesVariablesDuProblemeLineaireCoutsDeDemarrage(PROBLEME_HEBDO*,
                                                                            const int,
                                                                            const int);
void OPT_InitialiserLesCoutsLineaireCoutsDeDemarrage(PROBLEME_HEBDO*, const int, const int);
void OPT_InitialiserLeSecondMembreDuProblemeLineaireCoutsDeDemarrage(PROBLEME_HEBDO*, int, int);
void OPT_DecompteDesVariablesEtDesContraintesCoutsDeDemarrage(PROBLEME_HEBDO*);
void OPT_InitialiserNombreMinEtMaxDeGroupesCoutsDeDemarrage(PROBLEME_HEBDO*);
void OPT_AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage(PROBLEME_HEBDO*);
double OPT_SommeDesPminThermiques(const PROBLEME_HEBDO*, int, uint);
Optimisation::LinearProblemApi::FillContext buildFillContext(const PROBLEME_HEBDO* problemeHebdo,
                                                             int NumIntervalle);
void fillLinearProblem(Optimisation::LinearProblemApi::FillContext& fillCtx,
                       const PROBLEME_HEBDO* problemeHebdo,
                       Optimisation::OptimEntityContainer& optimEntityContainer,
                       bool namedProblems,
                       Optimisation::BendersDecomposition* bendersDecomposition = nullptr);
#endif /* __SOLVER_OPTIMISATION_FUNCTIONS_H__ */
