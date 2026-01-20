// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

#include <cmath>
#include <limits>
#include <string>
#include <vector>

#include <antares/solver/utils/ortools_utils.h>

namespace DoneesOptimisationJournaliere
{
using operations_research::MPObjective;
using operations_research::MPSolver;
using operations_research::MPVariable;

namespace
{
inline double toOrtoolsLowerBound(double value, double linf)
{
    if (value <= -linf / 2.)
    {
        return -MPSolver::infinity();
    }
    return value;
}

inline double toOrtoolsUpperBound(double value, double linf)
{
    if (value >= linf / 2.)
    {
        return MPSolver::infinity();
    }
    return value;
}
} // namespace

void H2O_J_MPSolver_CreateVariables(DONNEES_MENSUELLES* DonneesMensuelles,
                                    int NumeroDeProbleme,
                                    MPSolver& solver,
                                    std::vector<MPVariable*>& variables)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;

    const int NbPdt = ProblemeHydraulique.NbJoursDUnProbleme[NumeroDeProbleme];

    const CORRESPONDANCE_DES_VARIABLES& CorrespondanceDesVariables
      = ProblemeHydraulique.CorrespondanceDesVariables[NumeroDeProbleme];

    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe
      = ProblemeHydraulique.ProblemeLineairePartieFixe[NumeroDeProbleme];
    PROBLEME_LINEAIRE_PARTIE_VARIABLE& ProblemeLineairePartieVariable
      = ProblemeHydraulique.ProblemeLineairePartieVariable[NumeroDeProbleme];

    const int NombreDeVariables = ProblemeLineairePartieFixe.NombreDeVariables;

    variables.clear();
    variables.resize(NombreDeVariables, nullptr);

    std::vector<std::string> variableNames(NombreDeVariables);

    for (int pdt = 0; pdt < NbPdt; ++pdt)
    {
        const int varTurbine = CorrespondanceDesVariables.NumeroDeVariableTurbine[pdt];
        variableNames[varTurbine] = "turbine[" + std::to_string(pdt) + "]";

        const int varXi = CorrespondanceDesVariables.NumeroDeLaVariableXi[pdt];
        variableNames[varXi] = "xi[" + std::to_string(pdt) + "]";
    }

    variableNames[CorrespondanceDesVariables.NumeroDeLaVariableXiPlus] = "xi_plus";
    variableNames[CorrespondanceDesVariables.NumeroDeLaVariableXiMoins] = "xi_moins";

    for (int var = 0; var < NombreDeVariables; ++var)
    {
        if (variableNames[var].empty())
        {
            variableNames[var] = "x[" + std::to_string(var) + "]";
        }
    }

    const double linf = LINFINI;

    for (int var = 0; var < NombreDeVariables; ++var)
    {
        double lb = toOrtoolsLowerBound(ProblemeLineairePartieVariable.Xmin[var], linf);
        double ub = toOrtoolsUpperBound(ProblemeLineairePartieVariable.Xmax[var], linf);

        variables[var] = solver.MakeNumVar(lb, ub, variableNames[var]);
    }
}

void H2O_J_MPSolver_SetObjectiveCoefficients(DONNEES_MENSUELLES* DonneesMensuelles,
                                             int NumeroDeProbleme,
                                             MPSolver& solver,
                                             const std::vector<MPVariable*>& variables)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;

    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe
      = ProblemeHydraulique.ProblemeLineairePartieFixe[NumeroDeProbleme];

    const int NombreDeVariables = ProblemeLineairePartieFixe.NombreDeVariables;

    MPObjective* objective = solver.MutableObjective();
    objective->Clear();
    objective->SetMinimization();

    for (int var = 0; var < NombreDeVariables; ++var)
    {
        const double coeff = ProblemeLineairePartieFixe.CoutLineaire[var];
        if (coeff != 0.0)
        {
            objective->SetCoefficient(variables[var], coeff);
        }
    }
}

void H2O_J_MPSolver_CreateConstraints(DONNEES_MENSUELLES* DonneesMensuelles,
                                      int NumeroDeProbleme,
                                      MPSolver& solver,
                                      const std::vector<MPVariable*>& variables)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;

    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe
      = ProblemeHydraulique.ProblemeLineairePartieFixe[NumeroDeProbleme];
    PROBLEME_LINEAIRE_PARTIE_VARIABLE& ProblemeLineairePartieVariable
      = ProblemeHydraulique.ProblemeLineairePartieVariable[NumeroDeProbleme];

    const int NombreDeContraintes = ProblemeLineairePartieFixe.NombreDeContraintes;

    const double inf = MPSolver::infinity();

    for (int row = 0; row < NombreDeContraintes; ++row)
    {
        const double rhs = ProblemeLineairePartieVariable.SecondMembre[row];
        const char sens = ProblemeLineairePartieFixe.Sens[row];

        double lb = -inf;
        double ub = inf;

        switch (sens)
        {
        case '=':
            lb = rhs;
            ub = rhs;
            break;
        case '>':
            lb = rhs;
            break;
        case '<':
            ub = rhs;
            break;
        default:
            // Fallback: treat as free row with no bounds
            break;
        }

        const std::string rowName = std::string("row[") + std::to_string(row) + "]";
        auto* const constraint = solver.MakeRowConstraint(lb, ub, rowName);

        const int start = ProblemeLineairePartieFixe.IndicesDebutDeLigne[row];
        const int count = ProblemeLineairePartieFixe.NombreDeTermesDesLignes[row];

        for (int idx = 0; idx < count; ++idx)
        {
            const int matrixIndex = start + idx;
            const int col = ProblemeLineairePartieFixe.IndicesColonnes[matrixIndex];
            const double coeff
              = ProblemeLineairePartieFixe.CoefficientsDeLaMatriceDesContraintes[matrixIndex];

            constraint->SetCoefficient(variables[col], coeff);
        }
    }
}

void H2O_J_MPSolver_SolveAndRecover(DONNEES_MENSUELLES* DonneesMensuelles,
                                     int NumeroDeProbleme,
                                     MPSolver& solver,
                                     const std::vector<MPVariable*>& variables)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;

    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe
      = ProblemeHydraulique.ProblemeLineairePartieFixe[NumeroDeProbleme];
    PROBLEME_LINEAIRE_PARTIE_VARIABLE& ProblemeLineairePartieVariable
      = ProblemeHydraulique.ProblemeLineairePartieVariable[NumeroDeProbleme];

    const int NombreDeVariables = ProblemeLineairePartieFixe.NombreDeVariables;

    const auto status = solver.Solve();

    if (status == MPSolver::OPTIMAL || status == MPSolver::FEASIBLE)
    {
        DonneesMensuelles->ResultatsValides = OUI;
        ProblemeLineairePartieVariable.ExistenceDUneSolution = OUI_SPX;

        for (int var = 0; var < NombreDeVariables; ++var)
        {
            const double value = variables[var]->solution_value();
            ProblemeLineairePartieVariable.X[var] = value;

            double* const pt
              = ProblemeLineairePartieVariable.AdresseOuPlacerLaValeurDesVariablesOptimisees[var];
            if (pt)
            {
                *pt = value;
            }
        }
    }
    else
    {
        DonneesMensuelles->ResultatsValides = NON;
        ProblemeLineairePartieVariable.ExistenceDUneSolution = NON_SPX;
    }
}

} // namespace DoneesOptimisationJournaliere
