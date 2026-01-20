// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_H2O_M_FONCTIONS__
#define __SOLVER_H2O_M_FONCTIONS__

namespace DonneesOptimisationMensuelle
{
DONNEES_ANNUELLES H2O_M_Instanciation(int);
void H2O_M_OptimiserUneAnnee(DONNEES_ANNUELLES&, int);
void H2O_M_Free(DONNEES_ANNUELLES&);

void H2O_M_ConstruireLesContraintes(DONNEES_ANNUELLES&);
void H2O_M_ConstruireLesVariables(DONNEES_ANNUELLES&);
void H2O_M_InitialiserBornesEtCoutsDesVariables(DONNEES_ANNUELLES&);
void H2O_M_InitialiserLeSecondMembre(DONNEES_ANNUELLES&);
void H2O_M_ResoudreLeProblemeLineaire(DONNEES_ANNUELLES&, int);
void H2O_M_AjouterBruitAuCout(DONNEES_ANNUELLES&);
} // namespace DonneesOptimisationMensuelle
#endif /* __SOLVER_H2O_M_FONCTIONS__ */
