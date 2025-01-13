/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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
#ifndef __SOLVER_OPTIMISATION_FUNCTIONS_H__
#define __SOLVER_OPTIMISATION_FUNCTIONS_H__

#include <antares/optimization-options/options.h>
#include <antares/solver/utils/opt_period_string_generator.h>
#include <antares/writer/i_writer.h>
#include "antares/config/config.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/ISimulationObserver.h"
#include "antares/study/parameters/adq-patch-params.h"

#include "adequacy_patch_csr/hourly_csr_problem.h"

using AdqPatchParams = Antares::Data::AdequacyPatch::AdqPatchParams;
using OptimizationOptions = Antares::Solver::Optimization::OptimizationOptions;

void OPT_OptimisationHebdomadaire(const OptimizationOptions& options,
                                  PROBLEME_HEBDO* pProblemeHebdo,
                                  Solver::IResultWriter& writer,
                                  Solver::Simulation::ISimulationObserver& simulationObserver);
void OPT_NumeroDeJourDuPasDeTemps(PROBLEME_HEBDO*);
void OPT_NumeroDIntervalleOptimiseDuPasDeTemps(PROBLEME_HEBDO*);
void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaire(PROBLEME_HEBDO*);
void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeQuadratique(PROBLEME_HEBDO*);
void OPT_InitialiserLesPminHebdo(PROBLEME_HEBDO*);
void OPT_InitialiserLesContrainteDEnergieHydrauliqueParIntervalleOptimise(PROBLEME_HEBDO*);
void OPT_MaxDesPmaxHydrauliques(PROBLEME_HEBDO*);
void OPT_InitialiserLesBornesDesVariablesDuProblemeLineaire(PROBLEME_HEBDO*,
                                                            const int,
                                                            const int,
                                                            const int);
void OPT_InitialiserLesBornesDesVariablesDuProblemeQuadratique(PROBLEME_HEBDO*, int);
void OPT_InitialiserLeSecondMembreDuProblemeLineaire(PROBLEME_HEBDO*, int, int, int, const int);
void OPT_InitialiserLeSecondMembreDuProblemeQuadratique(PROBLEME_HEBDO*, int);
void OPT_InitialiserLesCoutsLineaire(PROBLEME_HEBDO*, const int, const int);
void OPT_InitialiserLesCoutsQuadratiques(PROBLEME_HEBDO*, int);
bool OPT_AppelDuSolveurQuadratique(PROBLEME_ANTARES_A_RESOUDRE*, const int);

bool OPT_PilotageOptimisationLineaire(const OptimizationOptions& options,
                                      PROBLEME_HEBDO* problemeHebdo,
                                      Solver::IResultWriter& writer,
                                      Solver::Simulation::ISimulationObserver& simulationObserver);
void OPT_VerifierPresenceReserveJmoins1(PROBLEME_HEBDO*);
bool OPT_PilotageOptimisationQuadratique(PROBLEME_HEBDO*);

/*!
** \brief Appel du solver
**
** \return True si l'operation s'est bien deroulee, false si le probleme n'a pas de solution
*/
bool OPT_AppelDuSimplexe(const OptimizationOptions& options,
                         PROBLEME_HEBDO*,
                         int,
                         const int,
                         const OptPeriodStringGenerator&,
                         Antares::Solver::IResultWriter& writer);
void OPT_LiberationProblemesSimplexe(const PROBLEME_HEBDO*);

bool OPT_OptimisationLineaire(const OptimizationOptions& options,
                              PROBLEME_HEBDO* problemeHebdo,
                              Solver::IResultWriter& writer,
                              Solver::Simulation::ISimulationObserver& simulationObserver);
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

#endif /* __SOLVER_OPTIMISATION_FUNCTIONS_H__ */
