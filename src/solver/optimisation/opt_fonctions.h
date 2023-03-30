/*
** Copyright 2007-2023 RTE
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
#ifndef __SOLVER_OPTIMISATION_FUNCTIONS_H__
#define __SOLVER_OPTIMISATION_FUNCTIONS_H__

#include "../config.h"
#include "opt_structure_probleme_a_resoudre.h"
#include "../simulation/sim_structure_donnees.h"
#include "adequacy_patch_csr/hourly_csr_problem.h"
#include "opt_period_string_generator_base.h"
#include "antares/study/parameters/adq-patch-params.h"

using AdqPatchParams = Antares::Data::AdequacyPatch::AdqPatchParams;

void OPT_OptimisationHebdomadaire(PROBLEME_HEBDO*, AdqPatchParams&, uint);
void OPT_NumeroDeJourDuPasDeTemps(PROBLEME_HEBDO*);
void OPT_NumeroDIntervalleOptimiseDuPasDeTemps(PROBLEME_HEBDO*);
void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaire(PROBLEME_HEBDO*);
void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeQuadratique(PROBLEME_HEBDO*);
void OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaire(PROBLEME_HEBDO*);
void OPT_ConstruireLaMatriceDesContraintesDuProblemeQuadratique(PROBLEME_HEBDO*);
void OPT_InitialiserLesPminHebdo(PROBLEME_HEBDO*);
void OPT_InitialiserLesContrainteDEnergieHydrauliqueParIntervalleOptimise(PROBLEME_HEBDO*);
void OPT_MaxDesPmaxHydrauliques(PROBLEME_HEBDO*);
void OPT_InitialiserLesBornesDesVariablesDuProblemeLineaire(PROBLEME_HEBDO*,
                                                            AdqPatchParams&,
                                                            const int,
                                                            const int,
                                                            const int);
void OPT_InitialiserLesBornesDesVariablesDuProblemeQuadratique(PROBLEME_HEBDO*, int);
void OPT_InitialiserLeSecondMembreDuProblemeLineaire(PROBLEME_HEBDO*, int, int, int, const int);
void OPT_InitialiserLeSecondMembreDuProblemeQuadratique(PROBLEME_HEBDO*, int);
void OPT_InitialiserLesCoutsLineaire(PROBLEME_HEBDO*, const int, const int, uint);
void OPT_InitialiserLesCoutsQuadratiques(PROBLEME_HEBDO*, int);
void OPT_ControleDesPminPmaxThermiques(PROBLEME_HEBDO*);
bool OPT_AppelDuSolveurQuadratique(PROBLEME_ANTARES_A_RESOUDRE*, const int);


using namespace Antares::Data::AdequacyPatch;
bool ADQ_PATCH_CSR(PROBLEME_ANTARES_A_RESOUDRE&, HourlyCSRProblem&, const AdqPatchParams&, uint week, int year);

bool OPT_PilotageOptimisationLineaire(PROBLEME_HEBDO*, AdqPatchParams&, uint);
void OPT_VerifierPresenceReserveJmoins1(PROBLEME_HEBDO*);
bool OPT_PilotageOptimisationQuadratique(PROBLEME_HEBDO*);

/*!
** \brief Appel du solver
**
** \return True si l'operation s'est bien deroulee, false si le probleme n'a pas de solution
*/
bool OPT_AppelDuSimplexe(PROBLEME_HEBDO*,
                         int,
                         const int,
                         std::shared_ptr<OptPeriodStringGenerator>);
void OPT_LiberationProblemesSimplexe(const PROBLEME_HEBDO*);
bool OPT_OptimisationLineaire(PROBLEME_HEBDO*, AdqPatchParams&, uint);
void OPT_SauvegarderLesPmaxThermiques(PROBLEME_HEBDO*);
void OPT_RestaurerLesDonnees(const PROBLEME_HEBDO*, const int);
/*------------------------------*/

void OPT_CalculerLesPminThermiquesEnFonctionDeMUTetMDT(PROBLEME_HEBDO*);
double OPT_CalculerAireMaxPminJour(int, int, int, int, int*, int*);

void OPT_ChargerLaContrainteDansLaMatriceDesContraintes(PROBLEME_ANTARES_A_RESOUDRE*,
                                                        double*,
                                                        int*,
                                                        int,
                                                        char,
                                                        const std::string& NomDeLaContrainte = "");
void OPT_ChainagesDesIntercoPartantDUnNoeud(PROBLEME_HEBDO*);

void OPT_AllocateFromNumberOfVariableConstraints(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
                                                 int);
void OPT_FreeOptimizationData(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre);
void OPT_AllocDuProblemeAOptimiser(PROBLEME_HEBDO*);
int OPT_DecompteDesVariablesEtDesContraintesDuProblemeAOptimiser(PROBLEME_HEBDO*);
void OPT_AugmenterLaTailleDeLaMatriceDesContraintes(PROBLEME_ANTARES_A_RESOUDRE*);
void OPT_LiberationMemoireDuProblemeAOptimiser(PROBLEME_HEBDO*);

void OPT_EcrireResultatFonctionObjectiveAuFormatTXT(double,
                                                    std::shared_ptr<OptPeriodStringGenerator>,
                                                    int);

/*------------------------------*/

void OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaireCoutsDeDemarrage(PROBLEME_HEBDO*, bool);
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
double OPT_SommeDesPminThermiques(const PROBLEME_HEBDO*, int, int);

#endif /* __SOLVER_OPTIMISATION_FUNCTIONS_H__ */
