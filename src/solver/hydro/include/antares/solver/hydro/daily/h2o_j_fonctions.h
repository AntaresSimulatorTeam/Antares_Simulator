// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_H2O_J_FONCTIONS__
#define __SOLVER_H2O_J_FONCTIONS__

#include <string>
#include <vector>

#include "h2o_j_donnees_optimisation.h"

namespace DoneesOptimisationJournaliere
{
DONNEES_MENSUELLES H2O_J_Instanciation(void);

void H2O_J_OptimiserUnMois(DONNEES_MENSUELLES*, const std::string&);
void H2O_J_Free(DONNEES_MENSUELLES*);
void H2O_J_ConstruireLesContraintes(int,
                                    std::vector<int>&,
                                    std::vector<int>&,
                                    int,
                                    int,
                                    std::vector<int>&,
                                    std::vector<char>&,
                                    std::vector<int>&,
                                    std::vector<double>&,
                                    std::vector<int>&,
                                    CORRESPONDANCE_DES_CONTRAINTES&);
void H2O_j_ConstruireLesVariables(int,
                                  std::vector<int>&,
                                  std::vector<double>&,
                                  std::vector<double>&,
                                  std::vector<int>&,
                                  std::vector<double*>&,
                                  CORRESPONDANCE_DES_VARIABLES&);
void H2O_J_InitialiserLesBornesdesVariables(DONNEES_MENSUELLES*, int);
void H2O_J_InitialiserLeSecondMembre(DONNEES_MENSUELLES*, int);
void H2O_J_InitialiserLeSecondMembre(DONNEES_MENSUELLES*, int, const std::string&);
void H2O_J_ResoudreLeProblemeLineaire(DONNEES_MENSUELLES*, int);
void H2O_J_LisserLesSurTurbines(DONNEES_MENSUELLES*, int);
void H2O_J_AjouterBruitAuCout(DONNEES_MENSUELLES&);

// OR-Tools MPSolver helpers for the daily hydro problem.
// Build the OR-Tools variables used by the daily heuristic.
H2O_J_MPSOLVER_VARIABLES H2O_J_MPSolver_CreateVariables(DONNEES_MENSUELLES*,
                                                        int,
                                                        operations_research::MPSolver&);

// Objective builder: minimize the sum of xi[t] and the two
// global deviation variables (xiPlus, xiMoins).
void H2O_J_MPSolver_SetObjectiveCoefficients(
  const H2O_J_MPSOLVER_VARIABLES&,
  operations_research::MPSolver&);

// Build constraints directly from DONNEES_MENSUELLES and the typed
// variable pointers, without relying on internal matrix structures or
// variable indices.
void H2O_J_MPSolver_CreateConstraints(
  DONNEES_MENSUELLES*,
  const H2O_J_MPSOLVER_VARIABLES&,
  operations_research::MPSolver&);

void H2O_J_MPSolver_SolveAndRecover(DONNEES_MENSUELLES*,
                                    int,
                                    operations_research::MPSolver&,
                                    const H2O_J_MPSOLVER_VARIABLES&);

// Convenience function that builds an MPSolver instance and runs the
// full daily optimisation pipeline for a given problem index:
//  - create variables
//  - set objective coefficients
//  - create constraints
//  - solve and recover solution values into DONNEES_MENSUELLES.
void H2O_J_MPSolver_Solve(DONNEES_MENSUELLES*, int);
} // namespace DoneesOptimisationJournaliere

#endif /* __SOLVER_H2O_J_FONCTIONS__ */
