// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_H2O_J_FONCTIONS__
#define __SOLVER_H2O_J_FONCTIONS__

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
} // namespace DoneesOptimisationJournaliere
#endif /* __SOLVER_H2O_J_FONCTIONS__ */
