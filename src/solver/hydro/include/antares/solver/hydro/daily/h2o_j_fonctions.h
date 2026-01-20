// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_H2O_J_FONCTIONS__
#define __SOLVER_H2O_J_FONCTIONS__

namespace operations_research
{
class MPSolver;
class MPVariable;
} // namespace operations_research

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
void H2O_J_MPSolver_CreateVariables(DONNEES_MENSUELLES*,
                                    int,
                                    operations_research::MPSolver&,
                                    std::vector<operations_research::MPVariable*>&);

void H2O_J_MPSolver_SetObjectiveCoefficients(DONNEES_MENSUELLES*,
                                             int,
                                             operations_research::MPSolver&,
                                             const std::vector<operations_research::MPVariable*>&);

void H2O_J_MPSolver_CreateConstraints(DONNEES_MENSUELLES*,
                                      int,
                                      operations_research::MPSolver&,
                                      const std::vector<operations_research::MPVariable*>&);

void H2O_J_MPSolver_SolveAndRecover(DONNEES_MENSUELLES*,
                                     int,
                                     operations_research::MPSolver&,
                                     const std::vector<operations_research::MPVariable*>&);
} // namespace DoneesOptimisationJournaliere

#endif /* __SOLVER_H2O_J_FONCTIONS__ */
