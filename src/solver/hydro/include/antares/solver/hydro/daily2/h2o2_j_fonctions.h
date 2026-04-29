// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_H2O2_J_FONCTIONS__
#define __SOLVER_H2O2_J_FONCTIONS__

DONNEES_MENSUELLES_ETENDUES H2O2_J_Instanciation();
void H2O2_J_OptimiserUnMois(DONNEES_MENSUELLES_ETENDUES&);
void H2O2_J_Free(DONNEES_MENSUELLES_ETENDUES&);
void H2O2_J_ConstruireLesContraintes(int,
                                     std::vector<int>&,
                                     std::vector<char>&,
                                     std::vector<int>&,
                                     std::vector<double>&,
                                     std::vector<int>&,
                                     CORRESPONDANCE_DES_VARIABLES_PB_ETENDU&);
void H2O2_j_ConstruireLesVariables(int,
                                   DONNEES_MENSUELLES_ETENDUES&,
                                   std::vector<double>&,
                                   std::vector<double>&,
                                   std::vector<int>&,
                                   std::vector<double*>&,
                                   CORRESPONDANCE_DES_VARIABLES_PB_ETENDU&);
void H2O2_J_apply_costs(const Hydro_problem_costs&, DONNEES_MENSUELLES_ETENDUES&);
void H2O2_J_InitialiserLesBornesdesVariables(DONNEES_MENSUELLES_ETENDUES&, int);
void H2O2_J_InitialiserLeSecondMembre(DONNEES_MENSUELLES_ETENDUES&, int);
void H2O2_J_ResoudreLeProblemeLineaire(DONNEES_MENSUELLES_ETENDUES&, int);

#endif /* __SOLVER_H2O2_J_FONCTIONS__ */
