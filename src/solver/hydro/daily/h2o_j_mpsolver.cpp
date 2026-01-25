// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
#include <vector>

#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

#include "ortools/linear_solver/linear_solver.h"

namespace DoneesOptimisationJournaliere
{
using operations_research::MPObjective;
using operations_research::MPSolver;
using operations_research::MPVariable;

H2O_J_MPSOLVER_VARIABLES H2O_J_MPSolver_CreateVariables(DONNEES_MENSUELLES* DonneesMensuelles,
                                                        MPSolver& solver)
{
    const int NbPdt = DonneesMensuelles->NombreDeJoursDuMois;

    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;

    H2O_J_MPSOLVER_VARIABLES vars(NbPdt);

    const std::vector<double>& TurbineMax = DonneesMensuelles->TurbineMax;
    const std::vector<double>& TurbineMin = DonneesMensuelles->TurbineMin;

    // Expected variable ordering for the daily problem:
    //  - first NbPdt variables: turbine[pdt]
    //  - next NbPdt variables:  xi[pdt]
    //  - last 2 variables:      xi_plus, xi_moins
    // We set both names and bounds in a single pass, without relying on
    // CORRESPONDANCE_DES_VARIABLES.

    // Turbine and xi variables.
    for (int pdt = 0; pdt < NbPdt; ++pdt)
    {
        // Turbine bounds from physical min/max.
        const double xmax = TurbineMax[pdt];
        const double xmin = std::min(TurbineMax[pdt], TurbineMin[pdt]);

        const std::string nameTurbine = "turbine[" + std::to_string(pdt) + "]";
        vars.turbine[pdt] = solver.MakeNumVar(xmin, xmax, nameTurbine);
    }

    // Xi variables are non-negative slacks with no explicit upper bound.
    for (int pdt = 0; pdt < NbPdt; ++pdt)
    {
        const std::string nameXi = "xi[" + std::to_string(pdt) + "]";
        vars.xi[pdt] = solver.MakeNumVar(0.0, MPSolver::infinity(), nameXi);
    }

    vars.xiPlus = solver.MakeNumVar(0.0, MPSolver::infinity(), "xi_plus");
    vars.xiMoins = solver.MakeNumVar(0.0, MPSolver::infinity(), "xi_moins");

    return vars;
}

void H2O_J_MPSolver_SetObjectiveCoefficients(const H2O_J_MPSOLVER_VARIABLES& variables,
                                             MPSolver& solver)
{
    MPObjective* objective = solver.MutableObjective();
    objective->Clear();
    objective->SetMinimization();

    // Cost structure mirrors the legacy instanciation:
    // - objective is the sum of all xi[t]
    // - plus the two global deviation variables xiPlus and xiMoins.
    for (const auto* xi: variables.xi)
    {
        objective->SetCoefficient(xi, 1.0);
    }

    objective->SetCoefficient(variables.xiPlus, 1.0);
    objective->SetCoefficient(variables.xiMoins, 1.0);
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
        energyConstraint->SetCoefficient(variables.turbine[pdt], 1.0);
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

bool H2O_J_MPSolver_SolveAndRecover(DONNEES_MENSUELLES* DonneesMensuelles,
                                    MPSolver& solver,
                                    const H2O_J_MPSOLVER_VARIABLES& variables)
{
    PROBLEME_HYDRAULIQUE& ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;
    const auto status = solver.Solve();

    if (status == MPSolver::OPTIMAL || status == MPSolver::FEASIBLE)
    {
        // Recover turbine values directly from the corresponding MPVariables.
        const int NbPdt = DonneesMensuelles->NombreDeJoursDuMois;
        for (int pdt = 0; pdt < NbPdt; ++pdt)
        {
            const double value = variables.turbine[pdt]->solution_value();
            DonneesMensuelles->Turbine[pdt] = value;
        }
        return true;
    }
    else
    {
        return false;
    }
}

void H2O_J_MPSolver_Solve(DONNEES_MENSUELLES* DonneesMensuelles)
{
    if (!DonneesMensuelles)
    {
        return;
    }

    // Create a fresh MPSolver instance for the daily problem. We use the
    // Sirius LP backend by default as requested.

    auto solver = std::unique_ptr<MPSolver>(MPSolver::CreateSolver("sirius"));

    if (!solver)
    {
        DonneesMensuelles->ResultatsValides = NON;
        return;
    }

    // 1) Create variables and wire them to the legacy data structures.
    H2O_J_MPSOLVER_VARIABLES vars = H2O_J_MPSolver_CreateVariables(DonneesMensuelles, *solver);

    // 2) Set objective coefficients.
    H2O_J_MPSolver_SetObjectiveCoefficients(vars, *solver);

    // 3) Create constraints.
    H2O_J_MPSolver_CreateConstraints(DonneesMensuelles, vars, *solver);

    // 4) Solve and recover the solution into DONNEES_MENSUELLES.
    bool success = H2O_J_MPSolver_SolveAndRecover(DonneesMensuelles, *solver, vars);

    DonneesMensuelles->ResultatsValides = success ? OUI : NON;
}

} // namespace DoneesOptimisationJournaliere
