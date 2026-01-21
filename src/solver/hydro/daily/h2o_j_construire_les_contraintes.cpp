// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

namespace DoneesOptimisationJournaliere
{
void H2O_J_ConstruireLesContraintes(int NbPdt,
                                    std::vector<int>& NumeroDeVariableTurbine,
                                    std::vector<int>& NumeroDeLaVariableXi,
                                    int NumeroDeLaVariableXiPlus,
                                    int NumeroDeLaVariableXiMoins,
                                    std::vector<int>& IndicesDebutDeLigne,
                                    std::vector<char>& Sens,
                                    std::vector<int>& NombreDeTermesDesLignes,
                                    std::vector<double>& CoefficientsDeLaMatriceDesContraintes,
                                    std::vector<int>& IndicesColonnes,
                                    CORRESPONDANCE_DES_CONTRAINTES& CorrespondanceDesContraintes)
{
    // The daily heuristic now builds constraints directly with OR-Tools
    // in H2O_J_MPSolver_CreateConstraints. This function is kept only to
    // preserve the public API; its implementation is no longer used.
    (void)NbPdt;
    (void)NumeroDeVariableTurbine;
    (void)NumeroDeLaVariableXi;
    (void)NumeroDeLaVariableXiPlus;
    (void)NumeroDeLaVariableXiMoins;
    (void)IndicesDebutDeLigne;
    (void)Sens;
    (void)NombreDeTermesDesLignes;
    (void)CoefficientsDeLaMatriceDesContraintes;
    (void)IndicesColonnes;
    (void)CorrespondanceDesContraintes;
}
} // namespace DoneesOptimisationJournaliere
