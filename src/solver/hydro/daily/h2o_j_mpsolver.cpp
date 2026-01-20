// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
#include <vector>

#include <antares/solver/utils/ortools_utils.h>
#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

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

    const CORRESPONDANCE_DES_VARIABLES& CorrespondanceDesVariables = ProblemeHydraulique
                                                                       .CorrespondanceDesVariables
                                                                         [NumeroDeProbleme];

    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe = ProblemeHydraulique
                                                                  .ProblemeLineairePartieFixe
                                                                    [NumeroDeProbleme];
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

    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe = ProblemeHydraulique
                                                                  .ProblemeLineairePartieFixe
                                                                    [NumeroDeProbleme];

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
                                      const H2O_J_MPSOLVER_VARIABLES& variables,
                                      MPSolver& solver)
{
    const int NbPdt = DonneesMensuelles->NombreDeJoursDuMois;
    if (NbPdt <= 0)
    {
        return;
    }

    const double inf = MPSolver::infinity();

    // --- Contrainte somme des turbines = somme des cibles (clamped) ---
    double turbineMinSum = 0.0;
    double turbineMaxSum = 0.0;

    for (int pdt = 0; pdt < NbPdt; ++pdt)
    {
        turbineMinSum += DonneesMensuelles->TurbineMin[pdt];
        turbineMaxSum += DonneesMensuelles->TurbineMax[pdt];
    }

    const double turbineDuMois = DonneesMensuelles->TurbineDuMois;
    const double energyTarget = std::clamp(turbineDuMois, turbineMinSum, turbineMaxSum);

    auto* const energyConstraint = solver.MakeRowConstraint(energyTarget,
                                                            energyTarget,
                                                            "energy_month");

    for (int pdt = 0; pdt < NbPdt; ++pdt)
    {
        energyConstraint->SetCoefficient(turbineVariables[pdt], 1.0);
    }

    // --- turbine[t] + Xi[t] >= cible[t] ---
    for (int pdt = 0; pdt < NbPdt; ++pdt)
    {
        auto* const c = solver.MakeRowConstraint(DonneesMensuelles->TurbineCible[pdt], inf);
        c->SetCoefficient(variables.turbine[pdt], 1.0);
        c->SetCoefficient(variables.xi[pdt], 1.0);
    }

    // --- -turbine[t] + Xi[t] >= -cible[t] ---
    for (int pdt = 0; pdt < NbPdt; ++pdt)
    {
        auto* const c = solver.MakeRowConstraint(-DonneesMensuelles->TurbineCible[pdt], inf);
        c->SetCoefficient(variables.turbine[pdt], -1.0);
        c->SetCoefficient(variables.xi[pdt], 1.0);
    }

    // --- turbine[t] + xi_plus >= cible[t] ---
    for (int pdt = 0; pdt < NbPdt; ++pdt)
    {
        auto* const c = solver.MakeRowConstraint(DonneesMensuelles->TurbineCible[pdt], inf);
        c->SetCoefficient(variables.turbine[pdt], 1.0);
        c->SetCoefficient(variables.xiPlus, 1.0);
    }

    // --- -turbine[t] + xi_moins >= -cible[t] ---
    for (int pdt = 0; pdt < NbPdt; ++pdt)
    {
        auto* const c = solver.MakeRowConstraint(-DonneesMensuelles->TurbineCible[pdt], inf);
        c->SetCoefficient(variables.turbine[pdt], -1.0);
        c->SetCoefficient(variables.xiMoins, 1.0);
    }
}

void H2O_J_MPSolver_SolveAndRecover(DONNEES_MENSUELLES* DonneesMensuelles,
                                    int NumeroDeProbleme,
                                    MPSolver& solver,
                                    const std::vector<MPVariable*>& variables)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;

    PROBLEME_LINEAIRE_PARTIE_FIXE& ProblemeLineairePartieFixe = ProblemeHydraulique
                                                                  .ProblemeLineairePartieFixe
                                                                    [NumeroDeProbleme];
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

            double* const pt = ProblemeLineairePartieVariable
                                 .AdresseOuPlacerLaValeurDesVariablesOptimisees[var];
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
